package main

import (
	"fmt"
	"io"
	"os"
)

//
//  Synopsis:
//	Convert Stdin to golang []byte
//  Usage:
//	var pupster_jpg = [...]byte{
//	}
//

func main() {

	var buf [8]byte

	in := os.Stdin
	out := os.Stdout
	out.Write([]byte("[...]byte{\n"))
	for {
		nr, err := in.Read(buf[:])
		if err != nil {
			if err == io.EOF {
				break
			}
			panic(err)
		}
		for i := 0;  i < nr;  i = i + 1 {
			out.Write([]byte(fmt.Sprintf(" 0x%x,", buf[i])))
		}
		out.Write([]byte("\n"))
	}
	out.Write([]byte("}\n"))
}
