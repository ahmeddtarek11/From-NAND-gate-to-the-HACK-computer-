

// // Runs an infinite loop that listens to the keyboard input. 
// // When a key is pressed (any key), the program blackens the screen,
// // i.e. writes "black" in every pixel. When no key is pressed, 
// // the screen should be cleared.

// //// Replace this comment with your code.



// 0=white
// -1=black


(LISTNER)
@KBD
D=M
//constats 
@8192
D=A
@n
M=D // setting the end address n=8192
@i
M=0 //setting start counter i=0

@whiteloop
D;JEQ

@blackloop
0;JMP


(whiteloop)
//checking the loop should end or no n-i=0
@n
D=M
@i
D=D-M
@END
D;JEQ
//changing pixels
@i
D=M
@SCREEN
A=A+D
M=0
//incrementing i 
@i
M=M+1
@whiteloop
0;JMP


(blackloop)
//checking the loop should end or no n-i=0
@n
D=M
@i
D=D-M
@END
D;JEQ
//changing pixels
@i
D=M
@SCREEN
A=A+D
M=-1
//incrementing i 
@i
M=M+1
@blackloop
0;JMP


(END)
@LISTNER
0;JMP



