package logger

import (
	"testing"
	"time"
)

const heartbeat_tick = 2 * time.Second

func test_open(what string, t *testing.T, options ...option) (*Logger, bool) {

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
			HeartbeatTick(heartbeat_tick),
	)
	if !ok {
		return
	}
	defer log.Close()

	//  test heartbeat

	sleep_tick := heartbeat_tick + time.Second
	log.INFO("sleep tick: %s", sleep_tick)
	time.Sleep(sleep_tick)
	log.INFO("awoke from %s sleep", sleep_tick)
}

func TestDirectory(t *testing.T) {
	log, ok := test_open(
			"TestDirectory",
			t,
			HeartbeatTick(heartbeat_tick),
			Directory("tmp"),
	)
	if !ok {
		return
	}
	defer log.Close()
}
