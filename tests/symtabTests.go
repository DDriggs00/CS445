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
	var ps person
	q = 5
	z = false

	var n1 float64
	fmt.Println(n1)
	q = (4 + 5) - 10 + (ps.age + q)
	if q == 5 {
		q = 1
	}
	y = f2(1.5, 1.8, 1, 2)
}

func f2(x1, x2 float64, y1, y2 int) float64 {
	var y float64
	return 2.5
}
