cpu 8086

org 0

;======================================================================
; ROM signature
;======================================================================

                    db 055h
                    db 0AAh
                    dw init
                    dw 01h
                    dw 00h
                    db '16 bit I/O test         ', 00h

TIMES 256-($-$$)    db 0FFh




; I/O addresses should be even numbers only
leds_data       equ 08h

init:
; initialise SS/SP
    xor ax,ax
    mov ss,ax
    mov sp,0FFFFh

flash:

    mov ax,1000
    call delay_ms

    mov ax,0FFFFh
    mov dx,leds_data
    out dx,ax

    mov ax,1000
    call delay_ms

    xor ax,ax
    mov dx,leds_data
    out dx,ax

    jmp flash


    
%include "delay.inc"

