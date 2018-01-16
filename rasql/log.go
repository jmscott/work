package main

import (
	"bytes"
	"fmt"
	"os"
	"runtime"
	"sort"
	"syscall"
	"time"
)

var (
	log_name =		os.Args[0]
	log_file		*os.File
	log_roll_done		chan(bool)
	log_c 			chan(string)
)

//  open the log file log/<log_name>-Dow.log in append or truncate mode

func log_open(append_trunc int) {

	dow := (time.Now().Weekday().String())[0:3]

	log_path := fmt.Sprintf("log/%s-%s.log", log_name, dow)
	mode := append_trunc | syscall.O_CREAT | syscall.O_RDWR

	if lf, err := os.OpenFile(log_path, mode, 06666);  err != nil {
		log_file = nil
		at := ""
		if append_trunc == syscall.O_APPEND {
			at = "APPEND"
		} else {
			at = "CREATE"
		}
		die("open(log:%s) failed: %s", at, err)
	} else {
		log_file = lf
	}
	log_c =	make(chan(string))
}

func logger() {
	
	colon := []byte(": ")
	new_line := []byte("\n")
	buf := bytes.NewBuffer(make([]byte, 1024))

	fill := func(msg string) {
		buf.Reset()

		buf.Write([]byte(time.Now().Format("2006/01/02 15:04:05")))
		buf.Write(colon)
		buf.Write([]byte(msg))
		buf.Write(new_line)
	}

	//  read log messages from global channel
	for msg := range log_c {
		if msg == "" {
			log_roll()
		}
		fill(msg)

		_, err := log_file.Write(buf.Bytes())
		if err != nil {
			os.Stderr.Write(buf.Bytes())
			fill("write(log_failed) failed: " + err.Error())
			os.Stderr.Write(buf.Bytes())
			panic(err)
		}
	}
}

func log_init(name string) {

	log_name = name		//  Note: '%' character be in log name

	log_open(syscall.O_APPEND)

	go logger()

	//  start log roll watcher that notices change of Dow

	go func() {
		today := time.Now().Weekday().String()[0:3]
		for {
			now := time.Now().Weekday().String()[0:3]
			if now != today {
				log("log roll watch: %s -> %s ...", today , now)
				log_c <- ""

				//  Note: need a timeout!
				<- log_roll_done

				log("log roll watch: rolled %s -> %s",
					today,
					now,
				)
				today = now
			}
			time.Sleep(3 * time.Second)
		}
	}()
	log("hello, world")
	log_env()
}

/*
 *  Synopsis:
 *	Format a time zone seconds into [+-]HHMM. suitable for file path
 *  Note:
 *	Do timezones exists which are not on minute boundaries?
 */
func tzo2file_name(sec int) string {
	
	min := (sec % 3600) / 60
	if min < 0 {
		min = -min
	}
	return fmt.Sprintf("%+03d%02d", sec / 3600, min)
}

func log_roll() {

	put := func (format string, args ...interface{}) {
		fmt.Fprintf(
			log_file,
			fmt.Sprintf("%s: log roll: " + format + "\n",
				time.Now().Format("2006/01/02 15:04:05"),
			),
			args...,
		)
	}

	yesterday := time.Now().Add(-time.Hour).Weekday().String()[0:3]
	today := time.Now().Weekday().String()[0:3]

	flip := func(before bool) {
		tz, sec := time.Now().Zone()

		if before {
			put("time zone (seconds UTC): %s (%d)", tz, sec)
		}
		put("log/%s-%s.log -> log/%s-%s.log",
			log_name,
			yesterday,
			log_name,
			today,
		)
		if !before {
			put("time zone (seconds UTC): %s (%d)", tz, sec)
		}
	}

	stat := func() {

		var m runtime.MemStats

		now := time.Now()
		runtime.ReadMemStats(&m)
		put("MemStat for %d routines, read in %s",
			runtime.NumGoroutine(),
			time.Since(now),
		)
		put("--")
		put("        Allocated in Use: %d bytes", m.Alloc)
		put("         Total Allocated: %d bytes", m.TotalAlloc)
		put("             From System: %d bytes", m.Sys)
		put("         Pointer Lookups: %d lookups", m.Lookups)
		put("                 Mallocs: %d mallocs", m.Mallocs)
		put("                   Frees: %d frees", m.Frees)
		put("   Heap Allocated in Use: %d bytes", m.HeapAlloc)
		put("        Heap From System: %d bytes", m.HeapSys)
		put("      Heap in Idle Spans: %d bytes", m.HeapIdle)
		put("   Heap in Non-Idle Span: %d bytes", m.HeapInuse)
		put("     Heap Released to OS: %d bytes", m.HeapReleased)
		put("Heap Total Alloc Objects: %d", m.HeapObjects)
		put("--")
	}

	stat()
	flip(true)

	if err := log_file.Close();  err != nil {
		log_file = nil
		die("close(roll log) failed: %s", err)
	}

	log_open(syscall.O_TRUNC)

	flip(false)
	stat()

	log_roll_done <- true
}

func log(format string, args ...interface{}) {
	if log_c != nil {
		log_c <- fmt.Sprintf(format, args...)
	} else {
		if log_name == "" {
			fmt.Fprintf(os.Stdout, format, args...)
		} else {
			fmt.Fprintf(os.Stdout, log_name + ": " + format,args...)
		}
	}
}

func log_env() {
	//  dump the process environment

	log("process environment ...")
	env := os.Environ()
	sort.Strings(env)
	for _, e := range env {
		log("  %s", e)
	}
}

func leave(exit_status int) {

	if db != nil {
		log("closing database ...")
		err := db.Close()
		db = nil
		if err != nil {
			log("ERROR: db.Close() failed", err)
		}
	}

	//  seems to force async i/o
	os.Stderr.Sync()

	if log_file != nil {
		log("good bye, cruel world")
		log_file.Sync()			//  force async i/o to os
		err := log_file.Close()
		log_file = nil
		if err != nil {
			fmt.Fprintf(
				os.Stderr,
				"\n%s: rasqld: ERROR: " +
				  "close(log file) failed: %s\n",
				time.Now().Format("2006/01/02 15:04:05"),
				err,
			)
		}
	}
	os.Exit(1)
}

func die(format string, args ...interface{}) {

	if log_file == nil {
		os.Stderr.Write([]byte(
			fmt.Sprintf("ERROR: " + format + "\n", args...),
		))
	} else {
		log("ERROR: " + format, args...)
	}
	leave(1)
}
