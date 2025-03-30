;=========================================================================
; beep_now
; Input:
;   None
; Notes:
;   Beep the speaker
;-------------------------------------------------------------------------
beep_now:
    ; save state
    push ax

    ; check if the Epoch module is installed
    in al,PIT_SIG
    and al,11111000b
    cmp al,10101000b
    jne .done

    ; set counter 2 mode 3
    mov al,0B6h
    out PIT_CTRL,al

    ; set 1000Hz frequency
    mov ax,PIT_FREQ/1000
    out PIT_CNTR2,al
    xchg al,ah
    out PIT_CNTR2,al

    ; enable counter 2 output
    mov al,01h
    out PIT_CNTR2_EN,al

    ; delay 100ms
    mov ax,100
    call delay_ms

    ; disable counter 2 output
    xor al,al
    out PIT_CNTR2_EN,al

.done:

    ; restore state
    pop ax

    ret
