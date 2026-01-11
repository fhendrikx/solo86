# Front Panel Switches

Left to right (bit 7 down to 0)

## bit 7 Console Selection
 - 0 == PiUART
 - 1 == 16550 UART port A

## bits 6-5 16550 UART Baud Rate

 - 00 == 115200
 - 01 == 38400
 - 10 == 9600
 - 11 == 1200

## bit 4 Auto boot or Monitor

 - 0 == Monitor
 - 1 == Auto boot

## bits 3-1 Auto boot selection

 - 000 -> 111 ROM application 0 -> 7

 ## bit 0 Start up beep

 - 0 == No PIT BEEP on start up
 - 1 == PIT BEEP on start up

 