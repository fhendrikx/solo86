
sys_init:
    push ax
    push bx
    push dx

    ; memory banking

    ; map ROM into the top half of memory, skip monitor
    mov al,1
    out BNK_ROW_0,al
    mov al,2
    out BNK_ROW_1,al
    mov al,3
    out BNK_ROW_2,al
    mov al,4
    out BNK_ROW_3,al
    mov al,5
    out BNK_ROW_4,al
    mov al,6
    out BNK_ROW_5,al
    mov al,7
    out BNK_ROW_6,al

    ; map RAM into the last segment so that hex loading still works
    mov al,01Fh
    out BNK_ROW_7,al

    ; detect hardware

    xor ax,ax
    or  ax,BNK_PRESENT
    mov [ cs:hardware ],ax

    mov bx,PIT_PRESENT
    or  bx,RTC_PRESENT
    mov dx,PIT_DETECT
    call hw_detect

    mov bx,COM_PRESENT
    mov dx,COM_DETECT
    call hw_detect

    mov bx,PNL_PRESENT
    mov dx,PNL_DETECT
    call hw_detect

    pop dx
    pop bx
    pop ax
    ret

hw_detect:
    push dx

    in al,dx
    cmp al,0AAh             ; look for 0xAA
    je .found
    and al,0F8h
    cmp al,0A8h             ; look for 0xA8 (for the epoch card)
    je .found
    jmp .end

.found:
    mov ax,[ cs:hardware ]
    or ax,bx
    mov [ cs:hardware ],ax

.end:
    pop dx
    ret

