;======================================================================
; 80x86 Monitor
; Ferry Hendrikx, April 2024
;======================================================================

cpu 8086

%include "defines.inc"
%include "delay.inc"
%include "leds.inc"
%include "macro.inc"
%include "messages.inc"
%include "uart.inc"


;======================================================================
; defines
;======================================================================

cseg        equ 0F000h
dseg        equ 01000h
sseg        equ 07000h

leds_data   equ 08h
uart_ctrl   equ 20h
uart_data   equ 22h
    
org         START


;======================================================================
; monitor start
;======================================================================

start:
; initialise SS/SP
    mov ax,sseg
    mov ss,ax
    mov sp,0FFFFh

    cli
    cld

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

    call enable_uart_interrupts

; do nothing while we wait for an interrupt
.wait:
    jmp .wait
    

halt:
    hlt
    jmp halt


;======================================================================
; intr_debug
;======================================================================

int_dummy:
    iret

int_irq0:

    push ax

    mov al,1
    call set_leds
    
    mov ax,1000
    call delay_ms
    
    pop ax

    iret
    
int_pi:

    push ax
    
    in al,uart_data
    call set_leds
    call print_chr
    
    pop ax
    
    iret

;======================================================================
; interrupts
;======================================================================

setloc 0FF00h

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
    dw int_irq0		; 20 - IRQ0
    dw int_dummy	; 21 - IRQ1
    dw int_dummy	; 22 - IRQ2
    dw int_pi		; 23 - PiUART
interrupts_end:	
    
;======================================================================
; reset code (called on CPU reset)
;======================================================================

setloc 0FFF0h

reset:
    jmp cseg:start


setloc 0FFF5h
    db BUILD

setloc 0FFFEh
    db VERSION
    db 0FFh

