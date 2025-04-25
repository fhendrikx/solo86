cpu 8086

org 0

;======================================================================
; ROM signature
;======================================================================

                    db 055h
                    db 0AAh
                    dw init
                    dw 01h
                    dw 1000h
                    dw 03h
                    dw 00h
                    dw 00h
                    dw 00h
                    db 'ELKS Loader', 00h

TIMES 256-($-$$)    db 0FFh


%include "../monitor/const.inc"

init:

    ; relocate to RAM before updating banking table
    cld
    mov ax,cs
    mov ds,ax
    xor si,si

    mov ax,01000h
    mov es,ax
    xor di,di

    ; this code occupies 16k in ROM (at most)
    mov cx,02000h

.copy:
    movsw
    loop .copy

    jmp 01000h:relocate

relocate:

    ; setup memory banking

    ; map the ROMFS image into 0x8000->0xD000 (3 x 128k banks)
    mov al,04h
    out BNK_ROW_0,al
    mov al,05h
    out BNK_ROW_1,al
    mov al,06h
    out BNK_ROW_2,al

    ; map the kernel image into 0xE000->0xF000
    mov al,07h
    out BNK_ROW_3,al

    jmp 0E000h:0014h

