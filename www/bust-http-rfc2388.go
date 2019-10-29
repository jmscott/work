/*
 *  Synopsis
 *	Parse stdin as multipart mime message according to RFC2388 to json.
 *  Usage:
 *	WORK_DIR=/tmp/bust-http-rfc2388-$$.d
 *	mkdir $WORK_DIR
 *	cd $WORK_DIR
 *	bust-http-rfc2388 <boundary> >request.json
 *  Note:
 *	Ought to rename to bust-http-rfc2388.go
 */
package main

import (
	"fmt"
	"io"
	"io/ioutil"
	"mime/multipart"
	"os"
	"syscall"
)

var (
	prog = "bust-http-rfc2388"
	stderr = os.NewFile(uintptr(syscall.Stderr), "/dev/stderr")
	stdin = os.NewFile(uintptr(syscall.Stdin), "/dev/stdin")
)

func die(format string, args ...interface{}) {

	fmt.Fprintf(
		stderr,
		"%s: ERROR: %s\n",
		prog,
		fmt.Sprintf(format, args...),
	)
	os.Exit(1)
}

func main() {
	if (len(os.Args) != 2) {
		die("wrong number of args: got %d, expected 2", len(os.Args))
	}
	mr := multipart.NewReader(stdin, os.Args[1])
	for {
		p, err := mr.NextPart()
		if err == io.EOF {
			break
		}
		if err != nil {
			die("multipart.NextPart() failed: %s", err)
		}
		slurp, err := ioutil.ReadAll(p)
		if err != nil {
			die("ioutil.ReadAll(mime part) failed: %s", err)
		}
		print("slurp: %q\n", slurp)
	}
	os.Exit(0)
}
