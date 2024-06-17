
# Hardware


## Interrupts

### 00h - 1Fh reserved; System (hardware/faults)

### 20h - 3Fh reserved; System

 - 20h IRQ0 Timer
 - 21h IRQ1 Reserved; Expansion board
 - 22h IRQ2 Reserved; Expansion board
 - 23h IRQ3 UART
 - 24h IRQ4 Reserved; Expansion board
 - 25h IRQ5 Reserved; Expansion board

### 40h - 7Fh reserved; System

### 80h - 80h reserved; System

 - 80h System Call (ELKS)


## IO Ports

### 00h - 9Fh reserved; System

 - 04h Timer
 - 08h LED
 - 10h Bank table
 - 20h Clock            -- TBD
 - 20h UART Status
 - 22h UART Data

### A0h - FFh reserved; Expansion boards


