
;======================================================================
; debug menu
;======================================================================

; prompt
prompt:
    print mesg_prompt

    call read_chr
    cmp al,'D'
    je  menu_dump
    cmp al,'F'
    je  menu_fill
    cmp al,'H'
    je  menu_help
    cmp al,'I'
    je  menu_inp
    cmp al,'J'
    je  menu_jump
    cmp al,'L'
    je  menu_load
    cmp al,'O'
    je  menu_out
    cmp al,'X'
    je  menu_exe
    jmp prompt

menu_dump:
; dump memory

    push bx
    push cx
    push ds
    push si

    call read_chr           ; space

    call read_hex           ; get segment
    mov bh,al
    call read_hex           ; get segment
    mov bl,al
    mov ds,bx

    call read_chr           ; colon

    call read_hex           ; get offset
    mov bh,al
    call read_hex           ; get offset
    mov bl,al
    mov si,bx

    print mesg_dump

    mov cx,256
    call mem_dump

    pop si
    pop ds
    pop cx
    pop bx
    jmp prompt

menu_fill:
; fill memory

    push ax
    push bx
    push cx
    push di
    push es

    call read_chr           ; space

    call read_hex           ; get segment
    mov bh,al
    call read_hex           ; get segment
    mov bl,al
    mov es,bx

    call read_chr           ; colon

    call read_hex           ; get offset
    mov bh,al
    call read_hex           ; get offset
    mov bl,al
    mov di,bx

    call read_chr           ; space

    call read_hex           ; get count
    xor cx,cx
    mov cl,al

    call read_chr           ; space

    call read_hex           ; get hex byte

    print mesg_fill

    call mem_fill

    pop es
    pop di
    pop cx
    pop bx
    pop ax
    jmp prompt

menu_help:
; help
    print mesg_help
    jmp prompt

menu_inp:
; inp pp

    push dx                 ; save echo state

    call read_chr           ; space
    call read_hex           ; get port
    xor dx,dx
    mov dl,al

    print mesg_inp

    in al,dx                ; INP
    call print_hex

    pop dx
    jmp prompt

menu_jump:
; jump

    call read_chr           ; space

    call read_hex           ; get segment
    mov bh,al
    call read_hex           ; get segment
    mov bl,al
    mov [ hex_seg ], bx

    call read_chr           ; colon

    call read_hex           ; get offset
    mov bh,al
    call read_hex           ; get offset
    mov bl,al
    mov [ hex_ofs ], bx

    print mesg_jump

; indirect far jump to CS:IP
    jmp far [ hex_ofs ]

menu_load:
; load intel hex file
    print mesg_load

    call read_hex_file

    print mesg_load_done
    jmp prompt

menu_out:
; out pp xx

    push dx                 ; save echo state

    call read_chr           ; space

    call read_hex           ; get port
    mov bl,al

    call read_chr           ; space
    call read_hex           ; get hex byte

    xor dx,dx
    mov dl,bl

    print mesg_out

    out dx,al               ; OUT

    pop dx
    jmp prompt

menu_exe:
; execute
    print mesg_exe

    xor ax,ax
    mov ds,ax
    mov es,ax
    mov ss,ax

    mov ax,2h
    push ax
    popf
; indirect far jump to CS:IP
    jmp far [ hex_ofs ]

