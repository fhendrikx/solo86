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
                    db 'Display demo            ', 00h

TIMES 256-($-$$)    db 0FFh


%include "../monitor/const.inc"

init:
    ; initialise SS/SP
    xor ax,ax
    mov ss,ax
    mov sp,0FFFFh

    ; set the video mode
    mov al,1
    out VC_MODE,al

    xor al,al
    out VC_HIGH_ADDR,al
    out VC_LOW_ADDR,al

    cld
    mov ax,cs
    mov ds,ax
    mov si,logo
    mov cx,49152

.load_image:
    lodsb
    out VC_DATA,al
    loop .load_image

    ; setup tune pointer
    mov si,imperial
    call tune

    hlt


%include "../monitor/delay.inc"
%include "sound.inc"
%include "xwing.inc"

