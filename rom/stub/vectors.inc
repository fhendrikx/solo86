;----------------------------------------------------------------------
; interrupt vectors
;----------------------------------------------------------------------

interrupts:
    dw int_dummy            ; 00 - divide by zero
    dw int_dummy            ; 01 - single step
    dw int_dummy            ; 02 - NMI
    dw int_dummy            ; 03 - breakpoint
    dw int_dummy            ; 04 - INTO overflow
    dw int_dummy            ; 05 - BOUND range
    dw int_dummy            ; 06 - invalid opcode
    dw int_dummy            ; 07 - extension not available
    dw int_dummy            ; 08 - interrupt table too small
    dw int_dummy            ; 09 - segment overrun
    dw int_dummy            ; 0A - reserved
    dw int_dummy            ; 0B - reserved
    dw int_dummy            ; 0C - reserved
    dw int_dummy            ; 0D - reserved
    dw int_dummy            ; 0E - reserved
    dw int_dummy            ; 0F - reserved
    dw int_dummy            ; 10 - extension error
    dw int_dummy            ; 11 - reserved
    dw int_dummy            ; 12 - reserved
    dw int_dummy            ; 13 - reserved
    dw int_dummy            ; 14 - reserved
    dw int_dummy            ; 15 - reserved
    dw int_dummy            ; 16 - reserved
    dw int_dummy            ; 17 - reserved
    dw int_dummy            ; 18 - reserved
    dw int_dummy            ; 19 - reserved
    dw int_dummy            ; 1A - reserved
    dw int_dummy            ; 1B - reserved
    dw int_dummy            ; 1C - reserved
    dw int_dummy            ; 1D - reserved
    dw int_dummy            ; 1E - reserved
    dw int_dummy            ; 1F - reserved
    dw int_irq0             ; 20 - user
    dw int_irq1             ; 21 - user
    dw int_irq2             ; 22 - user
    dw int_irq3             ; 23 - user
    dw int_piuart           ; 24 - PiUART
    dw int_timer            ; 25 - 100 Hz timer
    dw int_dummy            ; 26 - unused
    dw int_dummy            ; 27 - unused
    dw int_dummy            ; 28 - unused
    dw int_dummy            ; 29 - unused
    dw int_dummy            ; 2A - unused
    dw int_dummy            ; 2B - unused
    dw int_dummy            ; 2C - unused
    dw int_dummy            ; 2D - unused
    dw int_dummy            ; 2E - unused
    dw int_dummy            ; 2F - unused
    dw int_dummy            ; 30 - unused
    dw int_dummy            ; 31 - unused
    dw int_dummy            ; 32 - unused
    dw int_dummy            ; 33 - unused
    dw int_dummy            ; 34 - unused
    dw int_dummy            ; 35 - unused
    dw int_dummy            ; 36 - unused
    dw int_dummy            ; 37 - unused
    dw int_dummy            ; 38 - unused
    dw int_dummy            ; 39 - unused
    dw int_dummy            ; 3A - unused
    dw int_dummy            ; 3B - unused
    dw int_dummy            ; 3C - unused
    dw int_dummy            ; 3D - unused
    dw int_dummy            ; 3E - unused
    dw int_dummy            ; 3F - unused

interrupts_end:

