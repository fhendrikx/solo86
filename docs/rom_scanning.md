# ROM

Each Solo86 ROM consists of a signature, which outlines the technical detail
of the ROM, followed by the ROM itself. The ROM itself always starts at the
entry point 0x100.


# ROM signature

offset 0x0000 (first 32 bytes are the ROM signature)

- 0x000 word  signature     0x55 0xAA
- 0x002 word  <offset>      offset of entry point
- 0x004 word  <size>        size in 16K blocks
- 0x006 word  <segment>     destination segment (0 = don't care)
- 0x008 word  <flags>       2 = write (implies RAM mapped), 1 = execute (i.e this is code)
- 0x00A word  <res>         reserved
- 0x00C word  <res>         reserved
- 0x00E word  <res>         reserved
- 0x010 byte  <name>        16 ASCIIZ bytes (nul terminated string)
- 0x020 byte  padding       0x00

offset 0x0100

- 0x100 entry point


# ROM scanning

The Solo86 Monitor will scan ROM addresses 0x80000 through to 0xF0000, in 16K blocks.

If the monitor finds a ROM signature, the ROM will be added to the list of known ROMs.

If a ROM has a non-zero destination segment set, then the ROM will be copied to RAM first
(at the segment given) and then executed.

