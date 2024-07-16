
// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.

//setting R2 to 0 
@R2 
M=0


(LOOP)
// decrement R1 and if R1<0 goto END
@R1
D=M
D=D-1
@R1  //save decremented value to R1
M=D
@END   
D;JLT

// store R0 in D
@R0
D=M
// ADD R2 and  R0(D)
@R2
M=D+M

//loop back 
@LOOP
0;JMP





(END)
@END
0;JMP














