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

	driver		*driver
	file		*os.File
	message_c	chan(string)
	close_c		chan(interface{})
}

type driver struct {
	name		string
	open		func(*Logger) error
	close		func(*Logger) error
}

type option func(log *Logger) option

func (log *Logger) Option(opts ...option) (previous option) {

	for _, opt := range opts {
		previous = opt(log)
	}
	return previous
}

func (log *Logger) dow_open() (err error) {

	path := log.directory + "/" +
			log.name +
			"-" +
			time.Now().Weekday().String()[0:3] +
			".log"

	mode := os.O_APPEND | os.O_CREATE | os.O_WRONLY
	log.file, err = os.OpenFile(path, mode, 00640)
	if err != nil {
		return err
	}
	return nil
}

func (log *Logger) dow_close() error {

	if log.file == nil || log.file == os.Stderr {
		return nil
	}
	f := log.file
	log.file = nil
	return f.Close()
}

func (log *Logger) read() {

	for {
		select {
		case <- log.close_c:
			return
		case s := <- log.message_c:
			now := time.Now().Format("2006/01/02 15:04:05")
			msg := []byte(now + ": " + s + "\n")

			_, err := log.file.Write(msg)

			//  write to file failed, switch output to stderr.

			if err != nil {
				fmt.Fprintf(
					os.Stderr,
					"%s: PANIC: Logger: " +
						"Write(%s) failed: %s\n",
					now,
					log.name,
					err,
				)
				os.Stderr.Write(msg)
				log.file = os.Stderr
			}
		}
	}
}

func NewLogger(name, driver_name string, options ...option) (*Logger, error) {
	if name == "" {
		return nil, errors.New("empty log name")
	}
	if driver_name != "Dow" {
		return nil, errors.New("unknown log driver: " + driver_name)
	}

	log := &Logger{
		name:		name,
		directory:	".",
		driver:		&driver{
					name:	driver_name,
					open:	(*Logger).dow_open,
					close:	(*Logger).dow_close,
				},
	}

	for _, opt := range options {
		if opt(log) == nil {
			return nil, errors.New("error setting option")
		}
	}
	return log, nil
}

func (log *Logger) Open() error {

	if log == nil {
		return errors.New("Logger is null")
	}
	if log.driver == nil {
		return errors.New("NewLogger not called")
	}
	err := log.driver.open(log)
	if err != nil {
		return err
	}

	//  log file is closing
	log.close_c = make(chan(interface{}))

	//  open the logger message channel
	log.message_c = make(chan(string))

	go log.read()
	log.message_c <- "hello, world"

	return nil
}

func (log *Logger) Close() (error) {

	if log == nil || log.driver == nil {
		return nil
	}
	if log.file != nil {
		log.message_c <- "good bye, cruel world"
		log.close_c <- new(interface{})
	}
	return log.driver.close(log)
}
