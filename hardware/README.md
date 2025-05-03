
# Solo/86 Hardware

This directory contains the specifications, instructions and any code for the Solo/86 hardware.

The key components of the Solo/86 system are:

- [Main](solo86/hardware/main/README.md) the Mainboard
- [UART](solo86/hardware/uart/README.md) the Terminal (Keyboard, Mouse)
- [Epoch](solo86/hardware/epoch/README.md) the clock and timer
- [Console](solo86/hardware/console/README.md) the "Control Panel"
- [Panel](solo86/hardware/panel/README.md) the "Control Panel" (a version of Console that contains everything on one card)


## What do I need to build Solo/86?

You'll need some experience building electronics and some ability to debug hardware, if things go wrong.

All the parts are through-hole, so you'll need just a plain soldering iron and some patience.

At the minimum you'll need:

- Multi-meter
- Soldering Iron
- Programmer for CPLD (e.g. XXXXXX)
- Programmer for PLDs (e.g. TL866II plus)

(although pre-programmed CPLD and PLDs may be available upon request)


## Which parts do I need to build?

At the minimum, you will need to build:

- Main
- UART expansion card

If you want to run an operating system (such as ELKS), you will need to build:

- Main
- UART expansion card
- Epoch expansion card

A Console or Panel expansion card should also be built if you want to select Solo/86 options at boot time, or need hardware debugging.


