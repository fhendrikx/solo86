;======================================================================
; Solo86 BASIC
; Ferry Hendrikx, May 2024
;
; Adapted from bootBASIC by Oscar Toledo G.
;======================================================================

cpu 8086

org 0


;======================================================================
; defines
;======================================================================

; important segments
cseg_init       equ 0F000h  ; CS
cseg            equ 01000h  ; CS after relocation
dseg            equ 01000h
sseg            equ 01000h

; I/O addresses
uart_ctrl       equ 20h
uart_data       equ 22h

; basic
vars            equ 0x7e00  ; Variables (multiple of 256)
line            equ 0x7e80  ; Line input

program         equ 0x8000  ; Program address
                            ; (notice optimizations dependent on this address)

stack           equ 0xff00  ; Stack address
max_line        equ 1000    ; First unavailable line number
max_length      equ 20      ; Maximum length of line
max_size        equ max_line*max_length ; Max. program size


;======================================================================
; 0xf0000 - basic start
;======================================================================

init:
; clear interrupts
    cli
    cld

; copy ROM to RAM
    mov ax,cseg_init
    mov ds,ax
    xor si,si

    mov ax,cseg
    mov es,ax
    xor di,di

    mov cx,08000h

.copy_rom:
    movsw
    loop .copy_rom

    jmp cseg:relocate


;======================================================================
; 0x10xxx - basic relocation
;======================================================================

relocate:
; initialise DS
    mov ax,dseg
    mov ds,ax

; initialise SS/SP
    mov ax,sseg
    mov ss,ax

; start
start:
    cld
    mov di,program      ; Point to program

.clear_program:
    mov byte [di],0x0d  ; Fill with Carriage Return (CR) character
    inc di              ; Until reaching maximum 64K (DI becomes zero)
    jne .clear_program


;======================================================================
; main loop
;======================================================================

main_loop:
    mov sp,stack
    mov ax,main_loop
    push ax

    mov al,'>'          ; Show prompt
    call input_line     ; Accept line
    call input_number   ; Get number
    or ax,ax            ; No number or zero?
    je statement        ; Yes, jump
    call find_line      ; Find the line
    xchg ax,di
;   mov cx,max_length   ; CX loaded with this value in 'find_line'
    rep movsb           ; Copy entered line into program
    ret


;======================================================================
; Handle 'if' statement
;======================================================================

if_statement:
    call expr           ; Process expression
    or ax,ax            ; Is it zero?
    je f6               ; Yes, return (ignore if)

statement:
    call spaces         ; Avoid spaces
    cmp byte [si],0x0d  ; Empty line?
    je f6               ; Yes, return
    mov di,statements   ; Point to statements list
f5:
    mov al,[di]         ; Read length of the string
    inc di              ; Avoid length byte
    cbw                 ; Make AH zero
    dec ax              ; Is it zero?
    je f4               ; Yes, jump
    xchg ax,cx
    push si             ; Save current position
f16:
    rep cmpsb           ; Compare statement
    jne f3              ; Equal? No, jump
    pop ax
    call spaces         ; Avoid spaces
    jmp word [di]       ; Jump to process statement

f3:
    add di,cx           ; Advance the list pointer
    inc di              ; Avoid the address
    inc di
    pop si
    jmp f5              ; Compare another statement

f4:
    call get_variable   ; Try variable
    push ax             ; Save address
    lodsb               ; Read a line letter
    cmp al,'='          ; Is it assignment '=' ?
    je assignment       ; Yes, jump to assignment.


;======================================================================
; Handle 'list' statement
;======================================================================

list_statement:
    xor ax,ax           ; Start from line zero
.next:
    push ax
    call find_line      ; Find program line
    xchg ax,si
    cmp byte [si],0x0d  ; Empty line?
    je .eol             ; Yes, jump
    pop ax
    push ax
    call output_number  ; Show line number
.loop:
    lodsb               ; Show line contents
    call print_chr
    cmp byte [si],0x0d  ; EOL?
    jne .loop           ; Jump if it wasn't 0x0d (CR)
    mov al,0x0a
    call print_chr
.eol:
    pop ax
    inc ax              ; Go to next line
    cmp ax,max_line     ; Finished?
    jne .next           ; No, continue
f6:
    ret


;======================================================================
; Handle 'input' statement
;======================================================================

input_statement:
    call get_variable   ; Get variable address
    push ax             ; Save it
    mov al,'?'          ; Prompt
    call input_line     ; Wait for line
    ;
    ; Second part of the assignment statement
    ;
assignment:
    call expr           ; Process expression
    pop di
    stosw               ; Save onto variable
    ret


;======================================================================
; Handle 'inp' statement
;======================================================================

inp_statement:
    call expr           ; Handle expression
    mov dx,ax

    cmp byte [si],','   ; Comma found?
    jne error
    inc si
    xor ax,ax
    in al,dx
    push ax

    call get_variable   ; get variable address
    mov di,ax
    pop ax
    stosw
    ret


;======================================================================
; Handle 'out' statement
;======================================================================

out_statement:
    call expr           ; Handle expression
    mov dx,ax
    cmp byte [si],','   ; Comma found?
    jne error
    inc si
    call expr           ; Handle expression
    mov ah,0
    out dx,al
    ret


;======================================================================
; Handle expression.
;======================================================================

; First tier: addition & subtraction.
expr:
        call expr1          ; Call second tier
f20:    cmp byte [si],'-'   ; Subtraction operator?
        je f19              ; Yes, jump
        cmp byte [si],'+'   ; Addition operator?
        jne f6              ; No, return
        push ax
        call expr1_2        ; Call second tier
f15:    pop cx
        add ax,cx           ; Addition
        jmp f20             ; Find more operators

f19:
        push ax
        call expr1_2        ; Call second tier
        neg ax              ; Negate it (a - b converted to a + -b)
        jmp f15

; Second tier: division & multiplication.
expr1_2:
        inc si              ; Avoid operator
expr1:
        call expr2          ; Call third tier
f21:    cmp byte [si],'/'   ; Division operator?
        je f23              ; Yes, jump
        cmp byte [si],'*'   ; Multiplication operator?
        jne f6              ; No, return

        push ax
        call expr2_2        ; Call third tier
        pop cx
        imul cx             ; Multiplication
        jmp f21             ; Find more operators

f23:
        push ax
        call expr2_2        ; Call third tier
        pop cx
        xchg ax,cx
        cwd                 ; Expand AX to DX:AX
        idiv cx             ; Signed division
        jmp f21             ; Find more operators

; Third tier: parentheses, numbers and vars.
expr2_2:
        inc si              ; Avoid operator
expr2:
        call spaces         ; Jump spaces
        lodsb               ; Read character
        cmp al,'('          ; Open parenthesis?
        jne f24
        call expr           ; Process inner expr.
        cmp byte [si],')'   ; Closing parenthesis?
        je spaces_2         ; Yes, avoid spaces
        jmp error           ; No, jump to error

f24:    cmp al,'@'          ; Variable?
        jnc f25             ; Yes, jump
        dec si              ; Back one letter...
        call input_number   ; ...to read number
        jmp short spaces

f25:    cmp al,'r'
        jne f22
        cmp byte [si],0x6e
        jne f22
        lodsw               ; Advance SI by 2
        in al,0x40          ; Read timer counter 0
        mov ah,0
        ret

f22:    call get_variable_2 ; Get variable address
        xchg ax,bx
        mov ax,[bx]         ; Read
        ret                 ; Return



;======================================================================
; Handle 'goto' statement
;======================================================================

goto_statement:
    call expr           ; Handle expression
    jmp run_goto


;======================================================================
; Handle 'run' statement (equivalent to 'goto 0')
;======================================================================

run_statement:
    xor ax,ax

run_goto:
    call find_line      ; Find line in program

f27:
    cmp sp,stack-2      ; In interactive mode?
    je f31              ; Yes, jump.
    mov [stack-4],ax    ; No, replace the saved address of next line
    ret

f31:
    push ax
    pop si
    add ax,max_length   ; Point to next line
    push ax             ; Save for next time (this goes into address stack-4)
    call statement      ; Process current statement
    pop ax              ; Restore address of next line (could have changed)
    cmp ax,program+max_size ; Reached the end?
    jne f31             ; No, continue
    ret                 ; Yes, return


;======================================================================
; Handle "print" statement
;======================================================================

print_statement:
    lodsb               ; Read source
    cmp al,0x0d         ; End of line?
    je .exit
    cmp al,'"'          ; Double quotes?
    jne .expr           ; No, jump

.quoted:
    lodsb               ; Read string contents
    cmp al,'"'          ; Double quotes?
    je .semi            ; Yes, jump
    call print_chr
    cmp al,0x0d         ;
    jne .quoted         ; Jump if not finished with 0x0d (CR)
    ret                 ; Return

.expr:
    dec si
    call expr           ; Handle expression
    call output_number

.semi:
    lodsb               ; Read next character
    cmp al,';'          ; Is it semicolon?
    je .exit
    mov al,0x0a
    call print_chr

.exit:
    ret                 ; Yes, return



;======================================================================
; Handle 'system' statement
;======================================================================

system_statement:
    cli
    hlt


;======================================================================
; error handling
;======================================================================

error:
    mov si,error_message
    call print_str      ; Show error message
    jmp main_loop       ; Exit to main loop

error_message:
    db "error", 0Ah, 00h


;======================================================================
; Get variable address.
;======================================================================
; Also avoid spaces.

get_variable:
    lodsb               ; Read source
get_variable_2:
    and al,0x1f         ; 0x61-0x7a -> 0x01-0x1a
    add al,al           ; x 2 (each variable = word)
    mov ah,vars>>8      ; Setup high-byte of address
    dec si
    ;
    ; Avoid spaces after current character
    ;
spaces_2:
    inc si
    ;
    ; Avoid spaces
    ; The interpreter depends on this routine not modifying AX
    ;
spaces:
    cmp byte [si],' '   ; Space found?
    je spaces_2         ; Yes, move one character ahead.
    ret                 ; No, return.


;======================================================================
; Find line in program
;======================================================================
; Entry:
;   ax = line number
; Result:
;   ax = pointer to program.
;   cx = max. length allowed for line.
find_line:
    mov cx,max_length
    mul cx
    add ax,program
    ret


;======================================================================
; Output unsigned number
;======================================================================
; AX = value
output_number:
f26:
    xor dx,dx           ; DX:AX
    mov cx,10           ; Divisor = 10
    div cx              ; Divide
    or ax,ax            ; Nothing at left?
    push dx
    je f8               ; No, jump
    call f26            ; Yes, output left side
f8:
    pop ax
    add al,'0'          ; Output remainder as...
    call print_chr
    ret


;======================================================================
; Read number in input.
;======================================================================
; AX = result
input_number:
    xor bx,bx           ; BX = 0
f11:
    lodsb               ; Read source
    sub al,'0'
    cmp al,10           ; Digit valid?
    cbw
    xchg ax,bx
    jnc f12             ; No, jump
    mov cx,10           ; Multiply by 10
    mul cx
    add bx,ax           ; Add new digit
    jmp f11             ; Continue

f12:
    dec si              ; SI points to first non-digit
    ret


;======================================================================
; Input line from keyboard
;======================================================================
; Entry:
;   al = prompt character
; Result:
;   buffer 'line' contains line, finished with CR
;   SI points to 'line'.

input_line:
    call print_chr
    mov si,line
    push si
    pop di              ; Target for writing line

.read:
    call read_chr       ; Read keyboard
    stosb               ; Save key in buffer
    cmp al,0x08         ; Backspace?
    jne .next           ; No, jump
    dec di              ; Get back one character
    dec di

.next:
    cmp al,0x0d         ; CR pressed?
    jne .read           ; No, wait another key
    ret                 ; Yes, return


;======================================================================
; statements
;======================================================================

; First one byte with length of string
; Then string with statement
; Then a word with the address of the code

statements:
    db 5,"goto"
    dw goto_statement

    db 3,"if"
    dw if_statement

    db 6,"input"
    dw input_statement

    db 5,"list"
    dw list_statement

    db 4,"new"
    dw start

    db 4,"inp"
    dw inp_statement

    db 4,"out"
    dw out_statement

    db 6,"print"
    dw print_statement

    db 4,"run"
    dw run_statement

    db 7,"system"
    dw system_statement

    db 5,"quit"
    dw system_statement

    db 1


;======================================================================
; includes
;======================================================================

%include "stdio.inc"


;======================================================================
; reset code (called on CPU reset)
;======================================================================

TIMES 65536-16-($-$$)       db 0FFh

reset:
    jmp cseg_init:init


;======================================================================
; filler
;======================================================================

TIMES 65536-($-$$)          db 0FFh

