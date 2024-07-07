/*
 *  Synopsis:
 *	Listen on udp4 port and write human readable packets to stderr.
 *  Usage:
 *	udp4-listen <listen-ip4> <port> <output-path|stdout|stderr>
 *	udp4-listen 192.168.1.175 10514 raspberry-pi.log
 *	udp4-listen 192.168.1.175 10514 stderr
 *  Note:
 *	Consider replacing the default output of msg+"\n" to a pipe to
 *	long running background process to further masage the udp4 message.
 *	By the way, this piping trick may actually be possible using socats
 *	ability to fork subprocesses.  not sure.
 *
 *	This program only exists since socat can not terminate udp4 packets
 *	with a newline.  Various piping to a script were more cumbersone than
 *	than just writing this program.
 */
package main

import (
	"fmt"
	"net"
	"os"
	"os/signal"
	"syscall"
)

func die(format string, args ...interface{}) {
	
	fmt.Fprintf(os.Stderr, "ERROR: " + format + "\n", args...)
	fmt.Fprintf(
		os.Stderr,
		"usage: udp4-listen <host> <port> <out-path|stdout|stderr>",
	)
	os.Exit(1)
}

func slurp(in *net.UDPConn, out *os.File) {

	for {
	       var buf [2048]byte	//  larger than udp packet

		nb, err := in.Read(buf[:])
		if err != nil {
			die("in.Read(udp4) failed: %s", err)
		}
		if nb == 0 {
			die("in.Read(udp4): zero bytes")
		}
		if buf[nb - 1] != '\n' {
			buf[nb] = '\n'
			nb++
		}
		_, err = out.Write(buf[:nb])
		if err != nil {
			die("out.Write(out) failed: %s", err)
		}
	}
}

func main() {

	argc := len(os.Args) - 1
	if argc != 3 {
		die("wrong number of cli args: got %d, expected 3", argc)
	}

	//  open output

	var out *os.File
	out_path := os.Args[3]
	if out_path == "stdout" {
		out = os.Stdout
	} else if out_path == "stderr" {
		out = os.Stderr
	} else {
		var err error

		out, err = os.OpenFile(
				out_path,
				os.O_APPEND|os.O_WRONLY|os.O_CREATE,
				0755,
		)
		if err != nil {
			die("os.File.Open(%s) failed: %s", out_path, err)
		}
	}

	//  open udp4 listener

	service := fmt.Sprintf("%s:%s", os.Args[1], os.Args[2])
	addr, err := net.ResolveUDPAddr("udp4", service)
	if err != nil {
		die("net.ResolveUDPAddr(%s) failed: %s", service, err)
	}
	in, err := net.ListenUDP("udp4", addr)
	if err != nil {
		die("net.ListenUDP(%s) failed: %s", addr, err)
	}

	go slurp(in, out)

	sigc := make(chan os.Signal, 1)
	signal.Notify(sigc, syscall.SIGINT, syscall.SIGTERM)

	sig := <-sigc
	fmt.Println("\nudp4-listen: caught signal:", sig)
	os.Exit(0)
}
