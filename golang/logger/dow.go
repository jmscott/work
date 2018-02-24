package logger

import (
	"os"
	"time"
)

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
