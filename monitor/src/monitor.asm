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


;======================================================================
; menu
;======================================================================

; prompt
prompt:
    mov dx,1                ; echo please
    print mesg_prompt

    call read_chr
    cmp al,'D'
    je  menu_dump
    cmp al,'F'
    je  menu_fill
    cmp al,'H'
    je  menu_help
    cmp al,'I'
    je  menu_inp
    cmp al,'J'
    je  menu_jump
    cmp al,'L'
    je  menu_load
    cmp al,'O'
    je  menu_out
    cmp al,'X'
    je  menu_exe
    jmp prompt

menu_dump:
; dump memory

    push bx
    push cx
    push ds
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

    print mesg_dump

    mov cx,256
    call mem_dump

    pop si
    pop ds
    pop cx
    pop bx
    jmp prompt

menu_fill:
; fill memory

    push ax
    push bx
    push cx
    push di
    push es

    call read_chr           ; space

    call read_hex           ; get segment
    mov bh,al
    call read_hex           ; get segment
    mov bl,al
    mov es,bx

    call read_chr           ; colon

    call read_hex           ; get offset
    mov bh,al
    call read_hex           ; get offset
    mov bl,al
    mov di,bx

    call read_chr           ; space

    call read_hex           ; get count
    xor cx,cx
    mov cl,al

    call read_chr           ; space

    call read_hex           ; get hex byte

    print mesg_fill

    call mem_fill

    pop es
    pop di
    pop cx
    pop bx
    pop ax
    jmp prompt

menu_help:
; help
    print mesg_help
    jmp prompt

menu_inp:
; inp pp

    push dx                 ; save echo state

    call read_chr           ; space
    call read_hex           ; get port
    xor dx,dx
    mov dl,al

    print mesg_inp

    in al,dx                ; INP
    call print_hex

    pop dx
    jmp prompt

menu_jump:
; jump

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

    print mesg_jump

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

    push dx                 ; save echo state

    call read_chr           ; space

    call read_hex           ; get port
    mov bl,al

    call read_chr           ; space
    call read_hex           ; get hex byte

    xor dx,dx
    mov dl,bl

    print mesg_out

    out dx,al               ; OUT

    pop dx
    jmp prompt

menu_exe:
; execute
    print mesg_exe

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

