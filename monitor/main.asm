;======================================================================
; 80x86 Monitor
; Ferry Hendrikx, April 2024
;======================================================================

cpu 8086

%include "config.inc"
%include "delay.inc"
%include "led.inc"
%include "macro.inc"
%include "print.inc"

;======================================================================
; defines
;======================================================================

cseg        equ 0F000h
dseg        equ 01000h
sseg        equ 07000h

port_led    equ 08h
port_uart   equ 16h

org         START


;======================================================================
; monitor start
;======================================================================

start:
; initialise SS/SP
    mov ax,sseg
    mov ss,ax
    mov sp,0FFFFh

    cli
    cld

; initialise interrupts
    push cs
    pop ds

    xor di,di
    mov es,di
    mov si,interrupts
    mov ax,cseg
    mov cx,32
.copy:
    movsw               ; off (copied from table)
    stosw               ; seg (AX)
    loop .copy

; initialise DS
    mov ax,dseg
    mov ds,ax


; stage 1
.start_test:
    mov al,1
    call set_leds
    mov ax,2000
    call delay_ms

    int 3


; stage 2
    mov al,2
    call set_leds
    mov ax,2000
    call delay_ms

    int 3


; stage 3
    mov al,4
    call set_leds
    mov ax,2000
    call delay_ms

    mov bh,8
    mov al,0
    div bh              ; div by zero


; finish
    mov al,0
    call set_leds
    jmp .start_test


halt:
    hlt
    jmp halt


;======================================================================
; intr_debug
;======================================================================

int_debug:
    mov al,11110000b
    call set_leds
    iret


;======================================================================
; interrupts
;======================================================================

setloc 0FF00h

interrupts:
    dw int_debug        ; 00 - divide by zero
    dw int_debug        ; 01 - single step
    dw int_debug        ; 02 - NMI
    dw int_debug        ; 03 - breakpoint
    dw int_debug        ; 04 - INTO overflow
    dw int_debug        ; 05 - BOUND range
    dw int_debug        ; 06 - invalid opcode
    dw int_debug        ; 07 - extension not available
    dw int_debug        ; 08 - interrupt table too small
    dw int_debug        ; 09 - segment overrun
    dw int_debug        ; 0A - reserved
    dw int_debug        ; 0B - reserved
    dw int_debug        ; 0C - reserved
    dw int_debug        ; 0D - reserved
    dw int_debug        ; 0E - reserved
    dw int_debug        ; 0F - reserved
    dw int_debug        ; 10 - extension error
    dw int_debug        ; 11 - reserved
    dw int_debug        ; 12 - reserved
    dw int_debug        ; 13 - reserved
    dw int_debug        ; 14 - reserved
    dw int_debug        ; 15 - reserved
    dw int_debug        ; 16 - reserved
    dw int_debug        ; 17 - reserved
    dw int_debug        ; 18 - reserved
    dw int_debug        ; 19 - reserved
    dw int_debug        ; 1A - reserved
    dw int_debug        ; 1B - reserved
    dw int_debug        ; 1C - reserved
    dw int_debug        ; 1D - reserved
    dw int_debug        ; 1E - reserved
    dw int_debug        ; 1F - reserved


;======================================================================
; reset code (called on CPU reset)
;======================================================================

setloc 0FFF0h

reset:
    jmp cseg:start


setloc 0FFF5h
    db BUILD

setloc 0FFFEh
    db VERSION
    db 0FFh

