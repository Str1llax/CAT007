# CAT007 processor
## Specification
- 8-bit processor that operates only **INTEGERS**.
- 8-bit addressing – works with 256 bytes of RAM. Reads 8 bits at a time from RAM. The RAM uses Von Neumann architecture – it stores both the program and the data.
- 8-bit program counter. Implemented as a pointer to the RAM, pointer-type variables are 64 bits (on a 64-bit systems) (8 bytes).
- 8 general-purpose registers, each 8 bits wide. 
- 8-bit flag register.
- 16-bit instruction set with 20 instructions.

## Instructions
The instruction is 16 bits long. Five bits are allocated to the opcode.
The program counter is 8 bits long, so to read the instruction, you need to read one byte at a time, twice.
The first byte reads bits 15–08 of the instruction, and the second byte reads bits 07–00.

Instruction set:
| Instruction | Code | Type | Mode | Comment |
| ----------- | ---- | ---- | ---- | ------- |
| NOP | 0x00 | I |  | Nothing happens. |
| HLT | 0x0B | I |  | Stops execution. |
| IN | 0x01 | II | 0 | Reads one byte from the input file and writes it to the destination register. Sets the EOF(End Of File) flag if the end of the document has been reached. |
| OUT | 0x02 | II | 0 | Exports the contents of the source register to the output file. |
| MOV | 0x03 | II | 0 | Copies the contents of the source register to the destination register. |
| MOVC | 0x04 | III |  | Writes constant to the destination register. |
| JMP | 0x05 | IV |  | Unconditional jump, always happenning. Constant determines how much bytes jumping (signed values). |
| JZ | 0x06 | IV |  | Jumps if Zero flag was active. |
| JNZ | 0x07 | IV |  | Jumps if Zero flag was inactive. |
| JC | 0x08 | IV |  | Jumps if Carry flag was active. |
| JO | 0x09 | IV |  | Jumps if Overflow flag was active. |
| JFE | 0x0A | IV |  | Jumps if EOF flag was active. |
| INC | 0x0C | III |  | Increments value at destination register. |
| DEC | 0x0D | III |  | Dencrements value at destination register. |
| ADD | 0x0E | II | 0 | Adds value from source register to destination register. |
|  | 0x0E | II | 1 | Adds value from first source register to second source register and stores it in the destination register. |
| SUB | 0x0F | II | 0 | Substracts from destination register value stored in source register. |
|  | 0x0F | II | 1 | Substracts from first source register value from second source register and stores it in the destination register. |
| XOR | 0x10 | II | 0 | XOR between destination register and source register. Stores result in destination register |
|  | 0x10 | II | 1 | XOR between first source register and second source register. Stores result in destination register |
| OR | 0x11 | II | 0 | OR between destination register and source register. Stores result in destination register |
|  | 0x11 | II | 1 | OR between first source register and second source register. Stores result in destination register |
| AND | 0x12 | II | 0 | AND between destination register and source register. Stores result in destination register |
|  | 0x12 | II | 1 | AND between first source register and second source register. Stores result in destination register |
| LSR | 0x13 | II | 0 | Logicaly Shifts Right value from source register. Stores result in destination register |
| LSL | 0x14 | II | 0 | Logicaly Shifts Left value from source register. Stores result in destination register |


## To be added...
