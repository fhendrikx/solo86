
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

; BANKING I/O
BNK_PRESENT     equ 01h
BNK_TABLE       equ 10h
BNK_ROW_0       equ 10h
BNK_ROW_1       equ 12h
BNK_ROW_2       equ 14h
BNK_ROW_3       equ 16h
BNK_ROW_4       equ 18h
BNK_ROW_5       equ 1Ah
BNK_ROW_6       equ 1Ch
BNK_ROW_7       equ 1Eh

; PIT I/O
PIT_PRESENT     equ 02h
PIT_CNTR0       equ 00h
PIT_CNTR0_EN    equ 01h
PIT_DETECT      equ 01h
PIT_CNTR1       equ 02h
PIT_CNTR1_EN    equ 03h
PIT_CNTR2       equ 04h
PIT_CNTR2_EN    equ 05h
PIT_CTRL        equ 06h
PIT_LATCH       equ 07h

PIT_FREQ        equ 1000000 ; 1 MHz clock
PIT_TICK        equ PIT_FREQ / 100

; RTC I/O
RTC_PRESENT     equ 10h
RTC_CTRL        equ 0Ch     ; was RTC_ADDR
RTC_DATA        equ 0Eh

; COM I/O
COM_PRESENT     equ 20h
COM_CTRL        equ 20h
COM_DETECT      equ 21h
COM_DATA        equ 22h

; PANEL I/O
PNL_PRESENT     equ 80h
PNL_CTRL        equ 30h
PNL_DATA        equ 30h
PNL_DETECT      equ 31h

