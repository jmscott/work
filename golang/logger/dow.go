package logger

import (
	"os"
	"time"
)

var dow_driver = &driver{
	name:		"Dow",

	open:		(*Logger).dow_open,
	close:		(*Logger).dow_close,
	roll:		(*Logger).dow_roll,
	poll_roll:	(*Logger).dow_poll_roll,
}

func (log *Logger) dow_open() (err error) {

	dow := time.Now().Weekday().String()[0:3]
	path := log.directory +
			string(os.PathSeparator) +
			log.name +
			"-" +
			dow +
			".log"

	mode := os.O_APPEND | os.O_CREATE | os.O_WRONLY
	log.log_file, err = os.OpenFile(path, mode, log.log_file_perm)
	if err != nil {
		return err
	}
	log.log_path = path

	log.driver_data = dow 
	return nil
}

func (log *Logger) dow_close() error {

	if log.log_file == nil || log.log_file == os.Stderr {
		return nil
	}
	f := log.log_file
	log.log_file = nil
	return f.Close()
}

func (log *Logger) dow_poll_roll(now time.Time) (bool, error) {

	if now.Weekday().String()[0:3] == log.driver_data.(string) {
		return false, nil
	}
	return true, nil
}

func (log *Logger) dow_roll(now time.Time) error {
	
	err := log.log_file.Close()
	if err != nil {
		return err
	}

	dow := now.Weekday().String()[0:3]
	roll_path := log.directory +
			string(os.PathSeparator) +
			log.name +
			"-" +
			dow +
			".log"
	mode := os.O_TRUNC | os.O_CREATE | os.O_WRONLY
	log.log_file, err = os.OpenFile(roll_path, mode, log.log_file_perm)
	if err != nil {
		return err
	}
	log.log_path = roll_path
	log.driver_data = dow
	return nil
}
