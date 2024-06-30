
;======================================================================
; defines
;======================================================================

; important segments
cseg_init       equ 0F000h  ; CS
cseg            equ 00000h  ; CS after relocation
dseg            equ 01000h
sseg            equ 00000h


; I/O addresses (should be even numbers only)
ticks_ctrl      equ 04h
leds_data       equ 08h

bank_table      equ 10h
bank_row_0      equ 10h
bank_row_1      equ 12h
bank_row_2      equ 14h
bank_row_3      equ 16h
bank_row_4      equ 18h
bank_row_5      equ 1Ah
bank_row_6      equ 1Ch
bank_row_7      equ 1Eh

uart_ctrl       equ 20h
uart_data       equ 22h


; ROM scanning (first and last segment to check)
rom_first       equ 08000h
rom_last        equ 0F000h

rom_size        equ 0400h   ; ROM size
rom_max         equ 8       ; ROM maximum

; segment for relocation
rom_relocate    equ 01000h

