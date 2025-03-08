
;======================================================================
; defines
;======================================================================

; important segments
CSEG_INIT       equ 0F000h  ; CS
CSEG            equ 00000h  ; CS after relocation
DSEG            equ 01000h
SSEG            equ 00000h


; I/O addresses (should be even numbers only)
TICK_CMD        equ 04h
LEDS_DATA       equ 30h

BANK_TABLE      equ 10h
BANK_ROW_0      equ 10h
BANK_ROW_1      equ 12h
BANK_ROW_2      equ 14h
BANK_ROW_3      equ 16h
BANK_ROW_4      equ 18h
BANK_ROW_5      equ 1Ah
BANK_ROW_6      equ 1Ch
BANK_ROW_7      equ 1Eh

UART_CMD        equ 20h
UART_DATA       equ 22h


; ROM scanning (first and last segment to check)
ROM_FIRST       equ 08000h
ROM_LAST        equ 0F000h

ROM_SIZE        equ 0400h   ; ROM size
ROM_MAX         equ 8       ; ROM maximum

