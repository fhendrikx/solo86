
;----------------------------------------------------------------------
; memory dump; dump memory content at DS:SI for CX bytes
;----------------------------------------------------------------------

mem_dump:
    push ax
    push bx
    push cx
    push si
    push ds

.loop1:

    ; print the memory address SI
    mov ax,si
    mov al,ah
    call print_hex
    mov ax,si
    call print_hex
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
    cmp al,32		; space
    jb .print_dot

    cmp al,126		; last printable char
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
    pop ds
    pop si
    pop cx
    pop bx
    pop ax
    ret

;----------------------------------------------------------------------
; memory copy; copy cx *words* from ds:si to es:di
;----------------------------------------------------------------------

mem_copy_w:
    movsw
    loop mem_copy_w
    ret
    
