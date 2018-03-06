// Package logroll implements a very simple file roller for long running server
// processes.
package logroll

import (
	"errors"
	"fmt"
	"os"
	"time"
)

//  A rollable file.
type Roller struct {
	base_name string

	directory   string
	path        string
	file_perm   os.FileMode
	file        *os.File
	file_suffix string

	read_c    chan ([]byte)
	request_c chan (chan interface{})
	done_c    chan (interface{})

	poll_roll_tick time.Duration

	//  rate to roll file for hz driver
	hz_tick time.Duration

	driver      *driver
	driver_data interface{}
}

type driver struct {
	name      string
	open      func(*Roller) error
	close     func(*Roller) error
	roll      func(*Roller, time.Time) error
	poll_roll func(*Roller, time.Time) (bool, error)
}

type roll_option func(roll *Roller) roll_option

var roller_default = Roller{
	directory:      ".",
	poll_roll_tick: 3 * time.Second,
	file_perm:      00640,
	file_suffix:    "log",
	hz_tick:        10 * time.Minute,
}

func (roll *Roller) read() {

	for {
		now := time.Now()
		select {

		case <-roll.done_c:
			return

		//  request to roll the file

		case reply := <-roll.request_c:
			err := roll.driver.roll(roll, now)
			if err == nil {
				reply <- new(interface{})
				continue
			}
			roll.panic("roll", err)

		//  message to write to roll file

		case msg := <-roll.read_c:
			_, err := roll.file.Write(msg)
			if err == nil {
				continue
			}
			os.Stderr.Write(msg)
			roll.panic("Write", err)
		}
	}
}

func (roll *Roller) poll_roll() {

	tick := time.NewTicker(roll.poll_roll_tick)
	for now := range tick.C {
		rollable, err := roll.driver.poll_roll(roll, now)

		if err != nil {
			roll.panic("poll_roll", err)
		}
		if rollable == false {
			continue
		}
		reply := make(chan interface{})
		roll.request_c <- reply
		<-reply
	}
}

func (roll *Roller) panic(what string, err error) {

	var driver_name string

	if roll.driver != nil {
		driver_name = roll.driver.name
	}
	fmt.Fprintf(
		os.Stderr,
		"%s: PANIC: %s(%s): Logger.%s() failed: %s\n",
		time.Now().Format("2006/01/02 15:04:05"),
		roll.base_name,
		driver_name,
		what,
		err,
	)
	panic(err)
}

func ValidHzTick(tick time.Duration) error {
	if tick < time.Second {
		return errors.New(fmt.Sprintf("hz tick < 1s: %s", tick))
	}
	return nil
}

func HzTick(tick time.Duration) roll_option {
	return func(roll *Roller) roll_option {
		previous := roll.hz_tick
		roll.hz_tick = tick
		return HzTick(previous)
	}
}

//  Directory() sets the directory containing the rolled file.
//  The default directory is "."
func Directory(directory string) roll_option {
	return func(roll *Roller) roll_option {
		previous := roll.directory
		roll.directory = directory
		return Directory(previous)
	}
}

func FileSuffix(file_suffix string) roll_option {
	return func(roll *Roller) roll_option {
		previous := roll.file_suffix
		roll.file_suffix = file_suffix
		return FileSuffix(previous)
	}
}

func (roll *Roller) Options(options ...roll_option) (roll_option, error) {

	var previous roll_option

	for _, opt := range options {
		previous = opt(roll)
	}
	return previous, nil
}

// Open a File Roller with a base file name, a driver and variable number of
// options.  The base file is the prefix for the rolled file names.  The driver
// can be "Dow" or "Hz".  The "Dow" driver creates and rolls files with the
// day of the week embedded the file name.  The "Hz" driver creates a file
// named <base_name>.<suffix> and then rolls to a file
// <base_name>-YYYYMMDD_hhmmss[-+]hrmi.<suffix> at regular rate of time.
// An error is returned or the Roller is ready to accept bytes via
// logroll.Write().
func OpenRoller(
	base_name,
	driver_name string,
	options ...roll_option,
) (*Roller, error) {
	if base_name == "" {
		return nil, errors.New("empty roll base name")
	}
	roll := &Roller{}
	*roll = roller_default
	roll.base_name = base_name

	switch driver_name {
	case "Dow":
		roll.driver = dow_driver
	case "Hz":
		roll.driver = hz_driver
	default:
		return nil, errors.New("unknown roll driver: " + driver_name)
	}

	_, err := roll.Options(options...)
	if err != nil {
		return nil, err
	}

	//  open specific logger

	err = roll.driver.open(roll)
	if err != nil {
		return nil, err
	}

	//  request to close log file

	roll.done_c = make(chan (interface{}))

	//  open the logger message channel

	roll.read_c = make(chan ([]byte))

	//  open the roll request chan

	roll.request_c = make(chan (chan interface{}))

	go roll.read()

	go roll.poll_roll()

	return roll, nil
}

func (roll *Roller) Close() error {

	if roll == nil || roll.driver == nil {
		return nil
	}
	if roll.file != nil {
		roll.done_c <- new(interface{})

		//  Note: in the past Sync() seems to fix issues with incomplete
		//        log writes.
		//roll.file.Sync()
	}
	return roll.driver.close(roll)
}

func (roll *Roller) Write(msg []byte) error {
	roll.read_c <- msg
	return nil
}
