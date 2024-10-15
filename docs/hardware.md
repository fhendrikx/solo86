
# Hardware


## Interrupts

### 00h - 1Fh reserved; System (hardware/faults)

 - 00h Divide by zero
 - 01h Single step
 - 02h NMI
 - 03h Breakpoint
 - 04h INTO overflow
 - 05h BOUND range
 - 06h Invalid opcode
 - 07h Extension not available
 - 08h Interrupt table too small
 - 09h Segment overrun
 - 0Ah Reserved
 - 0Bh Reserved
 - 0Ch Reserved
 - 0Dh Reserved
 - 0Eh Reserved
 - 0Fh Reserved
 - 10h Extension error
 - 11h Reserved
 - 12h Reserved
 - 13h Reserved
 - 14h Reserved
 - 15h Reserved
 - 16h Reserved
 - 17h Reserved
 - 18h Reserved
 - 19h Reserved
 - 1Ah Reserved
 - 1Bh Reserved
 - 1Ch Reserved
 - 1Dh Reserved
 - 1Eh Reserved
 - 1Fh Reserved


### 20h - 2Fh reserved; System

 - 20h IRQ0 Timer
 - 21h IRQ1 Reserved; Expansion board
 - 22h IRQ2 Reserved; Expansion board
 - 23h IRQ3 UART
 - 24h IRQ4 Reserved; Expansion board
 - 25h IRQ5 Reserved; Expansion board
 - 26h Reserved
 - 27h Reserved
 - 28h Reserved
 - 29h Reserved
 - 2Ah Reserved
 - 2Bh Reserved
 - 2Ch Reserved
 - 2Dh Reserved
 - 2Eh Reserved
 - 2Fh Reserved

 - 30h System Call


## IO Ports

### 00h - 9Fh reserved; System

 - 04h Timer Data
 - 06h Timer Ctrl

 - 08h LED
 - 09h LED signature 0xAA

 - 10h Bank table; row 0
 - 12h Bank table; row 1
 - 14h Bank table; row 2
 - 16h Bank table; row 3
 - 18h Bank table; row 4
 - 1Ah Bank table; row 5
 - 1Ch Bank table; row 6
 - 1Eh Bank table; row 7

 - XXh Clock            -- TBD

 - 20h UART Ctrl
 - 21h UART signature 0xAA
 - 22h UART Data
 - 23h UART signature 0xAA
 - 24h UART reserved
 - 25h UART signature 0xAA
 - 26h UART reserved
 - 27h UART signature 0xAA
 - 28h UART reserved
 - 29h UART signature 0xAA
 - 2Ah UART reserved
 - 2Bh UART signature 0xAA
 - 2Ch UART reserved
 - 2Dh UART signature 0xAA
 - 2Eh UART reserved
 - 2Fh UART signature 0xAA

 - 80h Delay


### A0h - FFh reserved; Expansion boards


