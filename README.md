# Solo86

This repository contains the tools and binaries required to build and operate the Solo86 platform.

## What is Solo86?

An expandable computer built on the Intel IA16/32 architecture. The computer comes with 1MB RAM and 1MB of ROM. Solo 86 utilises a BUS that has been designed to be easy to use.

As we wanted a clean and simple architecture, the Solo86 platform is not compatible with IBM XT and AT-style machines.


## RAM/ROM in Solo86

The Solo86 is designed for real-mode operation and assumes only 1 MB of logical address space will be available. The platform has 1 MB of RAM, 1 MB of ROM. Memory mapped peripherals can be connected to the expansion bus.

The bottom half of the address space (0x00000->7FFFF) is mapped exclusively to RAM. This cannot be changed. However, the top half of the address space (0x80000->FFFFF) is broken into 8 x 64kB pages. These can be mapped to ROM or an external peripheral.

