
;======================================================================
; debug menu
;======================================================================

debug_menu:
    print dbg_welcome

; main loop
.loop:
    print dbg_prompt

    push cs
    pop ax
    mov ds,ax
    mov es,ax

    mov di,buffer
    call read_str           ; ES:DI

    mov si,buffer           ; DS:SI
    lodsb

    cmp al,'D'
    je  cmd_dasm
    cmp al,'F'
    je  cmd_fill
    cmp al,'H'
    je  cmd_help
    cmp al,'I'
    je  cmd_inp
    cmp al,'J'
    je  cmd_jump
    cmp al,'L'
    je  cmd_load
    cmp al,'O'
    je  cmd_out
    cmp al,'Q'
    je .exit
    cmp al,'R'
    je  cmd_read
    cmp al,'X'
    je  cmd_exe

    jmp .loop

.exit:
    ret


; disassemble
cmd_dasm:
    call spaces

    call inp_word           ; seg
    mov bx,ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov di,ax

    push ds
    mov ds,bx

    call dasm_inst

    pop ds

    jmp debug_menu.loop


; fill memory
cmd_fill:
    call spaces

    call inp_word           ; seg
    mov bx,ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov si,ax

    call spaces

    call inp_hex            ; count
    xor cx,cx
    mov cl,al

    call inp_hex            ; byte

    print dbg_cmd_fill

    push es
    mov es,bx

    call mem_fill

    pop es

    jmp debug_menu.loop


; help
cmd_help:
    print dbg_cmd_help

    jmp debug_menu.loop


; inp pp
cmd_inp:
    call spaces

    call inp_hex            ; port
    xor dx,dx
    mov dl,al

    print dbg_cmd_inp

    in al,dx                ; INP
    call print_hex

    jmp debug_menu.loop


; jump
cmd_jump:
    call spaces

    call inp_word           ; seg
    mov [ hex_seg ],ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov [ hex_ofs ],ax

    print dbg_cmd_jump

; indirect far jump to CS:IP
    jmp far [ hex_ofs ]


; load intel hex file
cmd_load:
    print dbg_cmd_load

    call read_hex_file

    print dbg_cmd_load_done

    jmp debug_menu.loop


; out pp xx
cmd_out:
    call spaces

    call inp_hex            ; port
    mov dl,al

    call spaces

    call inp_hex            ; byte

    print dbg_cmd_out

    out dx,al               ; OUT

    jmp debug_menu.loop


; read memory
cmd_read:
    call spaces

    call inp_word           ; seg
    mov bx,ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov si,ax

    print dbg_cmd_dump

    push ds
    mov ds,bx

    mov cx,256
    call mem_dump

    pop ds

    jmp debug_menu.loop


; execute
cmd_exe:
    print dbg_cmd_exe

; simulate cold start
    xor ax,ax
    mov ds,ax
    mov es,ax
    mov ss,ax

    mov ax,2h
    push ax
    popf
; indirect far jump to CS:IP
    jmp far [ hex_ofs ]


; syntax error
syntax:
    print dbg_syntax

    jmp debug_menu.loop


;======================================================================
; functions
;======================================================================

; skip over any spaces
spaces:
    lodsb
    cmp al,20h
    je spaces           ; keep going if we had a space
    dec si              ; undo the last load
    ret


; expect character AL
expect:
    mov bl,al
    lodsb
    cmp al,bl           ; is it what we expected?
    ret                 ; flag not changed on ret


; read 2 hex characters into AL from ES:DI
inp_hex:
    call .nibble
    mov ah,al
    shl ah,1            ; shl al,4 not possible
    shl ah,1
    shl ah,1
    shl ah,1
    call .nibble
    or al,ah
    ret

.nibble:
    lodsb               ; read char
    cmp al,41h
    jae .letter
    sub al,30h          ; '0'
    ret

.letter:
    sub al,37h          ; 'A' - 10
    ret


; read 4 hex characters into AX from ES:DI
inp_word:
    push bx
    call inp_hex
    mov bh,al
    call inp_hex
    mov bl,al
    mov ax,bx
    pop bx
    ret


;======================================================================
; data
;======================================================================

buffer      times 32    db 000h


