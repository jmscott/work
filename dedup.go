/*
 *  Synopsis:
 *	Remove duplicate lines on stdin and write to stdout.
 *  Note:
 *	A clang version exists in setspace, which will eventually replace this
 *	golang version.  surprisingly, this golang version is only about %25
 *	slower.
 */
package main

import (
	"bufio"
	"fmt"
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
	var buf[4096 * 4096]byte

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

	in := bufio.NewScanner(os.Stdin)
	in.Buffer(buf[:], len(buf))
	seen = make(map[string]bool, 4096 * 4096)
	for in.Scan() {
		txt := in.Text()
		if !seen[txt] {
			seen[txt] = true
			if put_count == false {
				fmt.Println(txt)
			}
		}
	}
	if err := in.Err(); err != nil {
		fmt.Fprintf(os.Stderr, "ERROR: %s\n", err)
		os.Exit(1)
	}
	if put_count {
		fmt.Printf("%d\n", len(seen))
	}
	os.Exit(0)
}
