
;======================================================================
; defines
;======================================================================

; important segments
CSEG_INIT       equ 0F000h  ; CS
CSEG            equ 00000h  ; CS after relocation
DSEG            equ 01000h
SSEG            equ 00000h

; ROM scanning (first and last segment to check)
ROM_FIRST       equ 08000h
ROM_LAST        equ 0F000h

ROM_SIZE        equ 0400h   ; ROM size
ROM_CNT         equ 8       ; ROM count (max)


; PIT I/O
PIT_CNTR0       equ 00h
PIT_CNTR0_EN    equ 01h
PIT_SIG         equ 01h
PIT_CNTR1       equ 02h
PIT_CNTR1_EN    equ 03h
PIT_CNTR2       equ 04h
PIT_CNTR2_EN    equ 05h
PIT_CTRL        equ 06h
PIT_LATCH       equ 07h

PIT_FREQ        equ 1000000 ; 1 MHz clock
PIT_TICK        equ PIT_FREQ / 100

; RTC I/O
RTC_CTRL        equ 0Ch     ; was RTC_ADDR
RTC_DATA        equ 0Eh

; BANK I/O
BANK_TABLE      equ 10h
BANK_ROW_0      equ 10h
BANK_ROW_1      equ 12h
BANK_ROW_2      equ 14h
BANK_ROW_3      equ 16h
BANK_ROW_4      equ 18h
BANK_ROW_5      equ 1Ah
BANK_ROW_6      equ 1Ch
BANK_ROW_7      equ 1Eh

; UART I/O
UART_CTRL       equ 20h
UART_SIG        equ 21h
UART_DATA       equ 22h

; PANL I/O
PANL_CTRL       equ 30h
PANL_DATA       equ 30h
PANL_SIG        equ 31h

