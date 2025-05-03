
# Solo/86 Hardware

This directory contains the specifications, instructions and any code for the Solo/86 hardware.

The key components of the Solo/86 system are:

- Main; the Mainboard
- UART expansion card; the Terminal (Keyboard, Mouse)
- Epoch expansion card; the clock and timer
- Console expansion card; the "Control Panel"
- Panel expansion card; the "Control Panel" (a version of Console that contains everything on one card)


## Requirements

At the minimum, you will need to build:

- Main
- UART expansion card

If you want to run an operating system (such as ELKS), you will need to build:

- Main
- UART expansion card
- Epoch expansion card

A Console or Panel expansion card should also be built if you want to select Solo/86 options at boot time, or need hardware debugging.


## Card orientation

Each expansion card card should have a male slot soldered to it's front. When seated on the mainboard, all the cards should face the front of the mainboard (i.e. where the Solo86 words can be found). The only possible exception is the last slot on the mainboard, this may have a 90 degree angled male slot, allowing you to easily work on new or experimental expansion cards.

![Card slot](image/card1.jpg)

![Card slot](image/card2.jpg)

