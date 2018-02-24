package logger

import (
	"testing"
	"time"
)

const heartbeat_pause = 2 * time.Second

func test_open(
	what string,
	t *testing.T,
	options ...option,
) (*Logger, bool) {

	log, err := Open("test", "Dow", options...)
	if err != nil {
		t.Fatalf("Open(%s) failed: %s", what, err)
		return nil, false
	}

	// test print functions

	log.INFO("INFO: %s: hi", what)
	log.WARN("%s: hi", what)
	log.ERROR("%s: hi", what)
	return log, true
}

func TestHeartbeat(t *testing.T) {

	log, ok := test_open(
			"TestHeartbeat",
			t,
			HeartbeatPause(heartbeat_pause),
	)
	if !ok {
		return
	}
	defer log.Close()

	//  test heartbeat

	sleep_pause := heartbeat_pause + time.Second
	log.INFO("sleep %s", sleep_pause)
	time.Sleep(sleep_pause)
	log.INFO("awoke from %s sleep", sleep_pause)
}

func TestDirectory(t *testing.T) {
	log, ok := test_open(
			"TestDirectory",
			t,
			HeartbeatPause(heartbeat_pause),
			Directory("tmp"),
	)
	if !ok {
		return
	}
	defer log.Close()
}
