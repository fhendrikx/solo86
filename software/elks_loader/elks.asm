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

    ; map the ROMFS image into 0x8000->0xC000 (5 banks)
    mov al,0Ah
    out BNK_ROW_0,al
    mov al,0Bh
    out BNK_ROW_1,al
    mov al,0Ch
    out BNK_ROW_2,al
    mov al,0Dh
    out BNK_ROW_3,al
    mov al,0Eh
    out BNK_ROW_4,al

    ; map RAM into the spare bank 0xD000
    mov al,1Dh
    out BNK_ROW_5,al

    ; map the kernel image into 0xE000
    mov al,0Fh
    out BNK_ROW_6,al

    ; map RAM into the spare bank 0xF000
    mov al,01Fh
    out BNK_ROW_7,al

    jmp 0E000h:0014h

