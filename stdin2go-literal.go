package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"strings"
	"unicode/utf8"
)

//
//  Synopsis:
//	Convert Stdin to golang []byte
//  Usage:
//	var pupster_jpg = [...]byte{
//	}
//

func main() {

	in := bufio.NewReader(os.Stdin)
	out := os.Stdout
	lineno := 0
	out.Write([]byte("`"))
	for {
		line, err := in.ReadString('\n')
		lineno += 1
		if err != nil {
			if err == io.EOF {
				break
			}
			panic(err)
		}
		if !utf8.ValidString(line) {
			panic(fmt.Sprintf(
				"invalid utf8: line number %d", lineno,
			))
		}
		if strings.Contains("`", line) {
			panic(fmt.Sprintf(
				"backtick exists: line number %d", lineno,
			))
		}
		out.Write([]byte(line))
	}
	out.Write([]byte("`\n"))
}
