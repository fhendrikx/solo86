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
; interrupt vectors (first 64 only)
;======================================================================

%include "vectors.inc"


;======================================================================
; 0xf0100 - monitor start (ROM location; don't move this)
;======================================================================

init:
    ; at power up this code is executing from ROM. After loading from an
    ; IHX file, this code is executing from RAM. Be aware that segments
    ; and stack pointers are not initialised

    ; clear interrupts
    cli
    cld

    leds 00000001b          ; 1 LED

    ; copy ROM to RAM (F000:0000 => 0000:0000)
    ; don't use the stack yet as the ROM copy will overwrite stored data
    ; this will also copy the first 64 interrupt vectors across; we'll
    ; need to clean out the rest later.

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

    leds 00000011b          ; 2 LEDs

    jmp CSEG:relocate       ; here we go!


;======================================================================
; padding
;======================================================================

; padding
TIMES 1024-($-$$)           db 00h


;======================================================================
; 0x00400 - monitor start (relocation point)
;======================================================================

relocate:
    leds 00000111b          ; 3 LEDs

    ; initialise DS
    mov ax,DSEG
    mov ds,ax

    ; initialise stack
    mov ax,SSEG
    mov ss,ax               ; SS:SP
    mov sp,0FFFFh

    leds 00001111b          ; 4 LEDs

    ; initialise
    call sys_init
    call rtc_init

    leds 00001111b          ; 5 LEDs

    ; startup sound

    ; check if the panel is installed
    mov ax,[ cs:hardware ]
    test ax,PNL_PRESENT
    je .no_sound

    ; check if switch 0 is on
    in al,PNL_DATA
    test al,00000001b
    jz .no_sound

    call beep_now

    ; ; setup tune pointer
    ; mov ax,cs
    ; mov ds,ax
    ; mov si,imperial
    ; call tune

.no_sound:
    leds 00111111b          ; 6 LEDs

    ; enable interrupts
    sti

    leds 01111111b          ; 7 LEDs

;======================================================================
; welcome
;======================================================================

    ; welcome
    print mon_welcome
    print mon_version

    leds 11111111b          ; 8 LEDs

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
%include "ihx.inc"
%include "mem.inc"
%include "messages.inc"
%include "rom.inc"
%include "rtc.inc"
%include "sound.inc"
%include "stdio.inc"
%include "system.inc"
%include "unasm.inc"
%include "version.inc"


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
    out 70h, al				; fake EOI
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

