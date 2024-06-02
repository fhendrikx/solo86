;======================================================================
; Ferry Hendrikx, April 2024
;======================================================================

uart_ctrl       equ 20h
uart_data       equ 22h


;======================================================================
; print a char in AL to the UART
;======================================================================

print_chr:
putc:
    out uart_data,al
    ret


;======================================================================
; read a char into AL from the UART (blocking)
; echo to the UART if DL==1
;======================================================================

read_chr:
getc:
    in al,uart_ctrl     ; any characters?
    cmp al,0
    jz read_chr

    in al,uart_data     ; read character

    cmp dl,1            ; echo?
    jne .exit
    out uart_data,al

.exit:
    ret


;======================================================================
; are there any bytes waiting?
;======================================================================

read_ready:
    in al,uart_ctrl     ; any bytes?
    ret


;======================================================================
; print an ASCIIZ string at CS:SI to the UART
;======================================================================

print_str:
puts:
    push ax
    push si
    push ds
    push cs             ; CS => DS
    pop ds
    cld

.loop:
    lodsb
    cmp al,0            ; are we done?
    jz .exit
    out uart_data,al
    jmp .loop

.exit:
    pop ds
    pop si
    pop ax
    ret


;======================================================================
; read an ASCIIZ string into ES:DI from the UART (blocking)
; echo to the UART if DL==1
;======================================================================

read_str:
gets:
    push ax
    push di
    push es
    cld

.loop:
    in al,uart_ctrl     ; any characters?
    cmp al,0
    jz .loop

    in al,uart_data     ; read character
    stosb

    cmp dl,1            ; echo?
    jne .continue
    out uart_data,al

.continue:
    cmp al,10           ; are we done?
    jz .exit

    jmp .loop

.exit:
    mov al,0            ; terminator
    stosb
    pop es
    pop di
    pop ax
    ret


;======================================================================
; print AL as 2 hex characters to the UART
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
; read 2 hex characters into AL from the UART (blocking)
; echo to the UART if DL==1
;======================================================================

read_hex:
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
    in al,uart_ctrl     ; any characters?
    cmp al,0
    jz .nibble

    in al,uart_data     ; read character

    cmp dl,1            ; echo?
    jne .continue
    out uart_data,al

.continue:
    cmp al,41h
    jae .letter

    sub al,30h          ; '0'
    ret

.letter:
    sub al,37h          ; 'A' - 10
    ret

