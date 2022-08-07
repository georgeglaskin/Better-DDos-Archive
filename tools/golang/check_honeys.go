package main

import (
	"bufio"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"strings"
	"time"
)

var total, honeys, nothoneys int
var honey_strings = [...]string{"aws", "amazon", "google", "hosting", "alibaba", "microsoft", "linode"}

func check_honey(host string) bool {
	resp, err := http.Get(fmt.Sprintf("http://ip-api.com/json/%s", host))
	if err != nil {
		fmt.Printf("%v\n", err)
		return true
	}

	defer resp.Body.Close()

	body, error := ioutil.ReadAll(resp.Body)
	if error != nil {
		return true
	}

	for i := 0; i < len(honey_strings); i++ {
		if strings.Contains(strings.ToLower(string(body)), honey_strings[i]) {
			return true
		}
	}

	return false
}

func handle_host(host string) {
	total++

	if check_honey(host) == true {
		honeys++
	} else {
		nothoneys++
	}
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	scanner := bufio.NewScanner(reader)

	i := 0

	go func() {
		for {
			fmt.Printf("[%d's] Total [%d] Honeys [%d] Normal [%d]\n", i, total, honeys, nothoneys)
			i++

			time.Sleep(1 * time.Second)
		}
	}()

	for scanner.Scan() {
		for i := 0; i < 1204; i++ {
			go handle_host(scanner.Text())
		}

		time.Sleep(3 * time.Second)
	}

}
