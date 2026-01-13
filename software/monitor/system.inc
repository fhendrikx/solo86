
;=========================================================================
; sys_init
; Input:
;   None
;-------------------------------------------------------------------------
sys_init:
    push ax
    push bx
    push cx
    push dx
    push ds
    push es

    ; memory banking

    ; map ROM into top-half of memory
    mov al,1
    out BNK_ROW_0,al
    mov al,2
    out BNK_ROW_1,al
    mov al,3
    out BNK_ROW_2,al

    ; map RAM into the last 128k so that hex loading still works
    mov al,0Fh
    out BNK_ROW_3,al

    ; expansion detection

    xor ax,ax
    or  ax,BNK_PRESENT
    mov [ cs:hardware ],ax

    mov bx,PIT_PRESENT
    or  bx,RTC_PRESENT
    mov dx,PIT_DETECT
    call exp_detect

    mov bx,COM_PRESENT
    mov dx,COM_DETECT
    call exp_detect

    mov bx,UART_PRESENT
    mov dx,UART_DETECT
    call exp_detect

    mov bx,ATA_PRESENT
    mov dx,ATA_DETECT

    mov bx,PNL_PRESENT
    mov dx,PNL_DETECT
    call exp_detect

    ; panel switches

    ; check if the panel is installed
    mov ax,[ cs:hardware ]
    test ax,PNL_PRESENT
    je .no_pnl

    ; set pnl_beep
    in al,PNL_DATA
    and al,00000001b
    mov [ cs:pnl_beep ],al

    ; set pnl_rom_app
    in al,PNL_DATA
    and al,00001110b
    shr al,1
    mov [ cs:pnl_rom_app ],al

    ; set pnl_auto
    in al,PNL_DATA
    and al,00010000b
    shr al,4
    mov [ cs:pnl_auto ],al

    ; set pnl_baud
    in al,PNL_DATA
    and al,01100000b
    shr al,5
    mov [ cs:pnl_baud ],al

    ; set pnl_con
    in al,PNL_DATA
    and al,10000000b
    shr al,7
    mov [ cs:pnl_con ],al

.no_pnl:

    ; COM port

    ; check if the PiUART is installed
    mov ax,[ cs:hardware ]
    test ax,COM_PRESENT
    je .no_com

    ; disable interrupts
    xor al,al
    out COM_CTRL,al

    ; VIDEO mode

    ; set console video mode
    xor al,al
    out VC_CTRL,al

.no_com:

    ; 16C550 UART

    ; check if the UART is installed
    mov ax,[ cs:hardware ]
    test ax,UART_PRESENT
    je .no_uart

    ; enable FIFO
    mov al,0C7h
    out UART_A_FCR,al

    ; DLAB = 1
    mov al,080h
    out UART_A_LCR,al

    ; set divisor
    xor bx,bx                               ; clear BX
    mov bl,[ cs:pnl_baud ]                  ; load the switch front panel baud rate
    shl bl,1                                ; multiple by two as the lookup table is 2 bytes per entry
    mov ax,[ cs:bx + uart_divisor_table ]   ; lookup divisor
    out UART_A_DLL,al                       ; output low byte
    xchg al,ah
    out UART_A_DLM,al                       ; output high byte

    ; DLAB = 0, 8N1
    mov al,03
    out UART_A_LCR,al

    ; check if CTS is set
    ; assume CTS being set means we can use autoflow control
    in al,UART_A_MSR
    test al,00010000b   ; CTS bit
    jz .no_rtscts

    ; enable autoflow control, DTR, RTS
    mov al,023h
    out UART_A_MCR,al
    jmp .done_mcr

.no_rtscts:
    ; enable DTR, RTS
    mov al,03h
    out UART_A_MCR,al

.done_mcr:

.no_uart:

    pop es
    pop ds
    pop dx
    pop cx
    pop bx
    pop ax
    ret

exp_detect:
    push dx

    in al,dx
    cmp al,0AAh             ; look for 0xAA
    je .found
    and al,0F8h
    cmp al,0A8h             ; look for 0xA8 (for the epoch card)
    je .found
    jmp .end

.found:
    mov ax,[ cs:hardware ]
    or ax,bx
    mov [ cs:hardware ],ax

.end:
    pop dx
    ret


;=========================================================================
; sys_remap
; Input:
;   AX - segment address
;   CL - count
;-------------------------------------------------------------------------
sys_remap:
    push ax
    push cx

    ; set defaults
    and ax,0E000h           ; ensure we get segment only
    cmp cl,0
    jg .loop
    mov cl,1                ; ensure count=1 if count=0

.loop:
    call sys_remap_seg

    add ax,02000h           ; increment segment

    dec cl                  ; decrement count
    cmp cl,0
    jg .loop

.exit:
    pop cx
    pop ax
    ret


;=========================================================================
; sys_remap_seg
; Input:
;   AX - segment address
;-------------------------------------------------------------------------
sys_remap_seg:
    push ax
    push cx
    push dx

    and ax,0E000h           ; ensure we get segment only

    ; bank row -> I/O port, value
    ; 8000 -> 10h, 0Ch
    ; A000 -> 12h, 0Dh
    ; C000 -> 14h, 0Eh
    ; E000 -> 16h, 0Fh

    mov cl,12
    shr ax,cl               ; 8000 -> 0008
    mov dx,ax

    ; calculate value
    sub al,08h              ; 08, 0A, 0C, 0E -> 00, 02, 04, 06
    shr al,1                ; div by 2
    add al,0Ch              ; 00, 01, 02, 03 -> 0C, 0D, 0E, 0F

    ; calculate port
    add dx,08h              ; 0008 -> 0010

    out dx,al

    pop dx
    pop cx
    pop ax
    ret

