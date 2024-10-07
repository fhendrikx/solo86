
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


    ; repeat last command?

    cmp al,0Ah
    je  cmd_last


    ; last command

    mov byte [cs:dbg_last],al


    ; which command?

    cmp al,'D'
    je  cmd_mem_dasm
    cmp al,'S'
    je  cmd_mem_dasm_style
    cmp al,'F'
    je  cmd_mem_fill
    cmp al,'R'
    je  cmd_mem_read

    cmp al,'I'
    je  cmd_inp
    cmp al,'O'
    je  cmd_out
    cmp al,'J'
    je  cmd_jmp

    cmp al,'L'
    je  cmd_ihf_load
    cmp al,'E'
    je  cmd_ihf_exec

    cmp al,'H'
    je  cmd_help

    cmp al,'Q'
    je .exit

    jmp .loop

.exit:
    ret


cmd_last:
    mov al,byte [cs:dbg_last]

    cmp al,'D'
    je .dasm
    cmp al,'R'
    je .read

    jmp debug_menu.loop

.dasm:
    push ds

    ; get last address
    mov bx,[cs:dbg_ofs]
    mov si,bx
    mov bx,[cs:dbg_seg]
    mov ds,bx

    mov cx,16

    call mem_dasm

    ; store last address
    mov [cs:dbg_ofs],bx
    mov bx,ds
    mov [cs:dbg_seg],bx

    pop ds

    jmp debug_menu.loop

.read:
    push ds

    ; get last address
    mov bx,[cs:dbg_ofs]
    mov si,bx
    mov bx,[cs:dbg_seg]
    mov ds,bx

    mov cx,256

    call mem_read

    ; store last address
    mov [cs:dbg_ofs],bx
    mov bx,ds
    mov [cs:dbg_seg],bx

    pop ds

    jmp debug_menu.loop


cmd_help:
    print dbg_cmd_help
    jmp debug_menu.loop


cmd_mem_dasm:
    call spaces

    call inp_word           ; seg
    mov bx,ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov si,ax

    push ds
    mov ds,bx
    mov cx,16

    call mem_dasm

    ; store last address
    mov [cs:dbg_ofs],bx
    mov bx,ds
    mov [cs:dbg_seg],bx

    pop ds

    jmp debug_menu.loop


cmd_mem_dasm_style:
    mov al,byte [cs:dasm_style]
    not al
    mov byte [cs:dasm_style],al

    cmp al,0
    jne .attstyle

    print dbg_cmd_dasm_intel
    jmp debug_menu.loop

.attstyle:
    print dbg_cmd_dasm_att
    jmp debug_menu.loop


cmd_mem_fill:
    call spaces

    call inp_word           ; seg
    mov bx,ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov di,ax

    call spaces

    call inp_hex            ; count
    xor cx,cx
    mov cl,al

    call spaces

    call inp_hex            ; byte

    print dbg_cmd_fill

    push es
    mov es,bx
    call mem_fill
    pop es

    jmp debug_menu.loop


cmd_mem_read:
    call spaces

    call inp_word           ; seg
    mov bx,ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov si,ax

    print dbg_cmd_read

    push ds
    mov cx,256
    mov ds,bx

    call mem_read

    ; store last address
    mov [cs:dbg_ofs],bx
    mov bx,ds
    mov [cs:dbg_seg],bx

    pop ds

    jmp debug_menu.loop


cmd_inp:
    call spaces

    call inp_hex            ; port
    xor dx,dx
    mov dl,al

    print dbg_cmd_inp

    in al,dx                ; INP
    call print_hex

    jmp debug_menu.loop


cmd_out:
    call spaces

    call inp_hex            ; port
    mov dl,al

    call spaces

    call inp_hex            ; byte

    print dbg_cmd_out

    out dx,al               ; OUT

    jmp debug_menu.loop


cmd_jmp:
    call spaces

    call inp_word           ; seg
    mov [cs:jmp_seg],ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov [cs:jmp_ofs],ax

    print dbg_cmd_jump

    ; indirect far jump to CS:IP
    jmp far [cs:jmp_ofs]


cmd_ihf_load:
    print dbg_cmd_load

    call ihf_load

    print dbg_cmd_load_loc  ; seg
    mov ax,[cs:ihf_loc]
    call print_word

    print dbg_cmd_load_jmp  ; seg:offset
    mov ax,[cs:ihf_seg]
    call print_word

    mov al,':'
    call print_chr

    mov ax,[cs:ihf_ofs]
    call print_word

    mov al,0Ah
    call print_chr

    jmp debug_menu.loop


cmd_ihf_exec:
    ; sanity check
    mov ax,[cs:ihf_seg]
    cmp ax,0
    je .exit

    print dbg_cmd_exec

    ; simulate cold start
    xor ax,ax
    mov ds,ax
    mov es,ax
    mov ss,ax

    mov ax,2h
    push ax
    popf

    ; indirect far jump to CS:IP
    jmp far [ ihf_ofs ]

.exit:
    print dbg_cmd_exec_err
    jmp debug_menu.loop

syntax:
    print dbg_syntax
    jmp debug_menu.loop


;======================================================================
; functions
;======================================================================

; skip over any spaces
;
spaces:
    lodsb
    cmp al,20h
    je spaces               ; keep going if we had a space
    dec si                  ; undo the last load
    ret


; expect character AL
;
expect:
    push bx
    mov bl,al
    lodsb
    cmp al,bl               ; is it what we expected?
    pop bx
    ret                     ; flag not changed on ret


; read 2 hex characters into AL from ES:DI
;
inp_hex:
    call .nibble
    mov ah,al
    shl ah,1                ; shl al,4 not possible
    shl ah,1
    shl ah,1
    shl ah,1
    call .nibble
    or al,ah
    ret

.nibble:
    lodsb                   ; read char
    cmp al,41h
    jae .letter
    sub al,30h              ; '0'
    ret

.letter:
    sub al,37h              ; 'A' - 10
    ret


; read 4 hex characters into AX from ES:DI
;
inp_word:
    push bx
    call inp_hex
    mov bh,al
    call inp_hex
    mov bl,al
    mov ax,bx
    pop bx
    ret

