; guitar freqs from https://www.researchgate.net/figure/Guitar-Fretboard-frequencies_tbl1_311707611
NOTE_D5     equ     PIT_FREQ/185
NOTE_A6     equ     PIT_FREQ/146
NOTE_D8     equ     PIT_FREQ/220

; notes from https://www.songsterr.com/a/wsa/misc-covers-the-imperial-march-band-version-by-john-williams-from-star-wars-tab-s451992

notes   dw NOTE_D5, 500
        dw NOTE_D5, 500
        dw NOTE_D5, 500
        dw NOTE_A6, 250
        dw NOTE_D8, 250

        dw NOTE_D5, 500
        dw NOTE_A6, 250
        dw NOTE_D8, 250
        dw NOTE_D5, 500

        dw 0

beep:
    ; set counter 2 mode 3
    mov al,0B6h
    out PIT_CMD, al

    ; set 1000Hz frequency
    mov ax,PIT_FREQ/1000
    out PIT_CNTR2,al
    mov al,ah
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

    ret

tune:
    cld
    mov si,notes

    ; set counter 2 mode 3
    mov al,0B6h
    out PIT_CMD,al

.loop:
    lodsw               ; load next note into AX
    cmp ax,0
    jz .done

    out PIT_CNTR2,al
    mov al,ah
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

.done:

    ; disable counter 2 output
    xor al,al
    out PIT_CNTR2_EN,al

    ret

