# Solo/86

This repository contains the tools and binaries required to build and operate
the Solo/86 platform.

## What is Solo/86?

An expandable computer built on the Intel IA16 architecture. The computer
comes with 1MB RAM and 1MB of ROM. Solo/86 utilises a BUS that has been
designed to be easy to use.

As we wanted a clean and simple architecture, the Solo/86 platform is not
compatible with IBM XT and AT-style machines.


## RAM/ROM in Solo/86

The Solo/86 is designed for real-mode operation and assumes only 1 MB of
logical address space will be available. The platform has 1 MB of RAM, 1 MB
of ROM. Memory mapped peripherals can be connected to the expansion bus.

The bottom half of the address space (0x00000->7FFFF) is mapped exclusively
to RAM. This cannot be changed.  However, the top half of the address space
(0x80000->FFFFF) is broken into 4 x 128kB blocks. These can be mapped to ROM,
RAM or an external peripheral.


## Building Solo/86

## What tools do I need to build Solo/86?

You'll need to install the following:
- make
- nasm
- perl


### Building

Load the environment for building:

    source env.sh

Then build the system:

    make

The ROM binaries will be written to the rom/ directory.

Uploadable Intel Hex Files will be written to the hex/ directory. These can be
uploaded directly to a running instance of the monitor using IHF Load command.

