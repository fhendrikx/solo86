
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

; Interrupt Controller
INT_CTRL        equ 18h
INT_EOI         equ 1Ah

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

; Video Control I/O
VC_CTRL         equ 28h
VC_PARAM        equ 2Ah
VC_DATA         equ 2Ch

; PANEL I/O
PNL_PRESENT     equ 80h
PNL_CTRL        equ 30h
PNL_DATA        equ 30h
PNL_DETECT      equ 31h

; ATA I/O
ATA_DATA        equ 40h
ATA_ERROR       equ 42h
ATA_FEAT        equ 42h
ATA_COUNT       equ 44h
ATA_LBA_LO      equ 46h
ATA_LBA_MD      equ 48h
ATA_LBA_HI      equ 4Ah
ATA_DRVH        equ 4Ch
ATA_CMND        equ 4Eh
ATA_STATUS      equ 4Eh
ATA_CTRL        equ 5Ch

