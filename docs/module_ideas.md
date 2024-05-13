# Module ideas

## LEDs and Switches

Basic version modeled on the RC2014 module, e.g.
  - Address decode
  - 74HCT245 buffer Switches
  - 74HCT273 drive LEDs

Advanced version using 82C55A, e.g.
  - Address decode
  - 82C55A reads switches and drives LEDs directly
  - 2.5mA per pin (sink/drive), need higher LED current limiting resistors
  - Chip needs initialisation before use
  - 24 I/O pins (with restrictions), could have 12 x LEDs, 12 x switches.
  - Maybe generate interrupts on events? (needs investigation)
  - Retro feel :)


## Storage

SD Card and Compact Flash
  - Address decode
  - SD card requires bit twiddling
    - 82C55A (overkill?)
    - PLD/CPLD
    - SPI interface chip?
  - CF direct bus access in 16 bit mode


 ## Time

 Real time clock and interval timer
  - Address decode
  - DS12885 real time clock
    - generate interrupt on alarm?
  - 82C54 interval timer
    - generate 100Hz ticks timer with interrupts


## PiUART

Pi based UART module
  - HDMI output
  - USB keyboard
  - Telnet over wifi
  - Upload over wifi


## PiVideo

 Pi based video module
  - HDMI output
  - USB keyboard
  - Telnet over wifi
  - Upload over wifi
  - Memory mapped graphics modes
    - 512x384 monochrome
    - 320x200 colour


## Real UART

A proper UART, not an emulation
  - 16550 or similar
  - 2 ports ideally
  - 16 bytes buffer (or more)
  - Automatic RTS/CTS controls
