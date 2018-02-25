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
	rollable:	(*Logger).dow_rollable,
}

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
	log.log_path = path
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

func (log *Logger) dow_rollable(now time.Time) (bool, error) {
	return false, nil
}

func (log *Logger) dow_roll() error {
	
	return nil
}
