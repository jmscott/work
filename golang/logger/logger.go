package logger

import (
	"errors"
	"fmt"
	"os"
	"time"
)

type Logger struct {
	name		string

	directory	string
	log_path	string
	log_file_perm	os.FileMode
	log_file	*os.File

	read_c		chan(string)
	roll_c		chan(chan interface{})
	close_c		chan(interface{})

	heartbeat_tick	time.Duration
	poll_roll_tick	time.Duration

	client_data	interface{}
	driver		*driver
	driver_data	interface{}
}

type driver struct {
	name		string
	open		func(*Logger) error
	close		func(*Logger) error
	roll		func(*Logger, time.Time) error
	poll_roll	func(*Logger, time.Time) (bool, error)
}

type option func(log *Logger) option

var logger_default = Logger{
	directory:	".",
	heartbeat_tick:	10 * time.Second,
	poll_roll_tick:	3 * time.Second,
	log_file_perm:	00640,
}

func (log *Logger) read() {

	for {
		now := time.Now()
		select {
		case <- log.close_c:
			return
		case reply := <- log.roll_c:
			err := log.driver.roll(log, now)
			if err == nil {
				reply <- new(interface{})
				continue
			}

			log.PANIC("roll", err)
		case s := <- log.read_c:
			msg := []byte(
					now.Format("2006/01/02 15:04:05") +
					": " +
					s +
					"\n",
			)

			_, err := log.log_file.Write(msg)
			if err == nil {
				continue
			}
			os.Stderr.Write(msg)
			log.PANIC("Write", err)
		}
	}
}

func (log *Logger) poll_roll() {

	tick := time.NewTicker(log.poll_roll_tick)
	for now := range tick.C {
		rollable, err := log.driver.poll_roll(log, now)

		if err != nil {
			log.PANIC("poll_roll", err)
		}
		if rollable == false {
			continue
		}
		log.INFO("request to log file")
		reply := make(chan interface{})
		log.roll_c <- reply
		log.INFO("log file rolled")
	}
}

//  Check heart beat tick

func ValidHeartbeatTick(tick time.Duration) error {
	if tick < 0 {
		return errors.New(
			fmt.Sprintf("heart beat tick < 0: %s", tick))
	}
	return nil
}

func HeartbeatTick(tick time.Duration) option {
	return func(log *Logger) option {
		previous := log.heartbeat_tick
		log.heartbeat_tick = tick
		return HeartbeatTick(previous)
	}
}

func Directory(directory string) option {
	return func(log *Logger) option {
		previous := log.directory
		log.directory = directory
		return Directory(previous)
	}
}

func (log *Logger) Options(options ...option) (option, error) {
	
	var previous option
	for _, opt := range options {
		previous = opt(log)
	}
	return previous, nil
}

func Open(name, driver_name string, options ...option) (*Logger, error) {
	if name == "" {
		return nil, errors.New("empty log name")
	}
	log := &Logger{}
	*log = logger_default
	log.name = name

	switch driver_name {
	case "Dow":
		log.driver = dow_driver
	default:
		return nil, errors.New("unknown log driver: " + driver_name)
	}

	_, err := log.Options(options...)
	if err != nil {
		return nil, err
	}

	//  open specific logger

	err = log.driver.open(log)
	if err != nil {
		return nil, err
	}

	//  request to close log file

	log.close_c = make(chan(interface{}))

	//  open the logger message channel

	log.read_c = make(chan(string))

	//  open the roll request chan

	log.roll_c = make(chan(chan interface{}))

	go log.read()

	log.INFO("hello, world")
	log.INFO("directory: %s", log.directory)
	log.INFO("log path: %s", log.log_path)

	//  Note: need to burb out log file
	log.INFO("heartbeat tick: %s", log.heartbeat_tick)

	go log.poll_roll()
	go log.heartbeat()

	return log, nil
}

//  Note: what happens if logger is closed!
func (log *Logger) heartbeat() {

	tick := time.NewTicker(log.heartbeat_tick)
	for range tick.C {

		log.INFO("alive")
	}
}

func (log *Logger) Close() (error) {

	if log == nil || log.driver == nil {
		return nil
	}
	if log.log_file != nil {
		log.INFO("good bye, cruel world")
		log.close_c <- new(interface{})
	}
	return log.driver.close(log)
}

func (log *Logger) INFO(format string, args ...interface{}) {
	log.read_c <- fmt.Sprintf(format, args...)
}

func (log *Logger) ERROR(format string, args ...interface{}) {
	log.read_c <- fmt.Sprintf("ERROR: " + format, args...)
}

func (log *Logger) WARN(format string, args ...interface{}) {
	log.read_c <- fmt.Sprintf("WARN: " + format, args...)
}

func (log *Logger) PANIC(what string, err error) {

	var driver_name string

	if log.driver != nil {
		driver_name = log.driver.name
	}
	fmt.Fprintf(
		os.Stderr,
		"%s: PANIC: %s(%s): Logger.%s() failed: %s\n",
		time.Now().Format("2006/01/02 15:04:05"),
		log.name,
		driver_name,
		what,
		err,
	)
	panic(err)
}
