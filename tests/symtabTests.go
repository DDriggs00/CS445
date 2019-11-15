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

func add(x1, x2 float64) float64 {
	return x1 + x2
}
func ret1() int {
	return 1
}
func main() {
	var q int
	var ps person
	ps.age = 2

	var n1 float64
	fmt.Println(n1)
	// q = (4 + 5) - 10 + (ps.age + q)
	if q == 5 {
		q = 1
	}
	y = add(1.5, '1')
	q = ret1()
}
