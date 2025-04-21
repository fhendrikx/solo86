
# Hardware


## Interrupts

### 00h - 1Fh reserved; INTEL (hardware/faults)

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

### 20h - 2Fh reserved; Solo/86 System

 - 20h IRQ0 Timer
 - 21h IRQ1 Reserved; Expansion board
 - 22h IRQ2 Reserved; Expansion board
 - 23h IRQ3 COM0/UART
 - 24h IRQ4 Reserved
 - 25h IRQ5 Reserved
 - 26h IRQ6 Reserved
 - 27h IRQ7 Reserved
 - 28h IRQ8 Reserved
 - 29h Reserved
 - 2Ah Reserved
 - 2Bh Reserved
 - 2Ch Reserved
 - 2Dh Reserved
 - 2Eh Reserved
 - 2Fh Reserved

### 30h - FFh available


## IO Ports

### 00h - 7Fh reserved; Solo/86 System

#### Epoch module (PIT)

 - 00h PIT Counter 0 register (read/write)
 - 01h PIT Counter 0 enable latch (write)
 - 01h PIT signature/latch state 0x10101... (read)
 - 02h PIT Counter 1 register (read/write)
 - 03h PIT Counter 1 enable latch (write)
 - 03h PIT signature/latch state 0x10101... (read) (alias)
 - 04h PIT Counter 2 register (read/write)
 - 05h PIT Counter 2 enable latch (write)
 - 05h PIT signature/latch state 0x10101... (read) (alias)
 - 06h PIT Control Word register (read/write)
 - 07h PIT unused latch (write)
 - 07h PIT signature/latch state 0x10101... (read) (alias)

#### Epoch module (RTC)

 - 0Ch RTC Address latch (write)
 - 0Eh RTC Data (read/write)

#### Mainboard memory banking (CPLD)

 - 10h Bank table; row 0
 - 12h Bank table; row 1
 - 14h Bank table; row 2
 - 16h Bank table; row 3
 - 18h Bank table; row 4
 - 1Ah Bank table; row 5
 - 1Ch Bank table; row 6
 - 1Eh Bank table; row 7

#### PiUART module

 - 20h UART Ctrl
 - 21h UART signature 0xAA (read)
 - 22h UART Data
 - 23h UART signature 0xAA (read) (alias)
 - 24h UART reserved
 - 25h UART signature 0xAA (read) (alias)
 - 26h UART reserved
 - 27h UART signature 0xAA (read) (alias)

 - 28h VC_CTRL
 - 29h UART signature 0xAA (read) (alias)
 - 2Ah VC_HIGH_ADDR
 - 2Bh UART signature 0xAA (read) (alias)
 - 2Ch VC_LOW_ADDR
 - 2Dh UART signature 0xAA (read) (alias)
 - 2Eh VC_DATA
 - 2Fh UART signature 0xAA (read) (alias)

#### Panel module

 - 30h LED (write)
 - 30h LED switch state (read)
 - 31h LED signature 0xAA (read)


### 80h - AFh reserved; Testing

 - 80h IO Slow Down

 - A0h Dummy EOI


### B0h - FFh available; Expansion Boards

