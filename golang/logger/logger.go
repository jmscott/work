package logger

import (
	"errors"
	"fmt"
	"os"
	"time"
)

const (
	log_file_perm = 00640
	heartbeat_pause_default = 10 * time.Second
)

var logger_default = Logger{
	directory:	".",
	heartbeat_pause:heartbeat_pause_default,
}

type Logger struct {
	name		string
	directory	string

	driver		*driver
	file		*os.File
	message_c	chan(string)
	close_c		chan(interface{})

	heartbeat_pause	time.Duration

	client_data	interface{}
}

type driver struct {
	name		string
	open		func(*Logger) error
	close		func(*Logger) error
}

type option func(log *Logger) option

func (log *Logger) dow_open() (err error) {

	path := log.directory + string(os.PathSeparator) +
			log.name +
			"-" +
			time.Now().Weekday().String()[0:3] +
			".log"

	mode := os.O_APPEND | os.O_CREATE | os.O_WRONLY
	log.file, err = os.OpenFile(path, mode, log_file_perm)
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
				panic(err)
			}
		}
	}
}

//  Check heart beat pause

func CheckHeartbeatPause(pause time.Duration) error {
	if pause < 0 {
		return errors.New(
			fmt.Sprintf("heart beat pause < 0: %s", pause))
	}
	return nil
}

func HeartbeatPause(pause time.Duration) option {
	return func(log *Logger) option {
		previous := log.heartbeat_pause
		log.heartbeat_pause = pause
		return HeartbeatPause(previous)
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
	if driver_name != "Dow" {
		return nil, errors.New("unknown log driver: " + driver_name)
	}

	log_df := logger_default
	log := &log_df

	log.name = name
	log.driver = &driver{
				name:	driver_name,
				open:	(*Logger).dow_open,
				close:	(*Logger).dow_close,
	}

	//  reset options

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
	log.message_c = make(chan(string))

	go log.read()

	log.INFO("hello, world")

	//  Note: need to burb out log file
	log.INFO("heartbeat pause: %s", log.heartbeat_pause)

	go log.heartbeat()

	return log, nil
}

func (log *Logger) heartbeat() {

	tick := time.NewTicker(log.heartbeat_pause)
	for range tick.C {
		log.INFO("alive")
	}
}

func (log *Logger) Close() (error) {

	if log == nil || log.driver == nil {
		return nil
	}
	if log.file != nil {
		log.INFO("good bye, cruel world")
		log.close_c <- new(interface{})
	}
	return log.driver.close(log)
}

func (log *Logger) INFO(format string, args ...interface{}) {
	log.message_c <- fmt.Sprintf(format, args...)
}

func (log *Logger) ERROR(format string, args ...interface{}) {
	log.message_c <- fmt.Sprintf("ERROR: " + format, args...)
}

func (log *Logger) WARN(format string, args ...interface{}) {
	log.message_c <- fmt.Sprintf("WARN: " + format, args...)
}
