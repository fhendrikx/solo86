
;----------------------------------------------------------------------
; INT handlers
;----------------------------------------------------------------------

; we don't assume that anything is working at this point, as the fault
; could be serious. Lets print out a message and shutdown everything.

int_exc00:
    mov si,exc_00
    jmp int_handler

int_exc01:
    mov si,exc_01
    jmp int_handler

int_exc02:
    mov si,exc_02
    jmp int_handler

int_exc03:
    mov si,exc_03
    jmp int_handler

int_exc04:
    mov si,exc_04
    jmp int_handler

int_exc05:
    mov si,exc_05
    jmp int_handler

int_exc06:
    mov si,exc_06
    jmp int_handler

int_exc07:
    mov si,exc_07
    jmp int_handler

int_exc08:
    mov si,exc_08
    jmp int_handler

int_exc09:
    mov si,exc_09
    jmp int_handler

int_exc0A:
    mov si,exc_0A
    jmp int_handler

int_exc0B:
    mov si,exc_0B
    jmp int_handler

int_exc0C:
    mov si,exc_0C
    jmp int_handler

int_exc0D:
    mov si,exc_0D
    jmp int_handler

int_exc0E:
    mov si,exc_0E
    jmp int_handler

int_exc0F:
    mov si,exc_0F
    jmp int_handler

int_exc10:
    mov si,exc_10
    jmp int_handler

int_exc11:
    mov si,exc_11
    jmp int_handler

int_exc12:
    mov si,exc_12
    jmp int_handler

int_exc13:
    mov si,exc_13
    jmp int_handler

int_exc14:
    mov si,exc_14
    jmp int_handler

int_exc15:
    mov si,exc_15
    jmp int_handler

int_exc16:
    mov si,exc_16
    jmp int_handler

int_exc17:
    mov si,exc_17
    jmp int_handler

int_exc18:
    mov si,exc_18
    jmp int_handler

int_exc19:
    mov si,exc_19
    jmp int_handler

int_exc1A:
    mov si,exc_1A
    jmp int_handler

int_exc1B:
    mov si,exc_1B
    jmp int_handler

int_exc1C:
    mov si,exc_1C
    jmp int_handler

int_exc1D:
    mov si,exc_1D
    jmp int_handler

int_exc1E:
    mov si,exc_1E
    jmp int_handler

int_exc1F:
    mov si,exc_1F
    jmp int_handler


    ; generic fault handler

int_handler:
    mov ax,cs
    mov ds,ax               ; CS => DS
    cld

.loop:
    lodsb
    cmp al,0                ; are we done?
    jz .done

    out COM_DATA,al
    jmp .loop

.done:
    cli
    hlt
    jmp $                   ; really, stop here


    ; generic dummy handler

int_dummy:
    iret

