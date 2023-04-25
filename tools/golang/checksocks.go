package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"runtime"
	"strconv"
	"strings"
	"sync"
	"time"

	"golang.org/x/net/proxy"
)

var (
	ourServer = "172.86.123.106"
	ourPort   = 9999

	wg sync.WaitGroup

	auth    int
	total   int
	success int
	failed  int
)

func sendLog(name, aval string) {
	file, err := os.OpenFile(name, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		return
	}

	defer file.Close()

	file.WriteString(aval)
}

func socksHandler(host string) {
	total++
	wg.Add(1)
	defer wg.Done()

	// Create a SOCKS5 dialer
	dialer, err := proxy.SOCKS5("tcp", host, nil, proxy.Direct)
	if err != nil {
		failed++
		return
	}

	// Connect to a remote server through the SOCKS5 proxy
	conn, err := dialer.Dial("tcp", fmt.Sprintf("%s:%d", ourServer, ourPort))
	if err != nil {
		failed++
		return
	}

	auth++

	// Use the connection
	fmt.Fprintf(conn, "Socks.Valid %s", host)
	if err != nil {
		return
	}
}

func titleWriter() {
	for {
		time.Sleep(1 * time.Second)
		fmt.Printf("Total (%d) Auth (%d) Found (%d) Running (%d) Failed (%d)\r\n", total, auth, success, runtime.NumGoroutine(), failed)
	}
}

func listenHandle(conn net.Conn) {
	defer conn.Close()
	buf := make([]byte, 1024)

	conn.Read(buf)

	strBuf := string(buf)

	if strings.Contains(strBuf, "Socks.Valid") {
		success++
		sendLog("validsocks.txt", strings.Split(strBuf, " ")[1]+"\r\n")
	}
}

func listenServer() {
	bcon, err := net.Listen("tcp", fmt.Sprintf("0.0.0.0:%d", ourPort))
	if err != nil {
		return
	}

	defer bcon.Close()

	for {
		conn, err := bcon.Accept()
		if err != nil {
			continue
		}

		go listenHandle(conn)
	}
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	scanner := bufio.NewScanner(reader)
	routines, _ := strconv.Atoi(os.Args[2])
	runType := os.Args[1]

	go titleWriter()
	go listenServer()

	for scanner.Scan() {
		for routines < runtime.NumGoroutine() {
			time.Sleep(1 * time.Second)
		}

		if runType != "manual" {
			go socksHandler(scanner.Text() + ":" + runType)
		} else {
			go socksHandler(scanner.Text())
		}
	}

	for {
		time.Sleep(1 * time.Second)
	}
}
