;======================================================================
; 80x86 Monitor
; Ferry Hendrikx, April 2024
;======================================================================

cpu 8086

org 0F000h


;======================================================================
; includes
;======================================================================

%include "macro.inc"


;======================================================================
; defines
;======================================================================

cseg                equ 0F000h
dseg                equ 01000h
sseg                equ 07000h

leds_data           equ 08h
uart_ctrl           equ 20h
uart_data           equ 22h


;======================================================================
; monitor start
;======================================================================

start:
    cli
    cld

; initialise SS/SP
    mov ax,sseg
    mov ss,ax
    mov sp,0FFFFh

; initialise interrupts
    push cs
    pop ds

    xor di,di
    mov es,di
    mov si,interrupts
    mov ax,cseg
    mov cx,(interrupts_end - interrupts)/2

.copy:
    movsw               ; off (copied from table)
    stosw               ; seg (AX)
    loop .copy

; initialise DS/ES
    mov ax,dseg
    mov ds,ax
    mov es,ax

    sti

; welcome
    mov si,mesg_welcome
    call print_str

    mov si,mesg_upload
    call print_str

; read intel hex file
    call read_hex_file

    mov si,mesg_upload_done
    call print_str


;    xor ax,ax
;    mov ds,ax
;    mov es,ax
;    jmp cseg:start

halt:
    hlt
    jmp halt


;======================================================================
; includes
;======================================================================

%include "delay.inc"
%include "intel.inc"
%include "leds.inc"
%include "messages.inc"
%include "stdio.inc"


;======================================================================
; intr_debug
;======================================================================

int_dummy:
    iret

int_pi:

    push ax

    in al,uart_data
    out leds_data,al
    out uart_data,al

    pop ax

    iret


;======================================================================
; interrupt offsets
;======================================================================

interrupts:
    dw int_dummy        ; 00 - divide by zero
    dw int_dummy        ; 01 - single step
    dw int_dummy        ; 02 - NMI
    dw int_dummy        ; 03 - breakpoint
    dw int_dummy        ; 04 - INTO overflow
    dw int_dummy        ; 05 - BOUND range
    dw int_dummy        ; 06 - invalid opcode
    dw int_dummy        ; 07 - extension not available
    dw int_dummy        ; 08 - interrupt table too small
    dw int_dummy        ; 09 - segment overrun
    dw int_dummy        ; 0A - reserved
    dw int_dummy        ; 0B - reserved
    dw int_dummy        ; 0C - reserved
    dw int_dummy        ; 0D - reserved
    dw int_dummy        ; 0E - reserved
    dw int_dummy        ; 0F - reserved
    dw int_dummy        ; 10 - extension error
    dw int_dummy        ; 11 - reserved
    dw int_dummy        ; 12 - reserved
    dw int_dummy        ; 13 - reserved
    dw int_dummy        ; 14 - reserved
    dw int_dummy        ; 15 - reserved
    dw int_dummy        ; 16 - reserved
    dw int_dummy        ; 17 - reserved
    dw int_dummy        ; 18 - reserved
    dw int_dummy        ; 19 - reserved
    dw int_dummy        ; 1A - reserved
    dw int_dummy        ; 1B - reserved
    dw int_dummy        ; 1C - reserved
    dw int_dummy        ; 1D - reserved
    dw int_dummy        ; 1E - reserved
    dw int_dummy        ; 1F - reserved
    dw int_pi           ; 20 - user
interrupts_end:


;======================================================================
; reset code (called on CPU reset)
;======================================================================

; setloc 0FFF0h

TIMES 64*1024-16-($-$$) db 0xFF


reset:
    jmp cseg:start

; setloc 0FFF5h
;    db __?DATE_NUM?__
;;
;;setloc 0FFFEh
;;    db 0FFh
;;    db 0FFh

