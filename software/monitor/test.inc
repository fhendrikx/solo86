
test:
    ; inc
    inc     ax
    inc     byte [bx + 45h]

    ; dec
    dec     cx
    dec     word [es:645h]

    ; add
    add     ax, bx
    add     cx, [ds:0654h]
    add     dx, 6h
    add     word [bx], 1352h

    ; sub
    sub     cx, [ds:12h]
    sub     ax, 45h
    sub     word [bx + 654h], 64h

    ; mul
    mul     al
    mul     bx
    mul     byte [bx + di + 465h]
    imul    bl
    imul    dx
    imul    word [bp + 1h]

    ; div
    div     cl
    div     dx
    div     byte [bp + di + 465h]
    idiv    bl
    idiv    ax
    idiv    word [es:0]

    ; shift
    shl     dh, 1
    shr     word [ds:6454h], 1
    sal     ax, 1
    sar     ax, 2

    ; roll
    rol     ax, cl
    ror     bx, 1
    rcl     byte [es:124h], cl
    rcr     dl, cl

    ; and
    and     al, 02h
    and     bx, 0FFFEh
    test    ax, 465h

    ; or
    or      al, 0Fh
    or      bx, 04AAAh

    ; ascii adjust
    aaa
    aad
    aam
    aas

    ; decimal adjust
    daa
    das

    ; convert byte/word
    cbw
    cwd

    ; cmp
    cmp     dx, word [ds:465h]
    cmp     ax, 0
    cmp     word [bx + si], 0FFFFh
    cmpsb
    cmpsw

    ; mov
    mov     dx, word [es:645Eh]
    mov     bx, dx
    mov     es, word [cs:0FFh]
    mov     ss, cx
    mov     ax, word [ds:3h]
    mov     [DS:12h], al
    movsb
    repne   movsw
    mov     dx, 1234h
    mov     byte [bx + di + 21h], 78h
    xchg    al, ah

    ; in
    in      al, 0F5h
    in      al, dx

    ; out
    out     12h, ax
    out     dx, al

    ; push
    push    ds
    push    ax
    push    al
    pushf
    push    word [es:465Eh]

    ; pop
    pop     es
    pop     cx
    POP     ah
    pop     word [ds:bx + di + 0AAAAh]
    popf

    ; conditional jump
    jo      conditional_jmp_label
    jno     conditional_jmp_label
    jb      conditional_jmp_label
    jae     conditional_jmp_label
    je      conditional_jmp_label
    jne     conditional_jmp_label
    jbe     conditional_jmp_label
    ja      conditional_jmp_label
    js      conditional_jmp_label
    jns     conditional_jmp_label
    jp      conditional_jmp_label
    jnp     conditional_jmp_label
    jl      conditional_jmp_label
    jge     conditional_jmp_label
    jle     conditional_jmp_label
    jg      conditional_jmp_label
    conditional_jmp_label:

    ; jmp
    jmp_label:
    jmp     jmp_label                   ; Short direct jump
    jmp     test                        ; Near direct jump
    db      0EAh, 78h, 56h, 34h, 12h    ; Workaround for far direct jump
    jmp     word [cs:0DDFFh]            ; Near indirect jump

    ; loop
    loop_Label:
    loop    loop_label
    loope   loop_label
    loopne  loop_label
    jcxz    loop_label

    ; call
    call    .proc                       ; Near direct call
    db      9Ah, 12h, 34h, 56h, 78h     ; Workaround for far direct call
    call    word [cs:0ABCDh]            ; Near indirect call

    ; ret
    ret     12h
    ret
    retf    0ABCDh

    ; int
    int     3
    int     21h
    int     0FFh

    ; iret
    iret

    ; segment override
    mov     ax, [es:2h]
    sub     bh, [ss:bx + di + 45FEh]
    xchg    cx, [ds:bp - 54h]
    add     dl, [cs:0E54Fh]

    ; other
    lahf
    sahf
    clc
    stc
    cli
    sti
    cld
    std
    xlat
    nop
    stosb
    stosw
    lodsb
    lodsw
    scasb
    scasw

    ; Unknown
    DB      69h

.proc:
    ret

