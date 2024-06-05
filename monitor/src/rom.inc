;======================================================================
; Ferry Hendrikx, June 2024
;======================================================================

;======================================================================
; scan ROM
;======================================================================

scan_rom:
    push bx
    push cx
    push di
    push ds
    push es
    push si

    mov bx,08000h       ; start at 0x80000
    mov ds,bx
    mov si,0

    push cs
    pop es
    mov di,rom_list

.scan:
    lodsw               ; DS:SI
    cmp ax, 0AA55h      ; ROM signature found?
    jne .next
    mov cx,30/2         ; copy 30 bytes

.copy:
    lodsw               ; DS:SI
    stosw               ; ES:DI
    cmp cx,0
    jnz .copy

.next:
    add bx,400h         ; next 16K block
    mov ds,bx
    mov si,0
    cmp bx,0F000h       ; are we finished?
    jb .scan

.exit:
    pop si
    pop es
    pop ds
    pop di
    pop cx
    pop bx
    ret

