//  rest service damon built from postgresql query files and json config

package main

import (
	"fmt"
	"net/http"
	"os"
	"os/signal"
	"runtime"
	"syscall"
)

var stderr = os.NewFile(uintptr(syscall.Stderr), "/dev/stderr")

func usage() {
	fmt.Fprintf(stderr, "usage: rasqld <config.json>\n")
}

func reply_ERROR(
	status int,
	w http.ResponseWriter,
	r *http.Request,
	format string,
	args ...interface{},
) {

	ERROR(r.RemoteAddr+": "+format, args...)
	http.Error(w, fmt.Sprintf(format, args...), status)
}

func boot() {

	var cf Config

	INFO("process id: %d", os.Getpid())
	INFO("go version: %s", runtime.Version())

	cf.load(os.Args[1])
	cf.SQLQuerySet.open()

	INFO("path sql query index: %s", cf.RESTPathPrefix)
	http.HandleFunc(
		cf.RESTPathPrefix,
		cf.handle_query_index_json,
	)

	//  install sql query handlers
	//
	//	/<rest-path-prefix>/<sql-query>
	//	/<rest-path-prefix>/csv/<sql-query>
	//	/<rest-path-prefix>/tsv/<sql-query>
	//	/<rest-path-prefix>/html/<sql-query>
	//

	for n, q := range cf.SQLQuerySet {

		//  json handler, the default

		http.HandleFunc(
			fmt.Sprintf("%s/%s", cf.RESTPathPrefix, n),
			cf.new_handler_query_json(q),
		)

		//  tsv handler

		http.HandleFunc(
			fmt.Sprintf("%s/tsv/%s", cf.RESTPathPrefix, n),
			cf.new_handler_query_tsv(q),
		)

		//  csv handler

		http.HandleFunc(
			fmt.Sprintf("%s/csv/%s", cf.RESTPathPrefix, n),
			cf.new_handler_query_csv(q),
		)

		//  html handler

		http.HandleFunc(
			fmt.Sprintf("%s/html/%s", cf.RESTPathPrefix, n),
			cf.new_handler_query_html(q),
		)
	}

	if cf.HTTPListen != "" {
		INFO("listening: %s%s", cf.HTTPListen, cf.RESTPathPrefix)
		go func() {
			err := http.ListenAndServe(cf.HTTPListen, nil)
			die("http listen error: %s", err)
		}()
	}
	if cf.TLSHTTPListen != "" {
		if cf.TLSCertPath == "" {
			die("http listen tls: missing tls-cert-path")
		}
		if cf.TLSKeyPath == "" {
			die("http listen tls: missing tls-key-path")
		}
		INFO("tls listening: %s%s", cf.TLSHTTPListen, cf.RESTPathPrefix)
		go func() {
			err := http.ListenAndServeTLS(
				cf.TLSHTTPListen,
				cf.TLSCertPath,
				cf.TLSKeyPath,
				nil,
			)
			die("http listen tls: %s", err)
		}()
	}
}

func main() {

	if len(os.Args) != 2 {
		die("wrong number of arguments: got %d, expected 1",
			len(os.Args),
		)
	}
	log_init("rasqld")

	boot()

	//  wait for signals
	//  Note: see SIGTERM ignored on mac 10.13.2 from time to time.

	c := make(chan os.Signal)
	signal.Notify(c, syscall.SIGTERM)
	signal.Notify(c, syscall.SIGQUIT)
	signal.Notify(c, syscall.SIGINT)
	s := <-c
	INFO("caught signal: %s", s)
	leave(0)
}
