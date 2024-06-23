;=========================================================================
; print
;=========================================================================

%imacro print 1
    push si
    mov si,%1
    call print_str
    pop si
%endm


;=========================================================================
; disasm
;=========================================================================

%imacro reg 2
%1: db %2, 000h
%endm

%imacro ins 2
%1: db %2, 000h
%endm

%imacro ext 9
%1: dw %2
    dw %3
    dw %4
    dw %5
    dw %6
    dw %7
    dw %8
    dw %9
%endm

%imacro inst 4
    dw %1               ; mnemonic
    db %2               ; type
    db %3               ; operand 1
    db %4               ; operand 2
%endm

