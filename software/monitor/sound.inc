; guitar freqs from https://www.researchgate.net/figure/Guitar-Fretboard-frequencies_tbl1_311707611
NOTE_D5     equ     PIT_FREQ/185
NOTE_A6     equ     PIT_FREQ/146
NOTE_D8     equ     PIT_FREQ/220

; notes from https://www.songsterr.com/a/wsa/misc-covers-the-imperial-march-band-version-by-john-williams-from-star-wars-tab-s451992

imperial    dw NOTE_D5, 500
            dw NOTE_D5, 500
            dw NOTE_D5, 500
            dw NOTE_A6, 250
            dw NOTE_D8, 250

            dw NOTE_D5, 500
            dw NOTE_A6, 250
            dw NOTE_D8, 250
            dw NOTE_D5, 500

            dw 0

;=========================================================================
; tune
; Input:
;   ds:si points to a null terminated tune to play
; Output:
;   None
; Notes:
;   See example tune above
;   The tune is an array of 16 bit values <note, duration_ms>
;   note is a value suitable for feeding to the PIT, e.g. PIT_FREQ/note_frequency
;   duration_ms is how long the note should play for in ms
;   The tune is terminated with a null value
;-------------------------------------------------------------------------

tune:
    ; save the state
    push ax
    pushf

    ; check if the Epoch module is installed
    in al,PIT_DETECT
    and al,11111000b
    cmp al,10101000b
    jne .done

    ; set direction
    cld

    ; set counter 2 mode 3
    mov al,0B6h
    out PIT_CTRL,al

.loop:
    lodsw               ; load next note into AX
    cmp ax,0
    jz .loop_done

    out PIT_CNTR2,al
    xchg al,ah
    out PIT_CNTR2,al

    ; enable counter 2 output
    mov al,01h
    out PIT_CNTR2_EN,al

    lodsw               ; load note length into AX
    call delay_ms

    ; a brief pause between notes
    ; disable counter 2 output
    xor al,al
    out PIT_CNTR2_EN,al

    mov ax,50
    call delay_ms

    jmp .loop

.loop_done:

    ; disable counter 2 output
    xor al,al
    out PIT_CNTR2_EN,al

.done:

    ; restore the state
    popf
    pop ax

    ret

