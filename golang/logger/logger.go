package logger

import (
	"errors"
	"fmt"
	"time"
)

type Logger struct {
	roll           *Roller
	heartbeat_tick time.Duration
}

type log_option func(log *Logger) log_option

var logger_default = Logger{
	heartbeat_tick: 10 * time.Second,
}

func ValidHeartbeatTick(tick time.Duration) error {
	if tick < 0 {
		return errors.New(
			fmt.Sprintf("heart beat tick < 0: %s", tick))
	}
	return nil
}

func HeartbeatTick(tick time.Duration) log_option {
	return func(log *Logger) log_option {
		previous := log.heartbeat_tick
		log.heartbeat_tick = tick
		return HeartbeatTick(previous)
	}
}

func (log *Logger) Options(options ...log_option) (log_option, error) {

	var previous log_option
	for _, opt := range options {
		previous = opt(log)
	}
	return previous, nil
}

func OpenLogger(roll *Roller, options ...log_option) (*Logger, error) {
	log := &Logger{}
	*log = logger_default
	log.roll = roll

	_, err := log.Options(options...)
	if err != nil {
		return nil, err
	}

	log.INFO("hello, world")
	log.INFO("directory: %s", roll.directory)
	log.INFO("log path: %s", roll.path)
	log.INFO("poll roll tick: %s", roll.poll_roll_tick)
	log.INFO("hz tick: %s", roll.hz_tick)

	log.INFO("heartbeat tick: %s", log.heartbeat_tick)
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

func (log *Logger) Close() error {

	if log.roll == nil {
		return nil
	}
	err := log.roll.Close()
	log.roll = nil
	return err
}

func (log *Logger) INFO(format string, args ...interface{}) {
	log.roll.read_c <- []byte(
		time.Now().Format("2006/01/02 15:04:05") +
			": " +
			fmt.Sprintf(format, args...) +
			"\n",
	)
}

func (log *Logger) ERROR(format string, args ...interface{}) {
	log.roll.read_c <- []byte(
		time.Now().Format("2006/01/02 15:04:05") +
			": " +
			fmt.Sprintf("ERROR: "+format, args...) +
			"\n",
	)
}

func (log *Logger) WARN(format string, args ...interface{}) {
	log.roll.read_c <- []byte(
		time.Now().Format("2006/01/02 15:04:05") +
			": " +
			fmt.Sprintf("WARN: "+format, args...) +
			"\n",
	)
}
