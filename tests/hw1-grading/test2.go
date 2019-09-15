package main
func f() int {
   // this was a deliberate test to see what you do with an underscore
   var thread_local int
   thread_local = 5
   return thread_local
}