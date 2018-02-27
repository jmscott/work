package logroll

import (
	"fmt"
	"os"
	"time"
)

var hz_driver = &roll_driver{
	name: "Hz",

	open:      (*Roller).hz_open,
	close:     (*Roller).hz_close,
	roll:      (*Roller).hz_roll,
	poll_roll: (*Roller).hz_poll_roll,
}

var zero_time time.Time

func (roll *Roller) hz_open() (err error) {

	now := time.Now()
	path := roll.dow_path(now)

	mode := os.O_APPEND | os.O_CREATE | os.O_WRONLY
	roll.file, err = os.OpenFile(path, mode, roll.file_perm)
	if err != nil {
		return err
	}
	roll.path = path

	roll.driver_data = now
	return nil
}

func (roll *Roller) hz_close() error {

	if roll.file == nil {
		return nil
	}
	f := roll.file
	roll.file = nil
	roll.driver_data = zero_time
	return f.Close()
}

func (roll *Roller) hz_poll_roll(now time.Time) (bool, error) {

	if now.Weekday().String()[0:3] == roll.driver_data.(string) {
		return false, nil
	}
	return true, nil
}

func tzo2file_name(sec int) string {

	min := (sec % 3600) / 60
	if min < 0 {
		min = -min
	}
	return fmt.Sprintf("%+03d%02d", sec/3600, min)
}

func (roll *Roller) hz_roll(now time.Time) error {

	err := roll.file.Close()
	if err != nil {
		return err
	}
	_, tzo := time.Now().Zone()

	path := roll.directory +
		string(os.PathSeparator) +
		roll.name +
		"-" +
		fmt.Sprintf("%d%02d%02d_%02d%02d%02d%s",
			now.Year(),
			now.Month(),
			now.Day(),
			now.Hour(),
			now.Minute(),
			now.Second(),
			tzo2file_name(tzo),
		) +
		"." +
		roll.file_suffix

	mode := os.O_TRUNC | os.O_CREATE | os.O_WRONLY
	roll.file, err = os.OpenFile(path, mode, roll.file_perm)
	if err != nil {
		return err
	}
	roll.path = path
	roll.driver_data = now
	return nil
}

func (roll *Roller) hz_path(now time.Time) string {
	_, tzo := now.Zone()
	return roll.directory +
		string(os.PathSeparator) +
		roll.name +
		"-" +
		fmt.Sprintf("%d%02d%02d_%02d%02d%02d%s",
			now.Year(),
			now.Month(),
			now.Day(),
			now.Hour(),
			now.Minute(),
			now.Second(),
			tzo2file_name(tzo),
		) +
		".log"
}
