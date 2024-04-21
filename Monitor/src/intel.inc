;======================================================================
; Ferry Hendrikx, April 2024
;======================================================================

;======================================================================
; read intel hex file records until we get an EOF record
; the decoded file is written to ES:DI
;======================================================================

read_hex_file:
    push ax
    push bx
    push di
    push es

.find_record:
    call read_chr
    cmp al,':'          ; search for ':'
    jnz .find_record

    call read_hex       ; get length
    mov cl,al
    call read_hex       ; eat address
    call read_hex

    call read_hex       ; get type
    cmp al,0            ; 0x00 - data
    je .rec_data
    cmp al,1            ; 0x01 - EOF
    je .exit
    cmp al,2            ; 0x02 - segment
    je .rec_segment
    cmp al,3            ; 0x03 - CS:IP
    je .rec_start
    jmp .find_record

.rec_data:
    call read_hex       ; get data (1 byte)
    stosb
    dec cl
    jnz .rec_data       ; are we done yet?
    jmp .find_record

.rec_segment:
    call read_hex       ; get segment
    mov bh,al
    call read_hex       ; get segment
    mov bl,al
    jmp .find_record

.rec_start:
    call read_hex       ; get CS
    mov bh,al
    call read_hex       ; get CS
    mov bl,al
    call read_hex       ; get IP
    mov bh,al
    call read_hex       ; get IP
    mov bl,al
    jmp .find_record

.exit:
    pop es
    pop di
    pop bx
    pop ax
    ret

