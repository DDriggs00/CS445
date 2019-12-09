package main

import "fmt"

type Foo struct {
	dd float64
}

func mkFoo() Foo {
	var s Foo
	s.dd = 5.5
	return s
}

func fib(f Foo, i int) float64 {
	var x float64
	x = f.dd * 2.0
	return x
}

func main() {
	var f Foo
	var i float64
	i = 0.0 + fib(f, 3)
	i = 5.0
	i = fib(f, "hello")
	fmt.Println(i)
}
