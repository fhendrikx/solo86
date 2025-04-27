
# Paging Design

 - See [paging_thoughts.md](paging_thoughts.md) for ideas considered.
 - The terms paging, banking, and mapping are used interchangeably.


## Goals

 - Flexible paging design that allows for ROM, RAM, and external peripherals to act as memory.
 - Software controlled.
 - Minimise CPLD resources where possible.


## Design

The design provides a very flexible mapping system at the expense of a 16 bit mapping table in the CPLD (therefore at least 16 macro cells). To minimise the number of inputs needed into each macro cell the number of conditions (if statements) is kept as small as possible.

Solo/86 is designed for real-mode operation and assumes only 1 MB of logical address space will be used. The platform has 1 MB of RAM, 1 MB of ROM, and memory mapped peripherals can be connected to the expansion bus. The bottom half of the address space (0x00000->7FFFF) is mapped exclusively to RAM. The top half of the address space (0x80000->FFFFF) is broken into 4 x 128kB pages. A 4 bit row in the banking table controls the mapping of RAM, ROM, and external peripherals into each of the 4 pages.

The row mapping is:
 - Row 0, I/O address 0x10, logical address 0x80000->9FFFF
 - Row 1, I/O address 0x12, logical address 0xA0000->BFFFF
 - Row 2, I/O address 0x14, logical address 0xC0000->DFFFF
 - Row 3, I/O address 0x16, logical address 0xE0000->FFFFF

When writing to a row the 4 LSBs are used, the remaining bits are reserved. When reading a row the 4 LSBs contain the row value, the remaining bits should be ignored as their value is reserved.

Each row is interpreted as follows:
 - Bit 3 (MSB) 0 == ROM, 1 == RAM / external peripheral
 - Bit 2 hardware signal A19
 - Bit 1 hardware signal A18
 - Bit 0 (LSB) hardware signal A17

When bit 3 is 1, bit 2 selects between RAM and an external peripheral. 1 == RAM, 0 == external peripheral.

The following table illustrates all possible mappings:

(0) 0000 => ROM 0x00000 -> 0x1FFFF
(1) 0001 => ROM 0x20000 -> 0x3FFFF
(2) 0010 => ROM 0x40000 -> 0x5FFFF
(3) 0011 => ROM 0x60000 -> 0x7FFFF
(4) 0100 => ROM 0x80000 -> 0x9FFFF
(5) 0101 => ROM 0xA0000 -> 0xBFFFF
(6) 0110 => ROM 0xC0000 -> 0xDFFFF
(7) 0111 => ROM 0xE0000 -> 0xFFFFF

(8) 1000 => External Peripheral 0x00000 -> 0x1FFFF
(9) 1001 => External Peripheral 0x20000 -> 0x3FFFF
(A) 1010 => External Peripheral 0x40000 -> 0x5FFFF
(B) 1011 => External Peripheral 0x60000 -> 0x7FFFF

(C) 1100 => RAM 0x80000 -> 0x9FFFF
(D) 1101 => RAM 0xA0000 -> 0xBFFFF
(E) 1110 => RAM 0xC0000 -> 0xDFFFF
(F) 1111 => RAM 0xE0000 -> 0xFFFFF


## Power on state

On reset the banking table is set to all zeros (every row contains "0000"). e.g.

 - Row 1, logical address 0x80000->0x9FFFF, ROM physical address 0x00000->0x1FFFF.
 - Row 2, logical address 0xA0000->0xBFFFF, ROM physical address 0x00000->0x1FFFF.
 - Row 3, logical address 0xC0000->0xDFFFF, ROM physical address 0x00000->0x1FFFF.
 - Row 4, logical address 0xE0000->0xFFFFF, ROM physical address 0x00000->0x1FFFF.

This means code designed to run on reset should reside at 0x1FFF0 in the ROM.

## Example mappings

```
# ROM
Row 0 = 0000 => logical address 0x80000->9FFFF, ROM physical address 0x00000->0x1FFFF
Row 3 = 0011 => logical address 0xE0000->FFFFF, ROM physical address 0x60000->0x7FFFF

# RAM
Row 0 = 1100 => logical address 0x80000->9FFFF, RAM physical address 0x80000->0x9FFFF
Row 3 = 1111 => logical address 0xE0000->FFFFF, RAM physical address 0xE0000->0xFFFFF

# External Peripheral
Row 0 = 1000 => logical address 0x80000->9FFFF, peripheral address 0x00000->0x1FFFF
Row 3 = 1011 => logical address 0xE0000->FFFFF, peripheral address 0x60000->0x7FFFF

```

