;======================================================================
; Ferry Hendrikx, April 2024
;======================================================================

;======================================================================
; print
;======================================================================

;======================================================================
; print a char in AL to the UART
;======================================================================

print_chr:
putc:
    out UART_DATA,al
    ret


;======================================================================
; print AL as 2 hex characters to the UART
;======================================================================

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
    out UART_DATA,al
    ret

.letter:
    add al,37h              ; 'A' - 10
    out UART_DATA,al
    ret


;======================================================================
; print AX as 4 hex characters to the UART
;======================================================================

print_word:
    push ax
    push ax
    mov al,ah
    call print_hex
    pop ax
    call print_hex
    pop ax
    ret


;======================================================================
; print an ASCIIZ string at CS:SI to the UART
;======================================================================

print_str:
puts:
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

    out UART_DATA,al
    jmp .loop

.exit:
    pop ds
    pop si
    pop ax
    ret



;======================================================================
; raw read
;======================================================================

;======================================================================
; are there any bytes waiting?
;======================================================================

raw_ready:
    in al,UART_CMD         ; any bytes?
    ret


;======================================================================
; read a char into AL from the UART (blocking)
;======================================================================

raw_chr:
getc:
    in al,UART_CMD         ; any characters?
    cmp al,0
    jz raw_chr
    in al,UART_DATA         ; read character
    ret


;======================================================================
; read 2 hex characters into AL from the UART (blocking)
;======================================================================

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
    in al,UART_CMD         ; any characters?
    cmp al,0
    jz .nibble

    in al,UART_DATA         ; read character
    cmp al,41h
    jae .letter
    sub al,30h              ; '0'
    ret

.letter:
    sub al,37h              ; 'A' - 10
    ret


;======================================================================
; read an ASCIIZ string into ES:DI from the UART (blocking)
;======================================================================

raw_str:
gets:
    push ax
    push di
    push es
    cld

.loop:
    in al,UART_CMD         ; any characters?
    cmp al,0
    jz .loop

    in al,UART_DATA         ; read character
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

;======================================================================
; are there any bytes waiting?
;======================================================================

read_ready:
    in al,UART_CMD         ; any bytes?
    ret


;======================================================================
; read a char into AL from the UART (blocking)
;======================================================================

read_chr:
    call raw_chr
    out UART_DATA,al
    ret


;======================================================================
; read 2 hex characters into AL from the UART (blocking)
;======================================================================

read_hex:
    call raw_hex
    out UART_DATA,al
    ret


;======================================================================
; read an ASCIIZ string into ES:DI from the UART (blocking)
;======================================================================

read_str:
    push ax
    push di
    push es
    cld

.loop:
    in al,UART_CMD         ; any characters?
    cmp al,0
    jz .loop

    in al,UART_DATA         ; read character
    stosb
    out UART_DATA,al

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

