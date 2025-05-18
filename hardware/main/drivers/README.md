# Clock Drivers

Earlier versions of the main board had stability issues caused by the PCB track for the clock signal being too long which resulted in reflections that distorted the signal.

This was partly addressed with better track routing, but it was decided to split the clock signal in two, one going to the CPU and CPLD, and the other going to the expansion bus. This results in two shorter traces, but also isolates the CPU and CPLD from problems on the expansion bus.

The output of the crystal is fed into a pair of buffers from a 74xx125 to create the new clock signals.

This directory contains scope captures of different logic families to see which resulted in the least distorted clock signals.

Single is looking at the clock on the expansion bus.

Dual is comparing the raw clock signal with the expansion bus. 

If I remember more about the differences between the tests I'll update this.

We've settled on the AHCT drivers.