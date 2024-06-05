
# ROM signature

offset 0x0000

- 0x000 word  0x55, 0xAA
- 0x002 word  < size > size in 16K blocks
- 0x004 word  < addr > addr of entry point
- 0x006 word  < flag > 0x01=RAM
- 0x008 byte  < name > 24 bytes
- 0x020 byte  0x00 padding

offset 0x0100

0x100 entry point


# ROM scanning

We will scan ROM addresses 0x80000 through to 0xF0000, in 16K blocks.

If we find the ROM signature, the ROM will be added to the list of known ROMs.

A ROM that has the 'RAM' flag enabled should be copied to RAM first.

