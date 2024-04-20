;======================================================================
; Ferry Hendrikx, April 2024
;======================================================================

;======================================================================
; print a single character in AL
;======================================================================

print_chr:
putc:
    out uart_data,al
    ret


;======================================================================
; print an ASCIIZ string as CS:SI to UART
;======================================================================

print_str:
puts:
    pushf
    push ax
    push bx
    push si
    push ds
    push cs
    pop ds              ; CS => DS
    cld

.loop:
    lodsb
    or al,al            ; are we done?
    jz .exit
    out uart_data,al
    jmp .loop

.exit:
    pop ds
    pop si
    pop bx
    pop ax
    popf
    ret


;======================================================================
; print a hex representation of the integer value in AL
;======================================================================

print_hex:
    push ax
    shr al,1            ; shr al,4 not possible
    shr al,1
    shr al,1
    shr al,1
    call .nibble
    pop ax
    and al,0fh
    call .nibble
    ret

.nibble:
    cmp al,9
    jg .letter
    add al,30h          ; '0'
    out uart_data,al
    ret

.letter:
    add al,37h          ; 'A' - 10
    out uart_data,al
    ret


;======================================================================
; read a single character into AL
;======================================================================

get_chr:
getc:
    in al,uart_data     ; read character
    ret


;======================================================================
; are there any characters waiting?
;======================================================================

has_chr:
    in al,uart_ctrl     ; any characters?
    ret

