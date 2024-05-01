;======================================================================
; 80x86 Monitor
; Ferry Hendrikx/Dylan Hall, April 2024
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

; important segments
cseg            equ 0F000h
dseg            equ 01000h
sseg            equ 00000h
rseg            equ 00000h  ; relocation segment
useg            equ 01000h  ; user segment

; I/O addresses should be even numbers only
mem_toggle      equ 06h
leds_data       equ 08h
uart_ctrl       equ 20h
uart_data       equ 22h


;======================================================================
; 0xf0100 - monitor start
;======================================================================

%include "vectors.inc"

init:
    cli
    cld

; copy ROM to RAM
; don't use the stack yet as the ROM copy will overwrite any stored data
    mov ax,cseg
    mov ds,ax
    xor si,si

    mov ax,rseg
    mov es,ax
    xor di,di

    mov cx,08000h

.rom_copy:
    movsw
    loop .rom_copy

    jmp rseg:reloc

reloc:
; toggle ROM -> RAM
    out mem_toggle, al      ; value of al doesn't matter

; initialise SS/SP
    mov ax,sseg
    mov ss,ax
    mov sp,0FFFFh

; emable interrupts
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
; indirect far jump to CS:IP
    jmp far [ start_address ]


halt:
    hlt
    jmp menu


;======================================================================
; intr_debug
;======================================================================

int_dummy:
    iret

int_irq0:

    push ax

    mov al,1
    call set_leds

    mov ax,1000
    call delay_ms

    xor ax,ax
    call set_leds

    pop ax

    iret


int_pi:
    push ax

    in al,uart_data
    out leds_data,al
    out uart_data,al

    pop ax

    iret


;======================================================================
; includes
;======================================================================

%include "delay.inc"
%include "intel.inc"
%include "leds.inc"
%include "mem.inc"
%include "messages.inc"
%include "stdio.inc"


;======================================================================
; intel hex file start address
;======================================================================

start_address:
    dw 0
start_segment:
    dw 0

;======================================================================
; reset code (called on CPU reset)
;======================================================================

TIMES 65536-16-($-$$)       db 0FFh

reset:
    jmp cseg:init

build:
    db __DATE__
    db 0

