
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


### 80h - AFh reserved; Testing

 - 80h IO Slow Down


### B0h - FFh available; Expansion Boards

 - available

