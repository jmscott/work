package logger

import (
	"testing"
	"time"
)

func Test(t *testing.T) {

	log, err := Open("test", "Dow")
	if err != nil {
		t.Fatalf("NewLogger() failed: %s", err)
	}

	err = log.Open()
	if err != nil {
		t.Fatalf("Open() failed: %s", err)
	}

	// test print functions

	log.INFO("INFO: hi")
	log.WARN("hi")
	log.ERROR("hi")

	//  test heartbeat

	log.INFO("sleep 11s")
	time.Sleep(11 * time.Second)
	log.INFO("awoke from 11s sleep")

	defer log.Close()
}
