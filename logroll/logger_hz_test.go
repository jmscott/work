package logroll

import (
	"testing"
	"time"
)

const hz_heartbeat_tick = 2 * time.Second

func TestHzHeartbeat(t *testing.T) {

	roll, err := OpenRoller("test", "Hz", HzTick(time.Minute))
	if err != nil {
		t.Fatalf("OpenRoller() failed: %s", err)
		return
	}

	log, err := OpenLogger(roll, HeartbeatTick(hz_heartbeat_tick))
	if err != nil {
		t.Fatalf("OpenLogger() failed: %s", err)
		return
	}

	// test print functions

	log.INFO("TestHzHeartbeat: INFO: hi")
	log.WARN("TestHzHeartbeat: hi")
	log.ERROR("TestHzHeartbeat: hi")

	defer log.Close()

	//  test heartbeat

	sleep_tick := 3 * time.Minute
	log.INFO("sleep tick: %s", sleep_tick)
	time.Sleep(sleep_tick)
	log.INFO("awoke from %s sleep", sleep_tick)
}
