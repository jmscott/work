/*
 *  Synopsis:
 *	Remove duplicate lines on stdin and write to stdout.
 *  Note:
 *	dedup seems to be about twice as fast as "sort -u", when
 *	LANG=en_US.UTF-8;  otherwise, for LANG=C, "sort -u" about 4 time
 *	faster.
 *
 *	A clang version exists in setspace, which will eventually replace this
 *	golang version.  surprisingly, this golang version is only about %25
 *	slower.
 */
package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
)

func die(format string, args ...interface{}) {
	
	fmt.Fprintf(os.Stderr, "dedup: ERROR: " + format + "\n", args...)
	fmt.Fprintf(
		os.Stderr,
		"usage: dedup [--count]",
	)
	os.Exit(1)
}

func main() {
	
	var seen map[string]bool

	put_count := false

	argv := os.Args[1:]
	argc := len(argv)
	if argc == 1 {
		if argv[0] != "--count" {
			die("unknown option: %s", argv[0])
		}
		put_count = true
	} else if argc != 0 {
		die("bad cli arg count: got %d, need 1 or 0", argc)
	}

	in := bufio.NewReaderSize(os.Stdin, 4096 * 4096)
	seen = make(map[string]bool, 4096 * 4096)
	for {
		line, err := in.ReadString('\n')
		if err != nil {
			if err == io.EOF {
				break
			}
			fmt.Fprintf(os.Stderr, "ERROR: %s\n", err)
			os.Exit(1)
		}
		if !seen[line] {
			seen[line] = true
			if put_count == false {
				os.Stdout.Write([]byte(line))
			}
		}
	}
	if put_count {
		fmt.Printf("%d\n", len(seen))
	}
	os.Exit(0)
}
