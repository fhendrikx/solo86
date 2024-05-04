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
mem_toggle      equ 06h
leds_data       equ 08h
uart_ctrl       equ 20h
uart_data       equ 22h


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

; initialise SS/SP
    mov ax,sseg
    mov ss,ax
    mov sp,0FFFFh

; toggle ROM -> RAM
    out mem_toggle, al      ; value of al doesn't matter

; enable interrupts
    sti


;======================================================================
; welcome
;======================================================================

; welcome
    print mesg_welcome
    print mesg_copyright
    print mesg_build


;======================================================================
; menu
;======================================================================

; prompt
prompt:
    print mesg_prompt

    call read_chr
    cmp al,'d'
    je  menu_dump
    cmp al,'h'
    je  menu_help
    cmp al,'i'
    je  menu_inp
    cmp al,'j'
    je  menu_jump
    cmp al,'l'
    je  menu_load
    cmp al,'o'
    je  menu_out
    cmp al,'r'
    je  menu_dump
    cmp al,'x'
    je  menu_exe
    jmp prompt

menu_dump:
; dump memory
    print mesg_dump

    push ds
    push bx
    push si

    call read_chr           ; space

    call read_hex           ; get segment
    mov bh,al
    call read_hex           ; get segment
    mov bl,al
    mov ds,bx

    call read_chr           ; colon

    call read_hex           ; get offset
    mov bh,al
    call read_hex           ; get offset
    mov bl,al
    mov si,bx

    mov cx,256
    call mem_dump

    pop si
    pop bx
    pop ds
    jmp prompt

menu_help:
; help
    print mesg_help
    jmp prompt

menu_inp:
; inp pp
    print mesg_inp
    push dx
    xor dx,dx
    call read_hex           ; get port
    mov dl,al
    in al,dx                ; INP
    call print_hex
    pop dx
    jmp prompt

menu_jump:
; jump
    print mesg_jump

    call read_chr           ; space
    call read_hex           ; get segment
    mov bh,al
    call read_hex           ; get segment
    mov bl,al
    mov [ start_segment ], bx

    call read_chr           ; colon

    call read_hex           ; get offset
    mov bh,al
    call read_hex           ; get offset
    mov bl,al
    mov [ start_address ], bx
; indirect far jump to CS:IP
    jmp far [ start_address ]

menu_load:
; load intel hex file
    print mesg_load
    call read_hex_file
    print mesg_load_done
    jmp prompt

menu_out:
; out pp xx
    print mesg_out
    push dx

    xor dx,dx
    call read_hex           ; get port
    mov dl,al
    call read_chr           ; space
    call read_hex           ; get hex
    out dx,al

    pop dx
    jmp prompt

menu_exe:
; execute
    print mesg_out

    xor ax,ax
    mov ds,ax
    mov es,ax
    mov ss,ax

    mov ax,2h
    push ax
    popf
; indirect far jump to CS:IP
    jmp far [ start_address ]


;======================================================================
; intr_dummy
;======================================================================

int_dummy:
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

TIMES 65536-64-($-$$)       db 0FFh

start_address:
    dw 00h
start_segment:
    dw 00h


;======================================================================
; reset code (called on CPU reset)
;======================================================================

TIMES 65536-16-($-$$)       db 0FFh

reset:
    jmp cseg_init:init


;======================================================================
; filler
;======================================================================

TIMES 65536-($-$$)          db 0FFh

