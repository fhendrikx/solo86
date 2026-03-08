
# IO Ports

We're using I/O address space in blocks of 16, e.g. 00->0F, 01->1F, etc.
Each module gets one (or more) blocks as needed.

Keep in mind the 286 has a 16 bit bus with the following limitations:
 - even numbered 8 bit READ/WRITE always uses D0-D7
 - odd numbered 8 bit READ/WRITE always uses D8-D15
 - even numbered 16 bit READ/WRITE always uses D0-D15
 - odd numbered 16 bit READ/WRITE are not permitted

In practice this means 8 bit peripherals connected to D0-D7 can only use the even numbered addresses so each block of 16 ports is really only 8 usable addresses.
We typically use the odd addresses to connect a signature (usually 0xAA) to help identify if a module is present.


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
 - 1Ch INT Base (Set base vector)


### PiUART module (COM)

 - 20h UART1 Ctrl
 - 22h UART1 Data
 - 24h UART2 Ctrl
 - 26h UART2 Data
 - 21h, 23h, 25h, 27h PiUART Signature 0xAA (read)

### PiUART module (Video)

 - 28h VC Ctrl
 - 2Ah VC Parameters
 - 2Ch VC Data
 - 2Eh VC Reserved
 - 29h, 2Bh, 2Dh, 2Fh PiUART Signature 0xAA (read)

### PiUART module (UART/8250 emulation)

 - 30h -> 3Fh
 - TODO


### Compact Flash module

 - 40h ATA DATA
 - 42h ATA Feature (write) or Error (read)
 - 44h ATA Sector Count
 - 46h ATA Sector Number  (LBA 0-7)
 - 48h ATA Cylinder Low   (LBA 8-15)
 - 4Ah ATA Cylinder High  (LBA 16-23)
 - 4Ch ATA Device/Head    (LBA 24-27)
 - 4Eh ATA Command (write) or Status (read)
 - 5Ch ATA Device Control (write) or Alternative Status (read)
 - 50h -> 5Ah (even), 5Eh Unused
 - 41h -> 5Fh (odd numbers) signature 0xAA (8 bit read only)


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


### Console/Panel module

Note, ELKS sometimes reads from I/O port 80h to create a short delay.
This will read the switch state which is harmless.

 - 80h LED Ctrl (write)
 - 80h LED Data Switch state (read)
 - 81h LED Signature 0xAA (read)

