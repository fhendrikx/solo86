
;======================================================================
; memory read; read memory content at DS:SI for CX bytes
;======================================================================

mem_read:
    push ax
    push bx
    push cx
    push ds
    push si

.loop1:

; print seg:offset
    mov ax,ds
    call print_word     ; seg

    mov al,':'
    call print_chr

    mov ax,si
    call print_word     ; offset

    mov al,':'
    call print_chr

    push si
    mov bl,16

.loop2:
    ; dump the memory content in hex
    mov al,' '
    call print_chr

    lodsb
    call print_hex
    dec bl
    jnz .loop2

    mov al,' '
    call print_chr

    pop si
    mov bl,16

.loop3:
    ; dump the memory content in ascii
    lodsb
    cmp al,32       ; space
    jb .print_dot

    cmp al,126      ; last printable char
    ja .print_dot

    jmp .print_ascii

.print_dot:
    mov al,'.'

.print_ascii:
    call print_chr
    dec bl
    jnz .loop3

    mov al,0ah
    call print_chr

    sub cx,16
    ja .loop1

.exit:
    pop si
    pop ds
    pop cx
    pop bx
    pop ax
    ret


;----------------------------------------------------------------------
; memory fill; fill memory content at ES:DI for CX bytes with AL
;----------------------------------------------------------------------

mem_fill:
    push ax
    push bx
    push cx
    push di
    push es

; fill data to ES:DI, keep going until CL is zero
.loop:
    stosb
    dec cl
    jnz .loop

.exit:
    pop es
    pop di
    pop cx
    pop bx
    pop ax
    ret

