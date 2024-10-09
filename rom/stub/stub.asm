;======================================================================
; 80x86 Stub
; Ferry Hendrikx/Dylan Hall, April 2024
;======================================================================

cpu 8086

org 0

;======================================================================
; ROM signature
;======================================================================

                    db 055h
                    db 0AAh
                    dw init
                    dw 04h
                    dw 01h
                    db 'Stub                    ', 00h

TIMES 256-($-$$)    db 0FFh


;======================================================================
; includes
;======================================================================

%include "macro.inc"
%include "vectors.inc"


;======================================================================
; defines
;======================================================================

; important segments
cseg            equ 0F000h
dseg            equ 00000h
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

; initialise interrupt vectors (DS:SI => ES:DI)
    push cs                 ; CS => DS
    pop ds
    mov si,interrupts

    xor di,di
    mov es,di
    mov ax,cseg
    mov cx,64

.copy_vec:
    movsw
    stosw
    loop .copy_vec

; initialise DS
    mov ax,dseg
    mov ds,ax

; initialise SS/SP
    mov ax,sseg
    mov ss,ax
    mov sp,0FFFFh

; ensure UART has interrupts disabled
    xor al,al
    out uart_ctrl,al

; enable interrupts
    sti

; welcome
    print mesg_welcome
    print mesg_copyright
    print mesg_build

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
    cmp al,'p'
    je pitest
    cmp al,'P'
    je pitest
    cmp al,'t'
    je ticks
    cmp al,'T'
    je ticks

    print mesg_unknown_cmd

    jmp menu


dump:
; dump memory
    print mesg_memdump
    mov ax,dseg
    mov ds,ax
    mov si,0
    mov cx,384
    call mem_dump

    jmp menu

ticks:
; toggle the ticks timer on/off
    print mesg_ticks

    in al,ticks_ctrl
    not al
    out ticks_ctrl,al

    jmp menu

pitest:
    print mesg_pi
    mov al,1
    mov [ piactive ],al
    out uart_ctrl,al

.pitest1:
    mov al,[ piactive ]
    cmp al,0
    jne .pitest1

    xor al,al
    out uart_ctrl,al

    mov al,0Ah
    call print_chr
    call print_chr

    jmp menu

piactive:
    db 0

halt:
    hlt
    jmp menu


led_blink:
    call set_leds

    mov ax,500
    call delay_ms

    xor ax,ax
    call set_leds

    ret


;======================================================================
; intr_debug
;======================================================================

int_dummy:
    iret

int_timer:
    push ax
    mov ax,[ ticks_count ]
    inc ax
    mov [ ticks_count ],ax
    call set_leds
    pop ax
    iret

ticks_count:
    dw 0

int_irq0:
    push ax
    mov al,00000001b
    call led_blink
    pop ax
    iret

int_irq1:
    push ax
    mov al,00000010b
    call led_blink
    pop ax
    iret

int_irq2:
    push ax
    mov al,00000100b
    call led_blink
    pop ax
    iret

int_irq3:
    push ax
    mov al,00001000b
    call led_blink
    pop ax
    iret

int_piuart:
    push ax
    in al,uart_data
    cmp al,01Bh             ; ESC
    jne .int_piuart1
    xor al,al
    out uart_ctrl,al        ; disable UART interrupts
    mov [ piactive ],al
.int_piuart1:
    out leds_data,al
    out uart_data,al
    pop ax
    iret


;======================================================================
; includes
;======================================================================

%include "delay.inc"
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


;======================================================================
; filler
;======================================================================

TIMES 65536-($-$$)          db 0FFh

