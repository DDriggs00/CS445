package main
import "fmt"
// This is a VGo test. It would not pass semantic checks yet. //
var x struct { }

func main() {
  var d float64
  var x, y int
  var L [5]int
  var T map[string]int
  var s string
  var c C
  x = 3 * 6
  c = null
  for x < 18 {
     if x == 9 {
     break
     } else if x <= 5 {
     break
     } else if x >= 15 {
     break
     } else {
     x += 1
     }
     }
  for x=0; x < 3; x+=1 { x = x + 5 }
  for x=3; x > 0; x-=1 { x = x - 2 }
  L[1] = 5 * 5 / x % 5
  L[1] = x
  if x<=y && y>=0 || y!=-1 { fmt.Println("hello") }
  return
}