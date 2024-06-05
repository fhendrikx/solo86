# ROM signature

offset 0x0000

 - byte    0x55
 - byte    0xAA
 - word    < size > in 16K blocks
 - word    < addr > of entry point
 - word    reserved
 - byte    < name > maximum 24 bytes
 - byte    0x00 padding

offset 0x0100

 - code entry point


# ROM scanning

We will scan ROM addresses 0x80000 through to 0xF0000, in 16K blocks.

If we find the ROM signature, the ROM will be added to the list of known ROMs.

