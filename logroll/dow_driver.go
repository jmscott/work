package logroll

import (
	"os"
	"time"
)

//  need to use interface?
var dow_driver = &driver{
	name: "Dow",

	open:      (*Roller).dow_open,
	close:     (*Roller).dow_close,
	roll:      (*Roller).dow_roll,
	poll_roll: (*Roller).dow_poll_roll,
}

func (roll *Roller) dow_path(now time.Time) string {
	dow := now.Weekday().String()[0:3]
	return roll.directory +
		string(os.PathSeparator) +
		roll.base_name +
		"-" +
		dow +
		"." +
		roll.file_suffix
}

func (roll *Roller) dow_open() (err error) {

	dow := time.Now().Weekday().String()[0:3]
	path := roll.dow_path(time.Now())

	mode := os.O_APPEND | os.O_CREATE | os.O_WRONLY
	roll.file, err = os.OpenFile(path, mode, roll.file_perm)
	if err != nil {
		return err
	}
	roll.path = path

	roll.driver_data = dow
	return nil
}

func (roll *Roller) dow_close() error {

	if roll.file == nil {
		return nil
	}
	f := roll.file
	roll.file = nil
	roll.driver_data = ""
	return f.Close()
}

func (roll *Roller) dow_poll_roll(now time.Time) (bool, error) {

	if now.Weekday().String()[0:3] == roll.driver_data.(string) {
		return false, nil
	}
	return true, nil
}

func (roll *Roller) dow_roll(now time.Time) error {

	err := roll.Close()
	if err != nil {
		return err
	}

	path := roll.dow_path(now)
	mode := os.O_TRUNC | os.O_CREATE | os.O_WRONLY
	roll.file, err = os.OpenFile(path, mode, roll.file_perm)
	if err != nil {
		return err
	}
	roll.path = path
	roll.driver_data = now.Weekday().String()[0:3]
	return nil
}