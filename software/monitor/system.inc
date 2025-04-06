
sys_init:
    push ax
    push bx
    push dx

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

