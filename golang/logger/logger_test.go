package logger

import (
	"testing"
	"time"
)

const heartbeat_tick = 2 * time.Second

func test_open(
	what string,
	t *testing.T,
	options ...log_option,
) (*Logger, bool) {

	roll, err := OpenRoller("test", "Dow")
	if err != nil {
		t.Fatalf("OpenRoller(%s) failed: %s", what, err)
		return nil, false
	}

	log, err := OpenLogger(roll, options...)
	if err != nil {
		t.Fatalf("OpenLogger(%s) failed: %s", what, err)
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
