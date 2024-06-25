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
%include "defines.inc"
%include "vectors.inc"


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

.copy:
    movsw
    loop .copy

    jmp cseg:relocate       ; here we go!


;======================================================================
; 0x00xxx - monitor relocation
;======================================================================

relocate:

; initialise DS
    mov ax,dseg
    mov ds,ax

; initialise stack
    mov ax,sseg
    mov ss,ax               ; SS:SP
    mov sp,0FFFFh

; setup memory banking table
; map RAM into the top half of memory
    ; mov al,011000b
    ; mov dx,bank_table
    ; mov cx,8

; .bank_init:
    ; out dx,al
    ; inc al
    ; inc dx
    ; inc dx
    ; loop .bank_init

; map ROM into the top half of memory, skip monitor
    mov al,1
    out bank_row_0,al
    mov al,2
    out bank_row_1,al
    mov al,3
    out bank_row_2,al
    mov al,4
    out bank_row_3,al
    mov al,5
    out bank_row_4,al
    mov al,6
    out bank_row_5,al
    mov al,7
    out bank_row_6,al
; map RAM into the last segment so that hex loading still works
    mov al,01Fh
    out bank_row_7,al

; enable interrupts
    sti


;======================================================================
; welcome
;======================================================================

; welcome
    print mon_welcome

; scan for ROMs
.scan:
    call rom_scan

    ; do we have any?
    mov dl,[cs:rom_cnt]
    cmp dl,0                ; did we find any?
    je .debugger            ; no

    call rom_menu

.debugger:
    call debug_menu
    jmp .scan


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
%include "ihf.inc"
%include "leds.inc"
%include "mem.inc"
%include "mem_dasm.inc"
%include "messages.inc"
%include "rom.inc"
%include "stdio.inc"


;======================================================================
; data
;======================================================================

; padding
TIMES 65536-1024-($-$$)     db 0FFh

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

