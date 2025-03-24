;======================================================================
; Solo86 Monitor
; Ferry Hendrikx/Dylan Hall, April 2024
;======================================================================

cpu 8086

org 0


;======================================================================
; includes
;======================================================================

%include "const.inc"
%include "macro.inc"


;======================================================================
; interrupt vectors
;======================================================================

%include "vectors.inc"


;======================================================================
; 0xf0100 - monitor start
;======================================================================

init:
; clear interrupts
    cli
    cld

    leds 01b

    ; set DS and SS
    mov ax,cs
    mov ds,ax
    mov ss,ax

    ;call beep_now
    ;call tune

; ensure UART has interrupts disabled
    xor al,al
    out UART_CTRL,al

    leds 011b

; copy ROM to RAM
; don't use the stack yet as the ROM copy will overwrite any stored data
    mov ax,CSEG_INIT
    mov ds,ax
    xor si,si

    mov ax,CSEG
    mov es,ax
    xor di,di

    mov cx,08000h

.copy:
    movsw
    loop .copy

    leds 0111b

    jmp CSEG:relocate       ; here we go!


;======================================================================
; 0x00xxx - monitor relocation
;======================================================================

relocate:
    leds 01111b

; initialise DS
    mov ax,DSEG
    mov ds,ax

; initialise stack
    mov ax,SSEG
    mov ss,ax               ; SS:SP
    mov sp,0FFFFh

    leds 011111b

; setup memory banking table
; map RAM into the top half of memory
    ; mov al,011000b
    ; mov dx,BANK_TABLE
    ; mov cx,8

; .bank_init:
    ; out dx,al
    ; inc al
    ; inc dx
    ; inc dx
    ; loop .bank_init

; map ROM into the top half of memory, skip monitor
    mov al,1
    out BANK_ROW_0,al
    mov al,2
    out BANK_ROW_1,al
    mov al,3
    out BANK_ROW_2,al
    mov al,4
    out BANK_ROW_3,al
    mov al,5
    out BANK_ROW_4,al
    mov al,6
    out BANK_ROW_5,al
    mov al,7
    out BANK_ROW_6,al
; map RAM into the last segment so that hex loading still works
    mov al,01Fh
    out BANK_ROW_7,al

    leds 0111111b

; initialise RTC
    call rtc_init

; enable interrupts
    sti

    leds 01111111b

;======================================================================
; welcome
;======================================================================

; welcome
    print mon_welcome

    leds 011111111b

; scan for ROMs
.scan:
    call rom_scan

    ; do we have any?
    mov dl,[cs:rom_cnt]
    cmp dl,0                ; did we find any?
    je .debugger            ; no

    call rom_menu

.debugger:
    call debug_menu
    jmp .scan


;======================================================================
; code
;======================================================================

%include "beep.inc"
%include "debug.inc"
%include "delay.inc"
%include "ihf.inc"
%include "mem.inc"
%include "mem_dasm.inc"
%include "messages.inc"
%include "rom.inc"
%include "rtc.inc"
%include "sound.inc"
%include "stdio.inc"


;======================================================================
; interrupts
;======================================================================

int_hwexc:
    iret

int_irq0:
    push ax
    mov ax,[ cs:irq0_cnt ]
    inc ax
    mov [ cs:irq0_cnt ],ax
    pop ax
    iret

int_irq1:
    push ax
    mov ax,[ cs:irq1_cnt ]
    inc ax
    mov [ cs:irq1_cnt ],ax
    pop ax
    iret

int_irq2:
    push ax
    mov ax,[ cs:irq2_cnt ]
    inc ax
    mov [ cs:irq2_cnt ],ax
    pop ax
    iret

int_irq3:
    push ax
    mov ax,[ cs:irq3_cnt ]
    inc ax
    mov [ cs:irq3_cnt ],ax
    pop ax
    iret

int_dummy:
    iret


;======================================================================
; data
;======================================================================

; padding
TIMES 65536-1024-($-$$)     db 0FFh

%include "data.inc"


;======================================================================
; reset code (called on CPU reset)
;======================================================================

; padding
TIMES 65536-16-($-$$)       db 0FFh

reset:
    jmp CSEG_INIT:init


;======================================================================
; filler
;======================================================================

TIMES 65536-($-$$)          db 0FFh

