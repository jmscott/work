package logroll_test

import (
	"github.com/jmscott/work/logroll"
	"time"
)

func Example() error {
	// Open a file
	//
	//	data/example.txn
	//
	// that rolls every 10 mminutes to file named
	//
	//	data/example-YYYYMMDD_HHMMSS[-+]hhmm.txn
	//
	// where [-+]hhmm are the timezone offsets in hours and minutes.
	//
	roll, err := logroll.OpenRoller(
			"example",
			"Hz",
			logroll.HzTick(10 * time.Minute),
			logroll.Directory("data"),
			logroll.FileSuffix("txn"),
	)
	if err != nil {
		return err
	}
	return roll.Write([]byte(
		time.Now().Format("2006/01/02 15:04:05") + "\t" + "OPEN-TXN\n"),
	)
}
