
# HEX files

## Uploading a HEX file.

The Solo/86 monitor can be used to upload Intel Hex files into RAM.
This is a great way to test ROM code changes before burning anything
to a ROM.

Each hex file must contain a segment destination address. The monitor
will use this to load the code into the correct address in RAM. When
building a hex file, the bin2hex tool takes 2 parameters: the file and
the segment to which it should be loaded.

For example, a ROM which will eventually execute from segment E000 can
be converted into a hex file as follows:

    $ perl ../bin/bin2hex --file=rom.bin --seg=e000 > rom.hex

In a separate terminal, you can now telnet into the monitor, and ensure
that e000 is mapped as RAM (otherwise the load will not do anything):

    telnet solo

    MON$ r e000 01

Then, the load can be started in the monitor:

    MON$ L

In another terminal you can now start your hex file upload using the
"up" script:

    $ ./up rom.hex

The monitor will signal when the load has finished, and you can now either
execute the code (using E) or jump to some arbitrary address within it (using
J).

    MON$ J e000:0100

