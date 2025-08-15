cpu 8086

org 0

;======================================================================
; ROM signature
;======================================================================

                    db 055h
                    db 0AAh
                    dw init
                    dw 04h
                    dw 0000h
                    dw 01h
                    dw 00h
                    dw 00h
                    dw 00h
                    db 'Display demo', 00h

TIMES 256-($-$$)    db 0FFh


%include "../monitor/const.inc"

init:
    ; initialise SS/SP
    xor ax,ax
    mov ss,ax
    mov sp,0FFFFh

    ; set the video mode
    mov al,40h              ; 256x192
    out VC_CTRL,al

    mov al,01h              ; graphics mode
    out VC_CTRL,al

    cld
    mov ax,cs
    mov ds,ax
    mov si,logo
    mov cx,49152

load_image:
    lodsb
    out VC_DATA,al
    loop load_image

    ; setup tune pointer
    ;mov si,imperial
    ;call tune


    mov dl, 0
    xor bx,bx


colour:

    ; bh == X coord
    ; bl == Y coord
    mov cx,256

wibble:

    mov al,bh
    out VC_PARAM,al
    inc bh

    mov al,bl
    out VC_PARAM,al
    inc bl

    mov al,dl
    out VC_PARAM,al

    mov al, 060h            ; SetPixel
    out VC_CTRL, al

    loop wibble

wait_for_idle:
    in al, VC_CTRL
    test al,80h
    jnz wait_for_idle

    inc dl
    inc bh

    jmp colour


    hlt


%include "../monitor/delay.inc"
%include "sound.inc"
%include "xwing.inc"

