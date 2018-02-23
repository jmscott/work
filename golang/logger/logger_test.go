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
	defer log.Close()
}
