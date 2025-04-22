;----------------------------------------------------------------------
; interrupt vectors
;----------------------------------------------------------------------

    dw int_exc00,0          ; 00 - divide by zero
    dw int_exc01,0          ; 01 - single step
    dw int_exc02,0          ; 02 - NMI
    dw int_exc03,0          ; 03 - breakpoint
    dw int_exc04,0          ; 04 - INTO overflow
    dw int_exc05,0          ; 05 - BOUND range
    dw int_exc06,0          ; 06 - invalid opcode
    dw int_exc07,0          ; 07 - extension not available
    dw int_exc08,0          ; 08 - interrupt table too small
    dw int_exc09,0          ; 09 - extension segment overrun
    dw int_exc0A,0          ; 0A - reserved
    dw int_exc0B,0          ; 0B - reserved
    dw int_exc0C,0          ; 0C - reserved
    dw int_exc0D,0          ; 0D - segment overrun
    dw int_exc0E,0          ; 0E - reserved
    dw int_exc0F,0          ; 0F - reserved
    dw int_exc10,0          ; 10 - extension error
    dw int_exc11,0          ; 11 - reserved
    dw int_exc12,0          ; 12 - reserved
    dw int_exc13,0          ; 13 - reserved
    dw int_exc14,0          ; 14 - reserved
    dw int_exc15,0          ; 15 - reserved
    dw int_exc16,0          ; 16 - reserved
    dw int_exc17,0          ; 17 - reserved
    dw int_exc18,0          ; 18 - reserved
    dw int_exc19,0          ; 19 - reserved
    dw int_exc1A,0          ; 1A - reserved
    dw int_exc1B,0          ; 1B - reserved
    dw int_exc1C,0          ; 1C - reserved
    dw int_exc1D,0          ; 1D - reserved
    dw int_exc1E,0          ; 1E - reserved
    dw int_exc1F,0          ; 1F - reserved
    dw int_irq00,0          ; 20 - irq0
    dw int_irq01,0          ; 21 - irq1
    dw int_irq02,0          ; 22 - irq2
    dw int_irq03,0          ; 23 - irq3
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

