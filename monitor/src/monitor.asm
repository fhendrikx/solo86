;======================================================================
; 80x86 Monitor
; Ferry Hendrikx, April 2024
;======================================================================

cpu 8086

org 0


;======================================================================
; includes
;======================================================================

%include "macro.inc"


;======================================================================
; defines
;======================================================================

cseg            equ 0F000h
dseg            equ 01000h
sseg            equ 07000h
rseg		equ 00000h	; relocation segment

; I/O addresses should be even numbers only
mem_toggle	equ 06h
leds_data       equ 08h
uart_ctrl       equ 20h
uart_data       equ 22h


;======================================================================
; 0xf0000 - reserved space
;======================================================================

start:
    jmp start


;======================================================================
; 0xf8000 - monitor start
;======================================================================

TIMES 32768-($-$$)      db 00h

init:
    cli
    cld

; initialise SS/SP
    mov ax,sseg
    mov ss,ax
    mov sp,0FFFFh
    
; copy ROM to RAM
    push cs
    pop ds
    xor si,si

    mov ax,rseg
    mov es,ax
    xor di,di

    mov cx,08000h
    
    call mem_copy_w

    jmp rseg:reloc

reloc:
; toggle ROM -> RAM
    out mem_toggle, al		; value of al doesn't matter
    
; initialise interrupts
    push cs
    pop ds

    xor di,di
    mov es,di
    mov si,interrupts
    mov ax,rseg
    mov cx,(interrupts_end - interrupts)/2

.copy:
    movsw               ; off (copied from table)
    stosw               ; seg (AX)
    loop .copy

    sti
    
; initialise DS
    mov ax,dseg
    mov ds,ax

; welcome
    print mesg_welcome

menu:
    print mesg_menu
    call read_chr
    push ax
    call print_chr

    mov al,0Ah
    call print_chr
    mov al,0Ah
    call print_chr
    pop ax

    cmp al,'d'
    je dump
    cmp al,'D'
    je dump

    cmp al,'h'
    je halt
    cmp al,'H'
    je halt

    cmp al,'r'
    je run
    cmp al,'R'
    je run

    cmp al,'u'
    je upload
    cmp al,'U'
    je upload

    print mesg_unknown_cmd
    
    jmp menu

upload:	
; prep upload    
    print mesg_upload

; initialise ES
    mov ax,cseg
    mov es,ax

; read hex file
    call read_hex_file
    print mesg_upload_done

    jmp menu

dump:	
; dump memory
    print mesg_memdump
    mov ax,cseg
    mov ds,ax
    mov si,0
    mov cx,384
    call mem_dump

    jmp menu

    
run:	
; reset
    print mesg_restart
    xor ax,ax
    mov ds,ax
    mov es,ax
    mov ss,ax

    mov ax,2h
    push ax
    popf
    jmp cseg:start

    
halt:
    hlt
    jmp halt


;======================================================================
; intr_debug
;======================================================================

int_dummy:
    iret

int_pi:

    push ax

    in al,uart_data
    out leds_data,al
    out uart_data,al

    pop ax

    iret


;======================================================================
; interrupt offsets
;======================================================================

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
    dw int_pi               ; 20 - user
interrupts_end:


;======================================================================
; includes
;======================================================================

TIMES 65536-1024-($-$$)     db 0FFh

%include "delay.inc"
%include "intel.inc"
%include "leds.inc"
%include "mem.inc"
%include "messages.inc"
%include "stdio.inc"


;======================================================================
; reset code (called on CPU reset)
;======================================================================

TIMES 65536-16-($-$$)       db 0FFh

reset:
    jmp cseg:init

build:
    db __DATE__
    db 0

