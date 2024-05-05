# Paging Thoughts

Want the ability to page between ROM and RAM.
The swap needs to be triggered from software.
At power up we must default to ROM at the top of memory (FFFF0) for initial JMP.
We have 1MB ROM, more than needs to be presented at any time.

## Initial implementation

At power up 512k RAM (lower), 512k ROM (upper).
Suits ELKS which wants 512k RAM, excess ROM which could be used as a ROM disk.
Hardware switch to toggle between bottom half or top half of ROM.
Writes always go to RAM.

ROM->RAM swap with:

`out mem_ctrl,1		; swaps ROM->RAM`

One way swap, e.g. you can't swap back to ROM (why would you want to?)

1 macrocell consumed to determine whether ROM or RAM should be active.
Initial state of macrocell determined with jumper (to allow booting from RAM after a warm reset).


## Possible enhancements

Goals:
 - More flexible paging
 - Software controlled
 - Optional hardware control (switches)
 - Ability to disable RAM + ROM so external memory mapped hardware can respond (video card)


Possible boot process:
1. Launch Monitor (held in 64k ROM)
2. Copy Monitor to 00000, contine executing from RAM
3. Determine desired application/memory layout, update paging
4. Jump to appropriate location

## Fully flexible paging
 
Top 512k broken into 8 x 64k pages.
Any of the 16 x ROM 64k can be assigned to any of the 8 pages.
Bottom 512k always RAM.

Want option to switch to RAM.
Want option to disable RAM + ROM so external memory mapped hardware can respond.

Each of the 8 pages needs 18 possible states (16 x 64k ROM, 1 x 64k RAM, 1 x OFF state)
5 bits per page, 8 pages => 40 macrocells minimum.

Default state "00000" (easy for CPLD), so map the first 64k of ROM to all 8 pages at power up.


## Cheaper flexible paging

As above, but bottom 768k RAM, top 256k pagable.
So 5 bits per page, 4 pages => 20 macrocells minimum.
If a ROM disk is wanted it could reside in a single 64k window then be paged in 64k at a time.


## Mode based paging

Fixed memory layouts, for example:

mode 0 960k RAM, 64k ROM (00000-0FFFF) [default, ROM holds Monitor]
mode 1 960k RAM, 64k ROM (10000-1FFFF)
mode 2 896k RAM, 128k ROM (20000-3FFFF)
mode 3 768k RAM, 256k ROM (40000-7FFFF)
mode 4 512k RAM, 512k ROM (80000-FFFFF)
mode 5 960k RAM, 64k memory mapped peripheral (video)
mode 6 896k RAM, 128k memory mapped peripheral (video)
mode 7 1024k RAM

=> 3 macrocells minimum

Not the cleanest way of including memory mapped peripherals. 



