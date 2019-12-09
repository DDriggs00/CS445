package main

import "fmt"

func main() {
	var m map[string]float64
	m = make(map[string]float64)
	m["hello"] = 5.5
	fmt.Println(m["hello"])
}
