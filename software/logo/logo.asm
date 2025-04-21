cpu 8086

org 0

;======================================================================
; ROM signature
;======================================================================

                    db 055h
                    db 0AAh
                    dw init
                    dw 01h
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
    mov al,11h              ; mode 1 with auto increment on write
    out VC_CTRL,al

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

    ; set the video mode
    mov al,12h              ; mode 2 with auto increment on write
    out VC_CTRL,al

    ; bh == Y coord
    ; bl == X coord

    xor bx,bx

.loop:
    mov al,bh
    out VC_HIGH_ADDR,al
    inc bh

    mov al,bl
    out VC_LOW_ADDR,al
    inc bl
    inc bl

    mov al,0Fh
    out VC_DATA,al

.wait_for_vsync:
    in al,VC_CTRL
    test al,80h
    jz .wait_for_vsync

    ; swap buffers by writing video mode again
    out VC_CTRL,al

    jmp .loop

    hlt


%include "../monitor/delay.inc"
%include "sound.inc"
%include "xwing.inc"

