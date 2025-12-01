;======================================================================
; Ferry Hendrikx, April 2024
;======================================================================

;=========================================================================
; print_chr
; Input:
;   AL - char
; Notes:
;   Prints character
;-------------------------------------------------------------------------
print_chr:
    out COM_DATA,al
    push ax

.wait_for_empty:
    in al, 06Ah
    and al, 020h ; check if transmit buffer is empty
    jz .wait_for_empty

    pop ax
    out 060h, al
    ret


;=========================================================================
; print_hex
; Input:
;   AL - char
; Notes:
;   Prints 2 hex characters
;-------------------------------------------------------------------------
print_hex:
    push ax
    push ax
    shr al,4
    call .nibble

    pop ax
    and al,0fh
    call .nibble
    pop ax
    ret

.nibble:
    cmp al,9
    jg .letter
    add al,30h              ; '0'
    call print_chr
    ret

.letter:
    add al,37h              ; 'A' - 10
    call print_chr
    ret


;=========================================================================
; print_word
; Input:
;   AX - word
; Notes:
;   Prints 4 hex characters
;-------------------------------------------------------------------------
print_word:
    push ax
    push ax
    mov al,ah
    call print_hex
    pop ax
    call print_hex
    pop ax
    ret


;=========================================================================
; print_str
; Input:
;   SI - offset
; Notes:
;   Prints ASCIIZ string
;-------------------------------------------------------------------------
print_str:
    push ax
    push si
    push ds
    cld

    push cs                 ; CS => DS
    pop ds

.loop:
    lodsb
    cmp al,0                ; are we done?
    jz .exit

    call print_chr
    jmp .loop

.exit:
    pop ds
    pop si
    pop ax
    ret


;=========================================================================
; raw_ready
; Input:
;   None
; Output:
;   AL - boolean
; Notes:
;   Any bytes waiting?
;-------------------------------------------------------------------------
raw_ready:
    in al,COM_CTRL          ; any bytes?
    ret


;=========================================================================
; raw_chr
; Input:
;   None
; Output:
;   AL - char
; Notes:
;   Blocking read of 1 character
;-------------------------------------------------------------------------
raw_chr:
    in al,COM_CTRL          ; any characters?
    cmp al,0
    jz raw_chr
    in al,COM_DATA          ; read character
    ret


;=========================================================================
; raw_hex
; Input:
;   None
; Output:
;   AL - char
; Notes:
;   Blocking read of 2 hex characters
;-------------------------------------------------------------------------
raw_hex:
    call .nibble
    mov ah,al
    shl ah,4

    call .nibble
    or al,ah
    ret

.nibble:
    in al,COM_CTRL          ; any characters?
    cmp al,0
    jz .nibble

    in al,COM_DATA          ; read character
    cmp al,41h
    jae .letter
    sub al,30h              ; '0'
    ret

.letter:
    sub al,37h              ; 'A' - 10
    ret


;=========================================================================
; raw_str
; Input:
;   ES - segment
;   DI - offset
; Notes:
;   Blocking read of ASCIIZ string
;-------------------------------------------------------------------------
raw_str:
    push ax
    push di
    push es
    cld

.loop:
    in al,COM_CTRL          ; any characters?
    cmp al,0
    jz .loop

    in al,COM_DATA          ; read character
    stosb
    cmp al,0Ah              ; are we done?
    jne .loop

.exit:
    mov dx,di               ; return length in dx
    mov al,0                ; terminator
    stosb
    pop es
    pop di
    pop ax
    ret


;======================================================================
; read with echo
;======================================================================

;=========================================================================
; ready_ready
; Input:
;   None
; Output:
;   AL - boolean
; Notes:
;   Any bytes waiting?
;-------------------------------------------------------------------------
read_ready:
    in al,COM_CTRL          ; any bytes?
    ret


;=========================================================================
; read_chr
; Input:
;   None
; Output:
;   AL - char
; Notes:
;   Blocking read of 1 character
;-------------------------------------------------------------------------
read_chr:
    call raw_chr
    call print_chr
    ret


;=========================================================================
; read_hex
; Input:
;   None
; Output:
;   AL - char
; Notes:
;   Blocking read of 2 hex characters
;-------------------------------------------------------------------------
read_hex:
    call raw_hex
    call print_chr
    ret


;=========================================================================
; read_str
; Input:
;   ES - buffer segment
;   DI - buffer offset
; Notes:
;   Blocking read of ASCIIZ string
;-------------------------------------------------------------------------
read_str:
    push ax
    push cx
    push di
    push es
    xor cx,cx
    cld

.loop:
    in al,COM_CTRL          ; any characters?
    cmp al,0
    jz .loop

    in al,COM_DATA          ; read character
    cmp al,00h
    je  .loop
    cmp al,0Dh              ; telnet
    je  .cr
    cmp al,7fh
    je  .bs

.next:
    inc cl
    stosb
    call print_chr

    cmp al,0Ah              ; are we done?
    je  .exit

    jmp .loop

.bs:
    cmp cl,0                ; any characters to delete?
    jz .loop
    dec cl
    dec di

    mov al,08h
    call print_chr          ; print DEL
    mov al,20h
    call print_chr          ; over-write with space
    mov al,08h
    call print_chr          ; print DEL
    jmp .loop

.cr:
    mov al,0Ah              ; convert CR to LF
    jmp .next

.exit:
    mov dx,di               ; return length in dx
    mov al,0                ; terminator
    stosb

    pop es
    pop di
    pop cx
    pop ax
    ret

