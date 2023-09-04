/*
 *  Synopsis:
 *	Listen on udp4 port and reflect incoming packets to ip4s.
 *  Usage:
 *	udp4-reflect <in-ip4:port> <out1-ip4:port> <out2-ip4:port> ...
 *	udp4-reflect 192.168.1.175:10514 10.187.1.3:20001 10.187.1.5:20001
 */
package main

import (
	"fmt"
	"net"
	"os"
	"os/signal"
	"syscall"
)

var in_count, out_count, err_count uint64

func die(format string, args ...interface{}) {
	
	fmt.Fprintf(os.Stderr, "ERROR: " + format + "\n", args...)
	os.Exit(1)
}

func put_stats() { 
	fmt.Fprintf(os.Stdout, " in: %d pkts\n", in_count)
	fmt.Fprintf(os.Stdout, "out: %d pkts\n", out_count)
	fmt.Fprintf(os.Stdout, "err: %d pkts\n", err_count)
}

func scatter(out []*net.UDPConn) (ref_c chan []byte) {

	ref_c = make(chan []byte, 256)

	go func() {
		
		for {
			pkt := <- ref_c
			for _, o := range out {
				_, err := o.Write(pkt)
				if err != nil {
					err_count++
					fmt.Fprintf(
						os.Stderr, 
						"Write(%s) failed: %s\n",
						o.RemoteAddr().String(),
						err,
					)
					put_stats()
				} else {
					out_count++
				}
			}
		}
	}()
	return ref_c
}

func reflect(in *net.UDPConn, out []*net.UDPConn) {

	ref_c := scatter(out)
	for {
	       var buf [2048]byte	//  larger than udp packet

		nb, err := in.Read(buf[:])
		if err != nil {
			die("in.Read(udp4) failed: %s", err)
		}
		if nb == 0 {
			die("in.Read(udp4): zero bytes")
		}
		ref_c <- buf[:nb]
		in_count++
	}
}

func main() {

	argc := len(os.Args)
	if argc < 3 {
		die("wrong number of cli args: got %d, expected > 1", argc)
	}

	//  open udp4 listener

	addr, err := net.ResolveUDPAddr("udp4", os.Args[1])
	if err != nil {
		die("net.ResolveUDPAddr(%s) failed: %s", os.Args[1], err)
	}
	in, err := net.ListenUDP("udp4", addr)
	if err != nil {
		die("net.ListenUDP(%s) failed: %s", addr, err)
	}

	//  open udp4 "reflectors"

	out := make([]*net.UDPConn, argc - 2)
	for i := 2;  i < argc;  i++ {
		arg := os.Args[i]
		addr, err := net.ResolveUDPAddr("udp4", arg)
		if err != nil {
			die("net.ResolveUDPAddr(%s) failed: %s", arg, err)
		}
		conn, err := net.DialUDP("udp4", nil, addr)
		if err != nil {
			die("net.DialUDP(%s) failed: %s", addr, err)
		}
		out[i-2] = conn
	}

	go reflect(in, out)

	sigc := make(chan os.Signal, 1)
	signal.Notify(sigc, syscall.SIGINT, syscall.SIGTERM)

	for {
		sig := <-sigc
		if sig != syscall.SIGUSR1 {
			fmt.Fprintf(
				os.Stderr,
				"\nudp4-reflect: %d pkts, caught signal: %s\n",
				in_count,
				sig,
			)
			os.Exit(1)
		}
		put_stats()
	}
	os.Exit(0)
}
