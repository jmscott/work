//  convert unix process environment to json object.

package main

import (
	"encoding/json"
	"fmt"
	"os"
	"strings"
)

func main() {

	env := make(map[string]string)

	for _, e := range os.Environ() {
		pair := strings.SplitN(e, "=", 2)
		env[pair[0]] = pair[1]
	}

	enc := json.NewEncoder(os.Stdout)
	enc.SetEscapeHTML(false)
	enc.SetIndent("", "\t")
	err := enc.Encode(env)
	if err != nil {
		fmt.Fprintf(os.Stderr, "ERROR: enc.Encode() failed: %s\n", err)
		os.Exit(1)
	}
	os.Exit(0)
}
