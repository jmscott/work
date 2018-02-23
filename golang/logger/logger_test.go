package logger

import "testing"

func Test(t *testing.T) {

	log, err := NewLogger(
			"test",
			"Dow",
		    )
	if err != nil {
		t.Fatalf("NewLogger() failed: %s", err)
	}

	err = log.Open()
	if err != nil {
		t.Fatalf("Open() failed: %s", err)
	}
	log.INFO("INFO: test: hi")
	log.WARN("test: hi")
	log.ERROR("test: hi")
	defer log.Close()
}
