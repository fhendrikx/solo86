
;----------------------------------------------------------------------
; memory dump; dump memory content at CS:SI for CX bytes
;----------------------------------------------------------------------

mem_dump:
    push ax
    push bx
    push cx
    push si
    push ds
    push cs
    pop ds
    mov bl,16

.loop:
    lodsb
    call print_hex
    ;mov al,' '
    ;call print_chr
    dec bl
    jnz .next

    mov al,0ah
    call print_chr
    mov bl,16

.next:
    dec cx
    jz .exit
    jmp .loop

.exit:
    pop ds
    pop si
    pop cx
    pop bx
    pop ax
    ret

