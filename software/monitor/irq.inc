
;----------------------------------------------------------------------
; IRQ handlers
;----------------------------------------------------------------------

int_irq00:
    push ax
    mov ax,[ cs:irq0_cnt ]
    inc ax
    mov [ cs:irq0_cnt ],ax
    out INT_EOI,al
    pop ax
    iret

int_irq01:
    push ax
    mov ax,[ cs:irq1_cnt ]
    inc ax
    mov [ cs:irq1_cnt ],ax
    out INT_EOI,al
    pop ax
    iret

int_irq02:
    push ax
    mov ax,[ cs:irq2_cnt ]
    inc ax
    mov [ cs:irq2_cnt ],ax
    out INT_EOI,al
    pop ax
    iret

int_irq03:
    push ax
    mov ax,[ cs:irq3_cnt ]
    inc ax
    mov [ cs:irq3_cnt ],ax
    out INT_EOI,al
    pop ax
    iret

