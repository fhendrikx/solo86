;======================================================================
; Ferry Hendrikx, April 2024
;======================================================================

;======================================================================
; read intel hex file records until we get an EOF record
;
; the decoded file is written to SEGMENT::OFFSET, where the SEGMENT is
; derived from record 0x02, and OFFSET is derived from each record 0x01
;======================================================================

read_hex_file:
    push ax
    push bx
    push cx
    push dx
    push di
    push es

; defaults for ES:DI (in case there is no 0x02 record present)
    mov ax,cseg
    mov es,ax
    xor di,di

    xor dx,dx               ; no echo please

.find_record:
    call read_chr
    cmp al,':'              ; search for ':'
    jnz .find_record

    call read_hex           ; get length
    mov cl,al               ; rec_data uses CL for counter

    call read_hex           ; get address
    mov bh,al
    call read_hex
    mov bl,al
    mov di,bx               ; rec_data uses ES:DI in stosb

    call read_hex           ; get type
    cmp al,0                ; 0x00 - Data
    je .rec_data
    cmp al,1                ; 0x01 - EOF
    je .rec_eof
    cmp al,2                ; 0x02 - Segment
    je .rec_segment
    cmp al,3                ; 0x03 - CS:IP
    je .rec_start
    jmp .find_record

; record type 0x00 - Data
; read data to ES:DI, keep going until CL is zero
.rec_data:
    call read_hex           ; get data (1 byte)
    stosb
    dec cl
    jnz .rec_data           ; are we done yet?
    call read_hex           ; get checksum (1 byte)
    jmp .find_record

; record type 0x01 - EOF
; the end of the road
.rec_eof:
    call read_hex           ; get checksum (1 byte)
    jmp .clean

; record type 0x02 - Segment
; read segment into ES
.rec_segment:
    call read_hex           ; get segment
    mov bh,al
    call read_hex           ; get segment
    mov bl,al
    mov es,bx               ; rec_data uses ES:DI in stosb

    call read_hex           ; get checksum (1 byte)
    jmp .find_record

; record type 0x03 - CS:IP
; read CS:IP into start_segment/start_address
.rec_start:
    call read_hex           ; get CS
    mov bh,al
    call read_hex           ; get CS
    mov bl,al
    mov [ start_segment ], bx

    call read_hex           ; get IP
    mov bh,al
    call read_hex           ; get IP
    mov bl,al
    mov [ start_address ], bx

    call read_hex           ; get checksum (1 byte)
    jmp .find_record

.clean:
; clean up any remaining chars in the UART
    call read_ready
    jz .exit
    call read_chr

.exit:
    pop es
    pop di
    pop dx
    pop cx
    pop bx
    pop ax
    ret

