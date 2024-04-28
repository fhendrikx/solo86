;----------------------------------------------------------------------
; interrupt vectors
;----------------------------------------------------------------------

    dw int_dummy,0          ; 00 - divide by zero
    dw int_dummy,0          ; 01 - single step
    dw int_dummy,0          ; 02 - NMI
    dw int_dummy,0          ; 03 - breakpoint
    dw int_dummy,0          ; 04 - INTO overflow
    dw int_dummy,0          ; 05 - BOUND range
    dw int_dummy,0          ; 06 - invalid opcode
    dw int_dummy,0          ; 07 - extension not available
    dw int_dummy,0          ; 08 - interrupt table too small
    dw int_dummy,0          ; 09 - segment overrun
    dw int_dummy,0          ; 0A - reserved
    dw int_dummy,0          ; 0B - reserved
    dw int_dummy,0          ; 0C - reserved
    dw int_dummy,0          ; 0D - reserved
    dw int_dummy,0          ; 0E - reserved
    dw int_dummy,0          ; 0F - reserved
    dw int_dummy,0          ; 10 - extension error
    dw int_dummy,0          ; 11 - reserved
    dw int_dummy,0          ; 12 - reserved
    dw int_dummy,0          ; 13 - reserved
    dw int_dummy,0          ; 14 - reserved
    dw int_dummy,0          ; 15 - reserved
    dw int_dummy,0          ; 16 - reserved
    dw int_dummy,0          ; 17 - reserved
    dw int_dummy,0          ; 18 - reserved
    dw int_dummy,0          ; 19 - reserved
    dw int_dummy,0          ; 1A - reserved
    dw int_dummy,0          ; 1B - reserved
    dw int_dummy,0          ; 1C - reserved
    dw int_dummy,0          ; 1D - reserved
    dw int_dummy,0          ; 1E - reserved
    dw int_dummy,0          ; 1F - reserved
    dw int_irq0,0           ; 20 - user
    dw int_dummy,0          ; 21 - user
    dw int_dummy,0          ; 22 - user
    dw int_pi,0             ; 23 - user
    dw int_dummy,0          ; 24 - unused
    dw int_dummy,0          ; 25 - unused
    dw int_dummy,0          ; 26 - unused
    dw int_dummy,0          ; 27 - unused
    dw int_dummy,0          ; 28 - unused
    dw int_dummy,0          ; 29 - unused
    dw int_dummy,0          ; 2A - unused
    dw int_dummy,0          ; 2B - unused
    dw int_dummy,0          ; 2C - unused
    dw int_dummy,0          ; 2D - unused
    dw int_dummy,0          ; 2E - unused
    dw int_dummy,0          ; 2F - unused
    dw int_dummy,0          ; 30 - unused
    dw int_dummy,0          ; 31 - unused
    dw int_dummy,0          ; 32 - unused
    dw int_dummy,0          ; 33 - unused
    dw int_dummy,0          ; 34 - unused
    dw int_dummy,0          ; 35 - unused
    dw int_dummy,0          ; 36 - unused
    dw int_dummy,0          ; 37 - unused
    dw int_dummy,0          ; 38 - unused
    dw int_dummy,0          ; 39 - unused
    dw int_dummy,0          ; 3A - unused
    dw int_dummy,0          ; 3B - unused
    dw int_dummy,0          ; 3C - unused
    dw int_dummy,0          ; 3D - unused
    dw int_dummy,0          ; 3E - unused
    dw int_dummy,0          ; 3F - unused

