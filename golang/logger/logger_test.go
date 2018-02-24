package logger

import (
	"testing"
	"time"
)

const heartbeat_pause = 2 * time.Second

func Test(t *testing.T) {

	log, err := Open("test", "Dow", HeartbeatPause(heartbeat_pause))
	if err != nil {
		t.Fatalf("Open() failed: %s", err)
	}

	// test print functions

	log.INFO("INFO: hi")
	log.WARN("hi")
	log.ERROR("hi")

	//  test heartbeat

	sleep_pause := heartbeat_pause + time.Second
	log.INFO("sleep %s", sleep_pause)
	time.Sleep(sleep_pause)
	log.INFO("awoke from %s sleep", sleep_pause)

	defer log.Close()
}
