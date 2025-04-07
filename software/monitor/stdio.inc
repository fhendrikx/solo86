;======================================================================
; Ferry Hendrikx, April 2024
;======================================================================

;=========================================================================
; stdio_init
;=========================================================================
stdio_init:
    ; initialise COM

    push ax

    ; disable interrupts
    xor al,al
    out COM_CTRL,al

    ; set console video mode
    out VC_MODE,al

.exit:
    pop ax
    ret


;=========================================================================
; print_chr
; Input:
;   AL - char
; Notes:
;   Prints character
;-------------------------------------------------------------------------
print_chr:
    out COM_DATA,al
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
    shr al,1                ; shr al,4 not possible
    shr al,1
    shr al,1
    shr al,1
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
    out COM_DATA,al
    ret

.letter:
    add al,37h              ; 'A' - 10
    out COM_DATA,al
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

    out COM_DATA,al
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
    shl ah,1                ; shl al,4 not possible
    shl ah,1
    shl ah,1
    shl ah,1

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
    out COM_DATA,al
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
    out COM_DATA,al
    ret


;=========================================================================
; read_str
; Input:
;   ES - segment
;   DI - offset
; Notes:
;   Blocking read of ASCIIZ string
;-------------------------------------------------------------------------
read_str:
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
    out COM_DATA,al

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

