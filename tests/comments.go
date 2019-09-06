package main
func main() {

    "This is //not a comment//"
    // This is a comment  //
    // We do not support /* this kind of "nested" comments // */
    // This also contains a //nested comment // that we don't even notice
    
    "Keep this string" // this is a comment "//... "
    "This // is not a comment... " it is just a string
    KEEPTHIS // Another comment. This line has an identifier on it
    x = 1 // Also a comment, after x = 1
    /*
     * what about this kind? should there be an error on line 13?
     */
}


