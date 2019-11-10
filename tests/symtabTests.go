package main

import "fmt"

var x int = 5
var y float64 = 10.5
var z bool = true
var a [3]rune

const b string = "test"

var m map[string]int
var n map[string]bool

type person struct {
	name string
	age  int
}

func main() {
	var q int
	q = 5
	z = false
	var n1 float64
	fmt.Println(n1)
	q = (4 + 5) - 10 + (2 + q)
	if q == 5 {
		q = 1
	}

}

func f2(x1, x2 float64, y1, y2 int) {
	var y float64
}
