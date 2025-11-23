
# IO Ports

## 00h - 7Fh reserved; Solo/86 System

### Epoch module (PIT)

 - 00h PIT Counter 0 register (read/write)
 - 01h PIT Counter 0 enable latch (write)
 - 01h PIT Signature/latch state 0x10101... (read)
 - 02h PIT Counter 1 register (read/write)
 - 03h PIT Counter 1 enable latch (write)
 - 03h PIT Signature/latch state 0x10101... (read) (alias)
 - 04h PIT Counter 2 register (read/write)
 - 05h PIT Counter 2 enable latch (write)
 - 05h PIT Signature/latch state 0x10101... (read) (alias)
 - 06h PIT Control Word register (read/write)
 - 07h PIT Reserved
 - 07h PIT Signature/latch state 0x10101... (read) (alias)

### Epoch module (RTC)

 - 0Ch RTC Address latch (write)
 - 0Eh RTC Data (read/write)


### Mainboard (memory banking; CPLD)

 - 10h BNK table; row 0
 - 12h BNK table; row 1
 - 14h BNK table; row 2
 - 16h BNK table; row 3

### Mainboard (interrupt controller; CPLD)

 - 18h INT Ctrl
 - 1Ah INT Data (EOI; end of interrupt)


### PiUART module (COM)

 - 20h UART Ctrl
 - 21h UART Signature 0xAA (read)
 - 22h UART Data
 - 23h UART Signature 0xAA (read) (alias)
 - 24h UART Reserved
 - 25h UART Signature 0xAA (read) (alias)
 - 26h UART Reserved
 - 27h UART Signature 0xAA (read) (alias)

### PiUART module (Video)

 - 28h VC Ctrl
 - 29h UART Signature 0xAA (read) (alias)
 - 2Ah VC Parameters
 - 2Bh UART Signature 0xAA (read) (alias)
 - 2Ch VC Data
 - 2Dh UART Signature 0xAA (read) (alias)
 - 2Eh VC Reserved
 - 2Fh UART Signature 0xAA (read) (alias)


### Console/Panel module

 - 30h LED Ctrl (write)
 - 30h LED Data Switch state (read)
 - 31h LED Signature 0xAA (read)

### Compact Flash module

 - 40h -> 5Fh
 - TODO fill in compact flash ports

### 16550 UART module

 - 60h -> 6Fh  16550 port A

 - 60h Receiver buffer (read), transmitter holding register (write)
 - 62h Interrupt enable register
 - 64h Interrupt identification register (read), FIFO control register (write)
 - 66h Line control register
 - 68h Modem control register
 - 6Ah Line status register
 - 6Ch Modem status register
 - 6Eh Scratch register

 - 061, 063, 065, 067, 069, 06B, 06D, 06Fh UART Signature 0xAA (read)

 - 70h -> 7Fh  16550 port B

 - 70h Receiver buffer (read), transmitter holding register (write)
 - 72h Interrupt enable register
 - 74h Interrupt identification register (read), FIFO control register (write)
 - 76h Line control register
 - 78h Modem control register
 - 7Ah Line status register
 - 7Ch Modem status register
 - 7Eh Scratch register

 - 071, 073, 075, 077, 079, 07B, 07D, 07Fh UART Signature 0xAA (read)

### 80h - AFh reserved; Testing

 - 80h IO Slow Down


### B0h - FFh available; Expansion Boards

 - available

