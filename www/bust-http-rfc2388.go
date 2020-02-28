/*
 *  Synopsis
 *	Parse stdin and extract files in rfc2388 multipart mime message body.
 *  Description:
 *	Parse standard input as a multipart mime message, extracting the
 *	mime parts as files in the current directory.  A json summary of the
 *	http request is written to REQUEST.json.
 *
 *	bust-http-rfc2388 is intended for cgi-bin programs, invoked after
 *	the outer-most mime headers have been parsed.
 *  Usage:
 *	WORK_DIR=/tmp/bust-http-rfc2388-$$.d
 *	mkdir $WORK_DIR
 *	cd $WORK_DIR
 *	bust-http-rfc2388 <boundary> >REQUEST.json
 *  Exit Status:
 *	10	failure		#  golang snatches codes 1-3 for panic()
 *  Note:
 *	The odd value of 10 for an exit status is used since go reserves
 *	1-3 for panics.  golang does not insure all panics can be caught.
 */
package main

import (
	"encoding/json"
	"fmt"
	"mime/multipart"
	"os"
	"syscall"
)

var (
	prog = "bust-http-rfc2388"
	stderr = os.NewFile(uintptr(syscall.Stderr), "/dev/stderr")
	stdin = os.NewFile(uintptr(syscall.Stdin), "/dev/stdin")
	stdout = os.NewFile(uintptr(syscall.Stdout), "/dev/stdout")
)

type json_request struct {

	Boundary	string			`json:"boundary"`
	Form		*multipart.Form		`json:"form"`
	MimePartPath	map[string][]string	`json:"mime_part_path"`
}

func die(format string, args ...interface{}) {

	fmt.Fprintf(
		stderr,
		"%s: ERROR: %s\n",
		prog,
		fmt.Sprintf(format, args...),
	)
	fmt.Fprintf(stderr, "usage: %s <boundary-string>\n", prog)
	os.Exit(10)
}

func main() {
	if (len(os.Args) != 3) {
		die("wrong number of args: got %d, expected 2", len(os.Args))
	}

	if os.Args[1] != "--boundary" {
		die("unknown option: %s", os.Args[1])
	}

	req := json_request{
		Boundary: os.Args[2],
	}

	mr := multipart.NewReader(stdin, req.Boundary)

	for {
		_, err := mr.NextRawPart()
		if err != nil {
			die("multipart.NextRawPart() failed: %s", err)
		}
	}

	enc := json.NewEncoder(os.Stdout)
	enc.SetIndent("", "\t")
	enc.SetEscapeHTML(false)
	if err := enc.Encode(&req);  err != nil {
		die("json.Encode(stdout) failed: %s", err)
	}
	os.Exit(0)
}
