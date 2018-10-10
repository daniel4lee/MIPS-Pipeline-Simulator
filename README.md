# MIPS-Pipeline-Simulator

This program implement the simple MIPS pipeline, including the following instructions `"lw"`, `"sw"`, `"add"`, `"addi"`, `"sub"`, `"and"`, `"andi"`, `"or"`, `"slt"`, `"bne"`.
In addition to the instructions, it also support detecting and dealing `"data hazard"`, `"hazard with load"`, `"branch hazard"`.

## Initialize

There are ten registers, and their initial value is as below:
![list_register](https://i.imgur.com/sryYf15.png)

The memory initial value is as below:  
![list_memory](https://i.imgur.com/NeRcky4.png)

p.s. instruction memory is start from 0.

## Input

The program will read the the `input.txt` file which in the same root, and the content of the file is the 32 bit machine codes.
The  format should like below.

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

After executing the program, it will simulate every pipeline register's value on each clock cycle and saving the record as a `.txt` file named `Result`.

### The content description: 
![format](https://i.imgur.com/HWRKnln.png)