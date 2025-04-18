;======================================================================
; Ferry Hendrikx, April 2024
;======================================================================

;======================================================================
; read intel hex file records until we get an EOF record
;
; the decoded file is written to SEGMENT::OFFSET, where the SEGMENT is
; derived from record 0x02, and OFFSET is derived from each record 0x01
;
; if a second record 0x02 appears in the record stream, then the decoded
; file will write to that location. This is normally used to ensure that
; files longer than 0xFFFF bytes can be correctly loaded.
;
; please note that ihx_seg:ihx_ofs may be different to ihx_loc. This is
; often when ihx_seg is a different segment than ihx_loc, for example,
; when loading a monitor at E000 and starting the code at FFFF:0000.
;======================================================================

;=========================================================================
; ihx_load
; Input:
;   None
; Output:
;   None
;-------------------------------------------------------------------------
ihx_load:
    push ax
    push bx
    push cx
    push dx
    push di
    push es

    ; defaults
    xor ax,ax
    mov [cs:ihx_seg],ax
    mov [cs:ihx_ofs],ax

.find_record:
    call raw_chr
    cmp al,':'              ; search for ':'
    jnz .find_record

    call raw_hex            ; get length
    mov cl,al               ; rec_data uses CL for counter

    call raw_hex            ; get address
    mov bh,al
    call raw_hex
    mov bl,al
    mov di,bx               ; rec_data uses (ES:)DI in stosb

    call raw_hex            ; get type
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
    call raw_hex            ; get data (1 byte)
    stosb
    dec cl
    jnz .rec_data           ; are we done yet?
    call raw_hex            ; get checksum (1 byte)
    jmp .find_record

    ; record type 0x01 - EOF
    ; the end of the road
.rec_eof:
    call raw_hex            ; get checksum (1 byte)
    jmp .clean

    ; record type 0x02 - Segment (may appear more than once when file > 64KB)
    ; read segment into ihx_loc and ES(:DI)
.rec_segment:
    call raw_hex            ; get segment
    mov bh,al
    call raw_hex            ; get segment
    mov bl,al
    mov [cs:ihx_loc],bx
    mov es,bx               ; rec_data uses ES:DI in stosb

    call raw_hex            ; get checksum (1 byte)
    jmp .find_record

    ; record type 0x03 - CS:IP
    ; read CS:IP into ihx_seg:ihx_ofs
.rec_start:
    call raw_hex            ; get CS
    mov bh,al
    call raw_hex            ; get CS
    mov bl,al
    mov [cs:ihx_seg],bx

    call raw_hex            ; get IP
    mov bh,al
    call raw_hex            ; get IP
    mov bl,al
    mov [cs:ihx_ofs],bx

    call raw_hex            ; get checksum (1 byte)
    jmp .find_record

    ; clean up any remaining chars in the COM
.clean:
    call raw_ready
    jz .exit
    call raw_chr

.exit:
    pop es
    pop di
    pop dx
    pop cx
    pop bx
    pop ax
    ret

