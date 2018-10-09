# MIPS-Pipeline-Simulator

## Input

The program will read the the `input.txt` file which in the same root, and the  format should like below.
```
10001101000000010000000000000011  
00000000000000100001100000100000  
00010000000000100000000000000110
```
The correspounding MIPS instructions is:
```shell
lw $1, 0x03($8)  
add $3, $0, $2  
beq $0, $2, 0x06 #(branch PC+4+4×6)   
```

## Output

After executing the program, it will produce a `.txt` file named `Result`.

### The content description: 
![format](https://i.imgur.com/HWRKnln.png)