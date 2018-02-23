package logger

import (
	"testing"
	"time"
)

func Test(t *testing.T) {

	log, err := Open("test", "Dow", HeartbeatPause(2 * time.Second))
	if err != nil {
		t.Fatalf("Open() failed: %s", err)
	}

	// test print functions

	log.INFO("INFO: hi")
	log.WARN("hi")
	log.ERROR("hi")

	//  test heartbeat

	log.INFO("sleep 3s")
	time.Sleep(3 * time.Second)
	log.INFO("awoke from 3s sleep")

	defer log.Close()
}
