
# Solo/86 Hardware

This directory contains the specifications, instructions and any firmware for the Solo/86 hardware.

<img src="hardware.jpg" alt="Solo/86 Hardware" width="500">


## Introduction

The Solo/86 mainboard contains a CPU, RAM, ROM and a CPLD. The system relies heavily on the CPLD as this contains the core logic (that replaces a whole series of chips on other computers). At a minimum, the Mainboard and UART expansion card will be required to boot the system to the Solo/86 Monitor.

The Solo/86 is designed for real-mode operation and assumes only 1 MB of logical address space will be available. The platform has 1 MB of RAM, and 1 MB of ROM. Memory mapped peripherals can be connected to the expansion bus. The bottom half of the address space (0x00000->7FFFF) is mapped exclusively to RAM. This cannot be changed. However, the top half of the address space (0x80000->FFFFF) is broken into 4 x 128kB blocks. These can be mapped to ROM, RAM or an external peripheral.


## Components

The key components of the Solo/86 system are:

- [Mainboard](/hardware/main/README.md) - Mainboard
- [UART](/hardware/uart/README.md) - Terminal (DIsplay, Keyboard, Mouse and TELNET access)
- [Console](/hardware/console/README.md) - "Control Panel"
- [Epoch](/hardware/epoch/README.md) - Clock and Programmable Interval Timer
- [Flash](/hardware/flash/README.md) - Compact Flash storage


## What do I need to build Solo/86?

You'll need some experience building electronics and some ability to debug hardware, if things go wrong.

All the parts are through-hole, so you'll need just a plain soldering iron and some patience.

At the minimum you'll need:

- Multi-meter
- Soldering Iron
- Programmer for CPLD (e.g. OpenOCD or maybe a Terasic Blaster)
- Programmer for PLDs (e.g. TL866II plus)

(although pre-programmed CPLD and PLDs may be available upon request)


## Which components do I need to build?

At the minimum, you will need to build the following components:

- Mainboard
- UART expansion card

If you want to run an operating system (such as ELKS), you will need to build:

- Mainboard
- UART expansion card
- Epoch expansion card
- Flash expansion card

A Console or Panel expansion card should also be built if you want to select Solo/86 options at boot time, or need hardware debugging.

