/*
 *  Synopsis
 *	Parse stdin as multipart mime message according to RFC2388 to json.
 *  Usage:
 *	WORK_DIR=/tmp/bust-http-rfc2388-$$.d
 *	mkdir $WORK_DIR
 *	cd $WORK_DIR
 *	bust-http-rfc2388 <boundary> >request.json
 *	curl -F 'data=@path/to/local/file' UPLOAD_ADDRESS
 *  See:
 *	https://medium.com/@petehouston/upload-files-with-curl-93064dcccc76
 */
package main

import (
	"encoding/json"
	"fmt"
	"mime/multipart"
	"os"
	"syscall"
	"time"
)

var (
	prog = "bust-http-rfc2388"
	stderr = os.NewFile(uintptr(syscall.Stderr), "/dev/stderr")
	stdin = os.NewFile(uintptr(syscall.Stdin), "/dev/stdin")
	stdout = os.NewFile(uintptr(syscall.Stdout), "/dev/stdout")
)

type ProcessSignature struct {
	Hostname	string		`json:"hostname"`
	Executable	string		`json:"executable"`
	Args		[]string
	StartTime	time.Time	`json:"start_time"`
	ProcessId	int		`json:"process_id"`
	ParentProcessId	int		`json:"parent_process_id"`
	Environment	[]string	`json:"environment"`	
	GroupId		int		`json:"group_id"`
	UserId		int		`json:"user_id"`
}

type json_response struct {

	Signature	ProcessSignature	`json:"process_signature"`
	BoundaryString	string			`json:"mime_boundary_string"`
	Form		*multipart.Form		`json:"form"`
	TmpFilePath	map[string][]string	`json:"tmp_file_path"`
}

func die(format string, args ...interface{}) {

	fmt.Fprintf(
		stderr,
		"%s: ERROR: %s\n",
		prog,
		fmt.Sprintf(format, args...),
	)
	fmt.Fprintf(stderr, "usage: %s <boundary-string>\n", prog)
	os.Exit(1)
}

func main() {
	if (len(os.Args) != 2) {
		die("wrong number of args: got %d, expected 2", len(os.Args))
	}

	res := json_response {
		Signature:	ProcessSignature {
			UserId:			os.Getuid(),
			GroupId:		os.Getgid(),
			ProcessId:		os.Getpid(),
			ParentProcessId:	os.Getppid(),
			StartTime:		time.Now(),
			Environment:		os.Environ(),
			Args:			os.Args,
		},
		BoundaryString:		os.Args[1],
	}

	if exe, err := os.Executable();  err != nil {
		die("os.Executable() failed: %s", err)
	} else {
		res.Signature.Executable = exe
	}

	if host, err := os.Hostname();  err != nil {
		die("os.Hostname() failed: %s", err)
	} else {
		res.Signature.Hostname = host
	}

	mr := multipart.NewReader(stdin, res.BoundaryString)
	form, err := mr.ReadForm(int64(0))
	if err != nil {
		die("multipart.ReadForm(stdin) failed: %s", err)
	}
	res.Form = form

	//  find the file paths for each part
	res.TmpFilePath = make(map[string][]string)
	for key, val := range res.Form.File {

		//  loop over the multiple file headers stored by os
		for i, fh := range val {
			what := fmt.Sprintf("%s:[%d]", key, i)
			f, err := fh.Open()
			if err != nil {
				die("Open(mime:%s) failed: %s", what, err)
			}
			osf, ok := f.(*os.File)
			if !ok {
				die("mime:*os.File: type assertion failed")
			}
			st, err := osf.Stat()
			if err != nil {
				die("Stat(mime:%s) failed: %s",what,err)
			}
			res.TmpFilePath[key] = append(
						res.TmpFilePath[key],
						st.Name(),
					)
			err = osf.Close()
			if err != nil {
				die("mime:*os.File.Close() failed: %s", err)
			}
		}
	}

	enc := json.NewEncoder(os.Stdout)
	enc.SetIndent("", "\t")
	enc.SetEscapeHTML(false)
	if err := enc.Encode(&res);  err != nil {
		die("json.Encode(stdout) failed: %s", err)
	}
	os.Exit(0)
}
