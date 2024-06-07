;======================================================================
; Solo86 Monitor
; Ferry Hendrikx/Dylan Hall, April 2024
;======================================================================

cpu 8086

org 0


;======================================================================
; includes
;======================================================================

%include "macro.inc"
%include "vectors.inc"


;======================================================================
; defines
;======================================================================

; important segments
cseg_init       equ 0F000h  ; CS
cseg            equ 00000h  ; CS after relocation
dseg            equ 01000h
sseg            equ 00000h

; I/O addresses should be even numbers only
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


;======================================================================
; 0xf0100 - monitor start
;======================================================================

init:
; clear interrupts
    cli
    cld

; ensure UART has interrupts disabled
    xor al,al
    out uart_ctrl,al

; copy ROM to RAM
; don't use the stack yet as the ROM copy will overwrite any stored data
    mov ax,cseg_init
    mov ds,ax
    xor si,si

    mov ax,cseg
    mov es,ax
    xor di,di

    mov cx,08000h

.copy_rom:
    movsw
    loop .copy_rom

    jmp cseg:relocate


;======================================================================
; 0x00xxx - monitor relocation
;======================================================================

relocate:
; initialise DS
    mov ax,dseg
    mov ds,ax

; initialise stack
    mov ax,sseg
    mov ss,ax           ; SS:SP
    mov sp,0FFFFh

; setup memory banking table
; map RAM into the top half of memory
    mov al,011000b
    mov dx,bank_table
    mov cx,8
.bank_init:
    out dx,al
    inc al
    inc dx
    inc dx
    loop .bank_init

; enable interrupts
    sti


;======================================================================
; welcome
;======================================================================

; welcome
    print mesg_welcome
    print mesg_copyright
    print mesg_build


    call rom_scan
    call rom_display


    call prompt


.loopx:
    jmp .loopx

    cli
    hlt

;======================================================================
; intr_dummy
;======================================================================

int_dummy:
    iret


;======================================================================
; includes
;======================================================================

%include "debug.inc"
%include "delay.inc"
%include "intel.inc"
%include "leds.inc"
%include "mem.inc"
%include "messages.inc"
%include "rom.inc"
%include "stdio.inc"


;======================================================================
; data
;======================================================================

; padding
TIMES 65536-1024-($-$$)     db 0FFh

; HEX address
hex_ofs:                    dw 00h
hex_seg:                    dw 00h

; ROM address
rom_ofs:                    dw 00h
rom_seg:                    dw 00h

; ROM data (8 entries)
rom_table:
%rep rom_max
                            dw 00h  ; segment
                            dw 00h  ; offset
                            dw 00h  ; size
                            dw 00h  ; flags
    TIMES 24                db 00h  ; name (24 bytes)
%endrep


;======================================================================
; reset code (called on CPU reset)
;======================================================================

; padding
TIMES 65536-16-($-$$)       db 0FFh

reset:
    jmp cseg_init:init


;======================================================================
; filler
;======================================================================

TIMES 65536-($-$$)          db 0FFh

