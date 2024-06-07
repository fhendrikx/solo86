
# ROM signature

offset 0x0000

- 0x000 byte  signature     0x55
- 0x001 byte  signature     0xAA
- 0x002 word  < offset >    offset of entry point
- 0x004 word  < size >      size in 16K blocks
- 0x006 word  < flags >     0x01=Execute in RAM
- 0x008 byte  < name >      24 bytes (padded with spaces)
- 0x020 byte  padding       0x00

offset 0x0100

- 0x100 entry point


# ROM scanning

We will scan ROM addresses 0x80000 through to 0xF0000, in 16K blocks.

If we find the ROM signature, the ROM will be added to the list of known ROMs.

A ROM that has the 'Execute in RAM' flag enabled will be copied to RAM first.

