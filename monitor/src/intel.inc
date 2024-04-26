;======================================================================
; Ferry Hendrikx, April 2024
;======================================================================

;======================================================================
; read intel hex file records until we get an EOF record
; the decoded file is written to ES (DI loaded from file)
;======================================================================

read_hex_file:
    push ax
    push bx
    push cx
    push di
    push es
    xor di,di

.find_record:
    call read_chr
    cmp al,':'          ; search for ':'
    jnz .find_record

    call read_hex       ; get length
    mov cl,al           ; rec_data uses cl for counter

    call read_hex       ; get address
    mov bh,al
    call read_hex
    mov bl,al
    mov di,bx           ; rec_data uses ES:DI in stosb

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
    call .checksum
    jmp .find_record

.rec_segment:
    call read_hex       ; get segment
    call read_hex       ; get segment
    call .checksum
    jmp .find_record

.rec_start:
    call read_hex       ; get CS
    call read_hex       ; get CS
    call read_hex       ; get IP
    call read_hex       ; get IP
    call .checksum
    jmp .find_record

.checksum:
    call read_hex	; read the checksum
    ;call read_hex	; read the new line
    ret
    
.exit:
    call .checksum	; read the trailing checksum
    pop es
    pop di
    pop cx
    pop bx
    pop ax
    ret

