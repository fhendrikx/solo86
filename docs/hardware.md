
# Hardware


## Interrupts

### 00h - 1Fh reserved; System (hardware/faults)

### 20h - 3Fh reserved; System

 - 20h IRQ0 (Timer)
 - 21h IRQ1
 - 22h IRQ2
 - 23h IRQ3 (UART)
 - 24h IRQ4
 - 25h IRQ5

### 40h - 7Fh reserved; System

### 80h - 80h reserved; System

 - 80h System Call (ELKS)


## IO Ports

### 00h - 7Fh reserved; System

 - 04h Timer
 - 08h LED
 - 10h Bank table
 - 20h Clock
 - 20h UART Status      -- can we move this out to 30h?
 - 22h UART Data        -- can we move this out to 32h?

### 80h - FFh reserved; System Expansion

 - 80h Slot 1
 - 90h Slot 2
 - A0h Slot 3
 - B0h Slot 4
 - C0h Slot 5
 - D0h Slot 6
 - E0h Slot 7
 - F0h Slot 8

