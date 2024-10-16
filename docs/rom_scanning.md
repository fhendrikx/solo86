
# ROM signature

offset 0x0000

- 0x000 word  signature     0x55 0xAA
- 0x002 word  <offset>      offset of entry point
- 0x004 word  <size>        size in 16K blocks
- 0x006 word  <segment>     destination segment (0 if execute in place)
- 0x008 byte  <name>        24 bytes (padded with spaces)
- 0x020 byte  padding       0x00

offset 0x0100

- 0x100 entry point


# ROM scanning

The Solo86 Monitor will scan ROM addresses 0x80000 through to 0xF0000, in 16K blocks.

If the monitor finds a ROM signature, the ROM will be added to the list of known ROMs.

If a ROM has a non-zero destination segment set, then the ROM will be copied to RAM first
(at the segment given) and then executed.

