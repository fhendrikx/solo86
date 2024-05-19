# Paging Design

 - See [paging_thoughts.md](paging_thoughts.md) for ideas considered.
 - The terms paging, banking, and mapping are used interchangeably.

## Goals

 - Flexible paging design that allows for ROM, RAM, and external peripherals to act as memory.
 - Software controlled.
 - Minimise CPLD resources where possible.

## Design

The design provides a very flexible mapping system at the expense of a 40 bit mapping table in the CPLD (therefore at least 40 macro cells). To minimise the number of inputs needed into each macro cell the number of conditions (if statements) is kept as small as possible.

The Solo86 is designed for real-mode operation and assumes only 1 MB of logical address space will be used. The platform has 1 MB of RAM, 1 MB of ROM, and memory mapped peripherals can be connected to the expansion bus.

The bottom half of the address space (0x00000->7FFFF) is mapped exclusively to RAM.
The top half of the address space (0x80000->FFFFF) is broken into 8 x 64kB pages.

A 5 bit row in the banking table controls the mapping of RAM, ROM, and external peripherals into each of the 8 pages.

The row mapping is:
 - Row 0, I/O address 0x10, logical address 0x80000->8FFFF
 - Row 1, I/O address 0x12, logical address 0x90000->9FFFF
 - ...
 - Row 7, I/O address 0x1E, logical address 0xF0000->FFFFF

When writing to a row the 5 LSBs are used, the rest are ignored.
When reading a row the 5 LSBs contain the row value, the remaining bits should be ignored as their value is not guaranteed.

On reset the banking table is set to all zeros (every row contains "00000").

Each row is interpreted as follows:
 - Bit 4 (MSB) 0 == ROM, 1 == RAM or external peripheral
 - Bit 3 hardware signal A19
 - Bit 2 hardware signal A18
 - Bit 1 hardware signal A17
 - Bit 0 (LSB) hardware signal A16

When bit 4 is 1, bit 3 selects between RAM and an external peripheral. 1 == RAM, 0 == external peripheral.

## Examples

```
# ROM
Row 0 = 00000 => logical address 0x80000->8FFFF, ROM physical address 0x0000->0x0FFFF
Row 3 = 00010 => logical address 0xB0000->BFFFF, ROM physical address 0x2000->0x2FFFF
Row 7 = 01111 => logical address 0xF0000->FFFFF, ROM physical address 0xF000->0xFFFFF

# RAM
Row 0 = 11000 => logical address 0x80000->8FFFF, RAM physical address 0x8000->0x8FFFF
Row 3 = 11010 => logical address 0xB0000->BFFFF, RAM physical address 0xA000->0xAFFFF
Row 7 = 11111 => logical address 0xF0000->FFFFF, RAM physical address 0xF000->0xFFFFF

# External Peripheral
Row 0 = 10000 => logical address 0x80000->8FFFF, peripheral address 0x0000->0x0FFFF
Row 3 = 10010 => logical address 0xB0000->BFFFF, peripheral address 0x2000->0x2FFFF
Row 7 = 10111 => logical address 0xF0000->FFFFF, peripheral address 0x7000->0x7FFFF

```



