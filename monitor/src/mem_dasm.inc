;======================================================================
; Ferry Hendrikx, June 2024
;======================================================================
;
; Format of instructions:
;
;======================================================================
;byte   thing           state
;======================================================================
;0      prefix          optional
;1      opcode          mandatory
;2      addr mode       optional
;
;3      disp/addr low   optional
;4      disp/addr high  optional
;5      data low        optional
;6      data high       optional
;
;
; Detailed breakdown of opcode and addr mode
;
;           7 6 5 4 3 2 1 0
;           ===============
;opcode     OPCODE      D W
;
;           OPCODE gives the operation
;
;                       D=direction (1=from R/M operand to REG operand,
;                                    0=from REG operand to R/M operand)
;
;                       C=rotate    (1=CL used, 0=1 or immediate)
;
;                       S=sign      (1=8-bit immediate to be extended,
;                                    0=16-bit immediate to be extended)
;
;                         W=word/byte (1=16-bit, 0=8-bit)
;
;           7 6 5 4 3 2 1 0
;           ===============
;addr mode  MOD REG   R/M
;
;           MOD (determines how R/M is interpreted; 2 bits)
;
;           00=R/M Table 1 for R/M operand
;           01=R/M Table 2 with  8-bit displacement
;           10=R/M Table 2 with 16-bit displacement
;           11=Two registers; use REG table
;
;               REGister/Segment REGister/Opcode extension (3 bits)
;
;               REG Table
;                   W=0 W=1      W=0 W=1
;               000=AL  AX   100=AH  SP
;               001=CL  CX   101=CH  BP
;               010=DL  DX   110=DH  SI
;               011=BL  BX   111=BH  DI
;
;               SREG Table (segment register)
;               000=ES       010=SS
;               001=CS       110=DS
;
;                     R/M (register/memory; 3 bits)
;
;                     R/M Table 1 (MOD=00)
;                     000=[BX+SI]  010=[BP+SI]  100=[SI]  110=DIRECT (2 operands)
;                     001=[BX+DI]  011=[BP+DI]  101=[DI]  111=[BX]
;
;                     R/M Table 2 (MOD=01|10) - Add DISP8/16 to register:
;                     000=[BX+SI]  010=[BP+SI]  100=[SI]  110=[BP]
;                     001=[BX+DI]  011=[BP+DI]  101=[DI]  111=[BX]
;
;           Addressing mode 00 (using R/M table 1)
;               opcode, addrmode
;               opcode, addrmode, offset low, offset high
;
;           Addressing mode 01 (using R/M table 2 with  8-bit displacement)
;               opcode, addrmode, disp8
;
;           Addressing mode 10 (using R/M table 2 with 16-bit displacement)
;               opcode, addrmode, disp16 (disp low, disp high)
;
;           Addressing mode 11 (using REG table)
;               two operand reg-reg


; MOV AX,BX
; 89 D8
; 10001001, 11 011 000 (W=1 16-bit, D=0 move from REG to R/M)
;       DW  11 BX  AX
;
; MOV BX,AX
; 89 C3
; 10001001, 11 000 011 (W=1 16-bit, D=0 move from REG to R/M)
;       DW  11 AX  BX
;
; MOV [BX+010h],CL
; 88 4F 10
; 10001000, 01 001 111, 00010000 (W=0 8-bit, D=0 move from REG to R/M)
;       DW  01 CL  [BX] + 10h
;
; MOV BX,address
; BB DE 01
; 10111011, addr low, addr high
;
; XOR AH,AH
; 30 E4
; 00110000, 11 100 100
;       DW  11 AH  AH (W=0 8-bit, D=0 move from REG to R/M)
;
; XOR DX,DX
; 31 D2
; 00110001, 11 010 010
;       DW  11 DX  DX (W=1 16-bit, D=0 move from REG to R/M)
;
; ADD BX,2
; 83 C3 02
; 10000011, 11 000 011, 00000010 (W=1 16-bit, S=8-bit imm extend to 16-bit)
;       SW  11 ADD BX   2h
;
; CALL DX
; FFD2
; 11111111, 11 010 010
;       SW  11 DX  DX

;======================================================================
; memory dasm; disassemble memory content at DS:SI for CX instructions
; returns offset in BX
;======================================================================

mem_dasm:
    push ax
    push cx
    push ds
    push si

    call clear

; disassemble an instruction at DS:SI

.loop:

    call print_addr         ; print seg:offset


; decode instruction

    call load_inst          ; load instruction

    cmp byte [cs:inst_type],ins_type_unknown
    je .exit

    cmp byte [cs:inst_type],ins_type_special
    je .special

    cmp byte [cs:inst_type],ins_type_ext
    jne .decode

    call load_extended      ; load extended instruction
    jmp .decode

.special:
    call load_byte          ; skip special weird extra byte


; decode operands

.decode:
    cmp byte [cs:inst_op1],ins_op_r8
    jb .operands            ; addressing byte only for op_r8 upwards

    ; addr already loaded?
    cmp byte [cs:inst_add],1
    je .operands

    call load_addr          ; load addressing

.operands:
    ; operand 1

    mov dl,[cs:inst_op1]
    cmp dl,ins_op_none
    je .print

    call load_opr

    ; operand 2

    mov dl,[cs:inst_op2]
    cmp dl,ins_op_none
    je .print

    call load_opr


.print:
    ; print padding

    call print_spaces

    ; print instruction and operands

    call print_inst
    call print_operands
    call print_end

    call clear

    loop .loop

    mov bx,si               ; return SI in BX

.exit:
    pop si
    pop ds
    pop cx
    pop ax
    ret


; load byte
;
load_byte:
    xor ax,ax
    lodsb

    push ax

    call print_hex

    ; keep count
    mov al,byte [cs:inst_cnt]
    add al,2
    mov byte [cs:inst_cnt],al

    pop ax

    ret


; load word
;
load_word:
    lodsw

    push ax

    call print_word

    ; keep count
    mov al,byte [cs:inst_cnt]
    add al,4
    mov byte [cs:inst_cnt],al

    pop ax

    ret


; load instruction byte
;
load_inst:
    push bx
    push dx

    call load_byte

    ; instruction
    mov [cs:inst_curr],al

    mov dl,5                ; size of inst
    mul dl
    mov bx,inst_table
    add bx,ax               ; offset

    mov ax,[cs:bx + 0]      ; mnemonic
    mov [cs:inst_mnem],ax

    mov al,[cs:bx + 2]      ; type
    mov [cs:inst_type],al

    ; operands
    mov al,[cs:bx + 3]      ; operand 1
    mov [cs:inst_op1],al
    mov al,[cs:bx + 4]      ; operand 2
    mov [cs:inst_op2],al

    pop dx
    pop bx
    ret


; load addressing byte
;
load_addr:
    push ax
    push cx
    push dx

    call load_byte

    mov dl,al

    mov cl,6                ; MOD
    shr al,cl
    mov [cs:inst_mod],al

    mov al,dl               ; REG / Ext. Opcode
    and al,038h
    shr al,1
    shr al,1
    shr al,1
    mov [cs:inst_reg],al

    mov al,dl               ; R/M
    and al,7h
    mov [cs:inst_rxm],al

    mov byte [cs:inst_add],1

    pop dx
    pop cx
    pop ax
    ret


; load extended instruction
;
load_extended:
    push ax
    push bx
    push dx

    call load_addr          ; load addr

    ; inst_reg is actually the extended inst
    xor ax,ax
    mov al,byte [cs:inst_reg]


    ; TODO


    shl al,1                ; word sized

    ; points to extended inst, shift into list
    mov bx,[cs:inst_mnem]
    add bx,ax

    mov dx,[cs:bx]
    mov [cs:inst_mnem],dx

    pop dx
    pop bx
    pop ax
    ret


; load operand (given in DL)
;
load_opr:
    push ax
    push dx

    ; determine how to handle operand
    cmp dl,ins_op_rm8
    jae .rm
    cmp dl,ins_op_r8
    jae .reg
    cmp dl,ins_op_far
    je .far
    cmp dl,ins_op_imm16
    jae .imm16
    cmp dl,ins_op_imm8
    jae .imm8
    jmp .exit

    ; R/M
.rm:
    ; 2 registers
    cmp byte [cs:inst_mod],3
    je .reg

    ; R/M table 2: 16-bit displacement
    cmp byte [cs:inst_mod],2
    je .disp16

    ; R/M table 2;  8-bit displacement
    cmp byte [cs:inst_mod],1
    je .disp8

    ; R/M table 1; check for direct operands
    cmp byte [cs:inst_rxm],6
    je .disp16
    jmp .exit

    ; REG
.reg:
    jmp .exit

    ; displacement 16-bit
.disp16:
    call load_word
    mov [cs:inst_off],ax
    jmp .exit

    ; displacement 8-bit
.disp8:
    call load_byte
    mov [cs:inst_off],ax
    jmp .exit

    ; FAR
.far:
    call load_word
    mov [cs:inst_imm],ax

    call load_word
    mov [cs:inst_off],ax
    jmp .exit

    ; IMM16
.imm16:
    call load_word
    mov [cs:inst_imm],ax
    jmp .exit

    ; IMM8
.imm8:
    call load_byte
    mov [cs:inst_imm],ax
    jmp .exit

.exit:
    pop dx
    pop ax
    ret


; print_addr
;
print_addr:
    mov ax,ds
    call print_word         ; seg

    mov al,':'
    call print_chr

    mov ax,si
    call print_word         ; offset

    mov al,' '
    call print_chr

    ret


; print spaces (add variable number of spaces)
;
print_spaces:
    push ax
    push cx

    xor cx,cx

    ; how many spaces are we short?
    mov al,byte [cs:inst_cnt]
    mov cl,16
    sub cl,al

.loop:
    mov al,20h
    call print_chr
    loop .loop

    pop cx
    pop ax
    ret


; print instruction
;
print_inst:
    push si
    mov si,[cs:inst_mnem]
    call print_str          ; inst

    mov al,20h
    call print_chr          ; space
    pop si
    ret


; print operands
;
print_operands:
    cmp byte [cs:dasm_style],0
    jne .attstyle

    ; Intel style
    mov dl,[cs:inst_op1]
    cmp dl,ins_op_none
    je .exit
    call print_opr          ; operand 1

    mov dl,[cs:inst_op2]
    cmp dl,ins_op_none
    je .exit

    mov al,','
    call print_chr

    call print_opr          ; operand 2
    jmp .exit

    ; AT&T style
.attstyle:
    mov dl,[cs:inst_op2]
    cmp dl,ins_op_none
    je .next
    call print_opr          ; operand 2

    mov al,','
    call print_chr

.next:
    mov dl,[cs:inst_op1]
    cmp dl,ins_op_none
    je .exit

    call print_opr          ; operand 1

.exit:
    ret


; print operand (given in DL)
;
print_opr:
    push ax
    push bx
    push dx

    ; determine how to handle operand
    cmp dl,ins_op_none
    je .exit                ; none

    cmp dl,ins_op_ds
    jbe .constreg

    cmp dl,ins_op_const1
    je .const1
    cmp dl,ins_op_const3
    je .const3

    cmp dl,ins_op_imm8
    je .imm8
    cmp dl,ins_op_eimm8
    je .imm16

    cmp dl,ins_op_short
    je .short
    cmp dl,ins_op_imm16
    je .imm16
    cmp dl,ins_op_near
    je .near
    cmp dl,ins_op_mem
    je .mem

    cmp dl,ins_op_far
    je .far

    cmp dl,ins_op_sreg
    jbe .reg

    ; addressing
    cmp byte [cs:inst_mod],3
    jne .addr

    xor bx,bx
    mov bl,byte [cs:inst_rxm]

    cmp dl,ins_op_rm8
    je .rmx
    add bl,8

    ; R/M
.rmx:
    cmp byte [cs:dasm_style],0
    je .rmxint

    mov al,'%'
    call print_chr

.rmxint:
    shl bl,1

    push si
    mov si,[cs:regs_table + bx]
    call print_str
    pop si

    jmp .exit

    ; addressing
.addr:
    ; TODO


    jmp .exit

    ; sreg
.constreg:
    cmp byte [cs:dasm_style],0
    je .constregint

    mov al,'%'
    call print_chr

.constregint:
    xor bx,bx
    mov bl,dl
    shl bl,1

    push si
    mov si,[cs:regs_table + bx]
    call print_str
    pop si
    jmp .exit

    ; const 1
.const1:
    mov al,'1'
    call print_chr
    jmp .exit

    ; const 3
.const3:
    mov al,'3'
    call print_chr
    jmp .exit

    ; immediate 8-bit
.imm8:
    mov ax,[cs:inst_imm]
    call print_hex
    jmp .exit

    ; short
.short:
    mov ax,[cs:inst_imm]
    cbw
    mov dx,si
    add ax,dx
    call print_word
    jmp .exit

    ; immediate 16-bit
.imm16:
    mov ax,[cs:inst_imm]
    call print_word
    jmp .exit

    ; near
.near:
    mov ax,[cs:inst_imm]
    mov dx,si
    add ax,dx
    inc ax
    call print_word
    jmp .exit

    ; mem
.mem:
    mov al,'['
    call print_chr

    mov ax,[cs:inst_off]
    call print_word

    mov al,']'
    call print_chr
    jmp .exit

    ; FAR
.far:
    mov ax,[cs:inst_off]
    call print_word

    mov al,':'
    call print_chr

    mov ax,[cs:inst_imm]
    call print_word
    jmp .exit

    ; REG
.reg:
    xor bx,bx
    mov bl,byte [cs:inst_reg]

    cmp dl,ins_op_r8
    je .reg_name

    add bl,8

    cmp dl,ins_op_r16
    je .reg_name

    add bl,8

.reg_name:
    cmp byte [cs:dasm_style],0
    je .intel

    mov al,'%'
    call print_chr

.intel:
    shl bl,1

    push si
    mov si,[cs:regs_table + bx]
    call print_str
    pop si

    jmp .exit

.exit:
    pop dx
    pop bx
    pop ax
    ret


; print end
;
print_end:
    mov al,0Ah
    call print_chr
    ret


; clear (reset current instruction)
;
clear:
    ; instruction
    mov byte [cs:inst_curr],0
    mov word [cs:inst_mnem],0
    mov byte [cs:inst_type],0

    ; addressing
    mov byte [cs:inst_add],0
    mov byte [cs:inst_mod],0
    mov byte [cs:inst_reg],0
    mov byte [cs:inst_rxm],0

    ; operands
    mov byte [cs:inst_op1],0
    mov byte [cs:inst_op2],0

    mov word [cs:inst_imm],0
    mov word [cs:inst_off],0

    ; byte count
    mov byte [cs:inst_cnt],0

    ret


;======================================================================
; table opcodes
;======================================================================

; instructions

ins ins_unknown,        'unknown'
ins ins_aaa,            'aaa'
ins ins_aad,            'aad'
ins ins_aam,            'aam'
ins ins_aas,            'aas'
ins ins_adc,            'adc'
ins ins_add,            'add'
ins ins_and,            'and'
ins ins_call,           'call'
ins ins_cbw,            'cbw'
ins ins_clc,            'clc'
ins ins_cld,            'cld'
ins ins_cli,            'cli'
ins ins_cmc,            'cmc'
ins ins_cmp,            'cmp'
ins ins_cmpsb,          'cmpsb'
ins ins_cmpsw,          'cmpsw'
ins ins_cwd,            'cwd'
ins ins_daa,            'daa'
ins ins_das,            'das'
ins ins_dec,            'dec'
ins ins_div,            'div'
ins ins_hlt,            'hlt'
ins ins_idiv,           'idiv'
ins ins_imul,           'imul'
ins ins_in,             'in'
ins ins_inc,            'inc'
ins ins_int,            'int'
ins ins_int3,           'int3'
ins ins_into,           'into'
ins ins_iret,           'iret'
ins ins_ja,             'ja'
ins ins_jae,            'jae'
ins ins_jb,             'jb'
ins ins_jbe,            'jbe'
ins ins_jc,             'jc'
ins ins_jcxz,           'jcxz'
ins ins_je,             'je'
ins ins_jg,             'jg'
ins ins_jge,            'jge'
ins ins_jl,             'jl'
ins ins_jle,            'jle'
ins ins_jmp,            'jmp'
ins ins_jna,            'jna'
ins ins_jnc,            'jnc'
ins ins_jne,            'jne'
ins ins_jng,            'jng'
ins ins_jnl,            'jnl'
ins ins_jno,            'jno'
ins ins_jnp,            'jnp'
ins ins_jns,            'jns'
ins ins_jo,             'jo'
ins ins_jp,             'jp'
ins ins_jpe,            'jpe'
ins ins_jpo,            'jpo'
ins ins_js,             'js'
ins ins_lahf,           'lahf'
ins ins_lds,            'lds'
ins ins_lea,            'lea'
ins ins_les,            'les'
ins ins_lock,           'lock'
ins ins_lodsb,          'lodsb'
ins ins_lodsw,          'lodsw'
ins ins_loop,           'loop'
ins ins_loope,          'loope'
ins ins_loopne,         'loopne'
ins ins_mov,            'mov'
ins ins_movsb,          'movsb'
ins ins_movsw,          'movsw'
ins ins_mul,            'mul'
ins ins_neg,            'neg'
ins ins_nop,            'nop'
ins ins_not,            'not'
ins ins_or,             'or'
ins ins_out,            'out'
ins ins_pop,            'pop'
ins ins_popf,           'popf'
ins ins_push,           'push'
ins ins_pushf,          'pushf'
ins ins_rcl,            'rcl'
ins ins_rcr,            'rcr'
ins ins_rep,            'rep'
ins ins_repne,          'repne'
ins ins_ret,            'ret'
ins ins_retf,           'retf'
ins ins_rol,            'rol'
ins ins_ror,            'ror'
ins ins_sahf,           'sahf'
ins ins_sar,            'sar'
ins ins_sbb,            'sbb'
ins ins_scasb,          'scasb'
ins ins_scasw,          'scasw'
ins ins_shl,            'shl'
ins ins_shr,            'shr'
ins ins_stc,            'stc'
ins ins_std,            'std'
ins ins_sti,            'sti'
ins ins_stosb,          'stosb'
ins ins_stosw,          'stosw'
ins ins_sub,            'sub'
ins ins_test,           'test'
ins ins_wait,           'wait'
ins ins_xchg,           'xchg'
ins ins_xlat,           'xlat'
ins ins_xlatb,          'xlatb'
ins ins_xor,            'xor'

ins ins_segover_es,     'seg es:'
ins ins_segover_cs,     'seg cs:'
ins ins_segover_ss,     'seg ss:'
ins ins_segover_ds,     'seg ds:'


; instructions (extended)

ext ins_ext_80_83,      ins_add,  ins_or,  ins_adc,  ins_sbb,  ins_and,  ins_sub,  ins_xor,  ins_cmp
ext ins_ext_d0_d3,      ins_rol,  ins_ror, ins_rcl,  ins_rcr,  ins_shl,  ins_shr,  ins_nop,  ins_sar
ext ins_ext_f6_f7,      ins_test, ins_nop, ins_not,  ins_neg,  ins_mul,  ins_imul, ins_div,  ins_idiv
ext ins_ext_fe_ff,      ins_inc,  ins_dec, ins_call, ins_call, ins_jmp,  ins_jmp,  ins_push, ins_nop


; instruction types

ins_type_unknown        equ 000h
ins_type_normal         equ 001h
ins_type_ext            equ 002h
ins_type_segover        equ 003h
ins_type_prefix         equ 004h
ins_type_special        equ 005h


; instruction operands

ins_op_none             equ 000h

ins_op_al               equ 001h        ; 8-bit registers
ins_op_cl               equ 002h
ins_op_dl               equ 003h
ins_op_bl               equ 004h
ins_op_ah               equ 005h
ins_op_ch               equ 006h
ins_op_dh               equ 007h
ins_op_bh               equ 008h

ins_op_ax               equ 008h        ; 16-bit registers
ins_op_cx               equ 009h
ins_op_dx               equ 00ah
ins_op_bx               equ 00bh
ins_op_sp               equ 00ch
ins_op_bp               equ 00dh
ins_op_si               equ 00eh
ins_op_di               equ 00fh

ins_op_es               equ 010h        ; segment registers
ins_op_cs               equ 011h
ins_op_ss               equ 012h
ins_op_ds               equ 013h

ins_op_const1           equ 01ah        ; constant values
ins_op_const3           equ 01bh

ins_op_imm8             equ 020h        ; 8-bit immediate
ins_op_eimm8            equ 021h
ins_op_short            equ 022h

ins_op_imm16            equ 023h        ; 16-bit immediate
ins_op_near             equ 024h
ins_op_mem              equ 025h

ins_op_far              equ 026h        ; 32-bit

ins_op_r8               equ 030h        ; MOD R/M
ins_op_r16              equ 031h
ins_op_sreg             equ 032h
ins_op_rm8              equ 033h
ins_op_rm16             equ 034h


; instruction table

inst_table:

inst ins_add,           ins_type_normal,    ins_op_rm8,     ins_op_r8       ; 00h
inst ins_add,           ins_type_normal,    ins_op_rm16,    ins_op_r16      ; 01h
inst ins_add,           ins_type_normal,    ins_op_r8,      ins_op_rm8      ; 02h
inst ins_add,           ins_type_normal,    ins_op_r16,     ins_op_rm16     ; 03h
inst ins_add,           ins_type_normal,    ins_op_al,      ins_op_imm8     ; 04h
inst ins_add,           ins_type_normal,    ins_op_ax,      ins_op_imm16    ; 05h
inst ins_push,          ins_type_normal,    ins_op_es,      ins_op_none     ; 06h
inst ins_pop,           ins_type_normal,    ins_op_es,      ins_op_none     ; 07h
inst ins_or,            ins_type_normal,    ins_op_rm8,     ins_op_r8       ; 08h
inst ins_or,            ins_type_normal,    ins_op_rm16,    ins_op_r16      ; 09h
inst ins_or,            ins_type_normal,    ins_op_r8,      ins_op_rm8      ; 0ah
inst ins_or,            ins_type_normal,    ins_op_r16,     ins_op_rm16     ; 0bh
inst ins_or,            ins_type_normal,    ins_op_al,      ins_op_imm8     ; 0ch
inst ins_or,            ins_type_normal,    ins_op_ax,      ins_op_imm16    ; 0dh
inst ins_push,          ins_type_normal,    ins_op_cs,      ins_op_none     ; 0eh
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 0fh
inst ins_adc,           ins_type_normal,    ins_op_rm8,     ins_op_r8       ; 10h
inst ins_adc,           ins_type_normal,    ins_op_rm16,    ins_op_r16      ; 11h
inst ins_adc,           ins_type_normal,    ins_op_r8,      ins_op_rm8      ; 12h
inst ins_adc,           ins_type_normal,    ins_op_r16,     ins_op_rm16     ; 13h
inst ins_adc,           ins_type_normal,    ins_op_al,      ins_op_imm8     ; 14h
inst ins_adc,           ins_type_normal,    ins_op_ax,      ins_op_imm16    ; 15h
inst ins_push,          ins_type_normal,    ins_op_ss,      ins_op_none     ; 16h
inst ins_pop,           ins_type_normal,    ins_op_ss,      ins_op_none     ; 17h
inst ins_sbb,           ins_type_normal,    ins_op_rm8,     ins_op_r8       ; 18h
inst ins_sbb,           ins_type_normal,    ins_op_rm16,    ins_op_r16      ; 19h
inst ins_sbb,           ins_type_normal,    ins_op_r8,      ins_op_rm8      ; 1ah
inst ins_sbb,           ins_type_normal,    ins_op_r16,     ins_op_rm16     ; 1bh
inst ins_sbb,           ins_type_normal,    ins_op_al,      ins_op_imm8     ; 1ch
inst ins_sbb,           ins_type_normal,    ins_op_ax,      ins_op_imm16    ; 1dh
inst ins_push,          ins_type_normal,    ins_op_ds,      ins_op_none     ; 1eh
inst ins_pop,           ins_type_normal,    ins_op_ds,      ins_op_none     ; 1fh
inst ins_and,           ins_type_normal,    ins_op_rm8,     ins_op_r8       ; 20h
inst ins_and,           ins_type_normal,    ins_op_rm16,    ins_op_r16      ; 21h
inst ins_and,           ins_type_normal,    ins_op_r8,      ins_op_rm8      ; 22h
inst ins_and,           ins_type_normal,    ins_op_r16,     ins_op_rm16     ; 23h
inst ins_and,           ins_type_normal,    ins_op_al,      ins_op_imm8     ; 24h
inst ins_and,           ins_type_normal,    ins_op_ax,      ins_op_imm16    ; 25h
inst ins_segover_es,    ins_type_segover,   ins_op_none,    ins_op_none     ; 26h
inst ins_daa,           ins_type_normal,    ins_op_none,    ins_op_none     ; 27h
inst ins_sub,           ins_type_normal,    ins_op_rm8,     ins_op_r8       ; 28h
inst ins_sub,           ins_type_normal,    ins_op_rm16,    ins_op_r16      ; 29h
inst ins_sub,           ins_type_normal,    ins_op_r8,      ins_op_rm8      ; 2ah
inst ins_sub,           ins_type_normal,    ins_op_r16,     ins_op_rm16     ; 2bh
inst ins_sub,           ins_type_normal,    ins_op_al,      ins_op_imm8     ; 2ch
inst ins_sub,           ins_type_normal,    ins_op_ax,      ins_op_imm16    ; 2dh
inst ins_segover_cs,    ins_type_segover,   ins_op_none,    ins_op_none     ; 2eh
inst ins_das,           ins_type_normal,    ins_op_none,    ins_op_none     ; 2fh
inst ins_xor,           ins_type_normal,    ins_op_rm8,     ins_op_r8       ; 30h
inst ins_xor,           ins_type_normal,    ins_op_rm16,    ins_op_r16      ; 31h
inst ins_xor,           ins_type_normal,    ins_op_r8,      ins_op_rm8      ; 32h
inst ins_xor,           ins_type_normal,    ins_op_r16,     ins_op_rm16     ; 33h
inst ins_xor,           ins_type_normal,    ins_op_al,      ins_op_imm8     ; 34h
inst ins_xor,           ins_type_normal,    ins_op_ax,      ins_op_imm16    ; 35h
inst ins_segover_ss,    ins_type_segover,   ins_op_none,    ins_op_none     ; 36h
inst ins_aaa,           ins_type_normal,    ins_op_none,    ins_op_none     ; 37h
inst ins_cmp,           ins_type_normal,    ins_op_rm8,     ins_op_r8       ; 38h
inst ins_cmp,           ins_type_normal,    ins_op_rm16,    ins_op_r16      ; 39h
inst ins_cmp,           ins_type_normal,    ins_op_r8,      ins_op_rm8      ; 3ah
inst ins_cmp,           ins_type_normal,    ins_op_r16,     ins_op_rm16     ; 3bh
inst ins_cmp,           ins_type_normal,    ins_op_al,      ins_op_imm8     ; 3ch
inst ins_cmp,           ins_type_normal,    ins_op_ax,      ins_op_imm16    ; 3dh
inst ins_segover_ds,    ins_type_segover,   ins_op_none,    ins_op_none     ; 3eh
inst ins_aas,           ins_type_normal,    ins_op_none,    ins_op_none     ; 3fh
inst ins_inc,           ins_type_normal,    ins_op_ax,      ins_op_none     ; 40h
inst ins_inc,           ins_type_normal,    ins_op_cx,      ins_op_none     ; 41h
inst ins_inc,           ins_type_normal,    ins_op_dx,      ins_op_none     ; 42h
inst ins_inc,           ins_type_normal,    ins_op_bx,      ins_op_none     ; 43h
inst ins_inc,           ins_type_normal,    ins_op_sp,      ins_op_none     ; 44h
inst ins_inc,           ins_type_normal,    ins_op_bp,      ins_op_none     ; 45h
inst ins_inc,           ins_type_normal,    ins_op_si,      ins_op_none     ; 46h
inst ins_inc,           ins_type_normal,    ins_op_di,      ins_op_none     ; 47h
inst ins_dec,           ins_type_normal,    ins_op_ax,      ins_op_none     ; 48h
inst ins_dec,           ins_type_normal,    ins_op_cx,      ins_op_none     ; 49h
inst ins_dec,           ins_type_normal,    ins_op_dx,      ins_op_none     ; 4ah
inst ins_dec,           ins_type_normal,    ins_op_bx,      ins_op_none     ; 4bh
inst ins_dec,           ins_type_normal,    ins_op_sp,      ins_op_none     ; 4ch
inst ins_dec,           ins_type_normal,    ins_op_bp,      ins_op_none     ; 4dh
inst ins_dec,           ins_type_normal,    ins_op_si,      ins_op_none     ; 4eh
inst ins_dec,           ins_type_normal,    ins_op_di,      ins_op_none     ; 4fh
inst ins_push,          ins_type_normal,    ins_op_ax,      ins_op_none     ; 50h
inst ins_push,          ins_type_normal,    ins_op_cx,      ins_op_none     ; 51h
inst ins_push,          ins_type_normal,    ins_op_dx,      ins_op_none     ; 52h
inst ins_push,          ins_type_normal,    ins_op_bx,      ins_op_none     ; 53h
inst ins_push,          ins_type_normal,    ins_op_sp,      ins_op_none     ; 54h
inst ins_push,          ins_type_normal,    ins_op_bp,      ins_op_none     ; 55h
inst ins_push,          ins_type_normal,    ins_op_si,      ins_op_none     ; 56h
inst ins_push,          ins_type_normal,    ins_op_di,      ins_op_none     ; 57h
inst ins_pop,           ins_type_normal,    ins_op_ax,      ins_op_none     ; 58h
inst ins_pop,           ins_type_normal,    ins_op_cx,      ins_op_none     ; 59h
inst ins_pop,           ins_type_normal,    ins_op_dx,      ins_op_none     ; 5ah
inst ins_pop,           ins_type_normal,    ins_op_bx,      ins_op_none     ; 5bh
inst ins_pop,           ins_type_normal,    ins_op_sp,      ins_op_none     ; 5ch
inst ins_pop,           ins_type_normal,    ins_op_bp,      ins_op_none     ; 5dh
inst ins_pop,           ins_type_normal,    ins_op_si,      ins_op_none     ; 5eh
inst ins_pop,           ins_type_normal,    ins_op_di,      ins_op_none     ; 5fh
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 60h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 61h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 62h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 63h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 64h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 65h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 66h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 67h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 68h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 69h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 6ah
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 6bh
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 6ch
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 6dh
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 6eh
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; 6fh
inst ins_jo,            ins_type_normal,    ins_op_short,   ins_op_none     ; 70h
inst ins_jno,           ins_type_normal,    ins_op_short,   ins_op_none     ; 71h
inst ins_jb,            ins_type_normal,    ins_op_short,   ins_op_none     ; 72h
inst ins_jae,           ins_type_normal,    ins_op_short,   ins_op_none     ; 73h
inst ins_je,            ins_type_normal,    ins_op_short,   ins_op_none     ; 74h
inst ins_jne,           ins_type_normal,    ins_op_short,   ins_op_none     ; 75h
inst ins_jbe,           ins_type_normal,    ins_op_short,   ins_op_none     ; 76h
inst ins_ja,            ins_type_normal,    ins_op_short,   ins_op_none     ; 77h
inst ins_js,            ins_type_normal,    ins_op_short,   ins_op_none     ; 78h
inst ins_jns,           ins_type_normal,    ins_op_short,   ins_op_none     ; 79h
inst ins_jp,            ins_type_normal,    ins_op_short,   ins_op_none     ; 7ah
inst ins_jnp,           ins_type_normal,    ins_op_short,   ins_op_none     ; 7bh
inst ins_jl,            ins_type_normal,    ins_op_short,   ins_op_none     ; 7ch
inst ins_jge,           ins_type_normal,    ins_op_short,   ins_op_none     ; 7dh
inst ins_jle,           ins_type_normal,    ins_op_short,   ins_op_none     ; 7eh
inst ins_jg,            ins_type_normal,    ins_op_short,   ins_op_none     ; 7fh
inst ins_ext_80_83,     ins_type_ext,       ins_op_rm8,     ins_op_imm8     ; 80h
inst ins_ext_80_83,     ins_type_ext,       ins_op_rm16,    ins_op_imm16    ; 81h
inst ins_ext_80_83,     ins_type_ext,       ins_op_rm8,     ins_op_imm8     ; 82h
inst ins_ext_80_83,     ins_type_ext,       ins_op_rm16,    ins_op_eimm8    ; 83h
inst ins_test,          ins_type_normal,    ins_op_r8,      ins_op_rm8      ; 84h
inst ins_test,          ins_type_normal,    ins_op_r16,     ins_op_rm16     ; 85h
inst ins_xchg,          ins_type_normal,    ins_op_r8,      ins_op_rm8      ; 86h
inst ins_xchg,          ins_type_normal,    ins_op_r16,     ins_op_rm16     ; 87h
inst ins_mov,           ins_type_normal,    ins_op_rm8,     ins_op_r8       ; 88h
inst ins_mov,           ins_type_normal,    ins_op_rm16,    ins_op_r16      ; 89h
inst ins_mov,           ins_type_normal,    ins_op_r8,      ins_op_rm8      ; 8ah
inst ins_mov,           ins_type_normal,    ins_op_r16,     ins_op_rm16     ; 8bh
inst ins_mov,           ins_type_normal,    ins_op_rm16,    ins_op_sreg     ; 8ch
inst ins_lea,           ins_type_normal,    ins_op_r16,     ins_op_rm16     ; 8dh
inst ins_mov,           ins_type_normal,    ins_op_sreg,    ins_op_rm16     ; 8eh
inst ins_pop,           ins_type_normal,    ins_op_rm16,    ins_op_none     ; 8fh
inst ins_xchg,          ins_type_normal,    ins_op_ax,      ins_op_ax       ; 90h
inst ins_xchg,          ins_type_normal,    ins_op_cx,      ins_op_ax       ; 91h
inst ins_xchg,          ins_type_normal,    ins_op_dx,      ins_op_ax       ; 92h
inst ins_xchg,          ins_type_normal,    ins_op_bx,      ins_op_ax       ; 93h
inst ins_xchg,          ins_type_normal,    ins_op_sp,      ins_op_ax       ; 94h
inst ins_xchg,          ins_type_normal,    ins_op_bp,      ins_op_ax       ; 95h
inst ins_xchg,          ins_type_normal,    ins_op_si,      ins_op_ax       ; 96h
inst ins_xchg,          ins_type_normal,    ins_op_di,      ins_op_ax       ; 97h
inst ins_cbw,           ins_type_normal,    ins_op_none,    ins_op_none     ; 98h
inst ins_cwd,           ins_type_normal,    ins_op_none,    ins_op_none     ; 99h
inst ins_call,          ins_type_normal,    ins_op_far,     ins_op_none     ; 9ah
inst ins_wait,          ins_type_normal,    ins_op_none,    ins_op_none     ; 9bh
inst ins_pushf,         ins_type_normal,    ins_op_none,    ins_op_none     ; 9ch
inst ins_popf,          ins_type_normal,    ins_op_none,    ins_op_none     ; 9dh
inst ins_sahf,          ins_type_normal,    ins_op_none,    ins_op_none     ; 9eh
inst ins_lahf,          ins_type_normal,    ins_op_none,    ins_op_none     ; 9fh
inst ins_mov,           ins_type_normal,    ins_op_al,      ins_op_mem      ; a0h
inst ins_mov,           ins_type_normal,    ins_op_ax,      ins_op_mem      ; a1h
inst ins_mov,           ins_type_normal,    ins_op_mem,     ins_op_al       ; a2h
inst ins_mov,           ins_type_normal,    ins_op_mem,     ins_op_ax       ; a3h
inst ins_movsb,         ins_type_normal,    ins_op_none,    ins_op_none     ; a4h
inst ins_movsw,         ins_type_normal,    ins_op_none,    ins_op_none     ; a5h
inst ins_cmpsb,         ins_type_normal,    ins_op_none,    ins_op_none     ; a6h
inst ins_cmpsw,         ins_type_normal,    ins_op_none,    ins_op_none     ; a7h
inst ins_test,          ins_type_normal,    ins_op_al,      ins_op_imm8     ; a8h
inst ins_test,          ins_type_normal,    ins_op_ax,      ins_op_imm16    ; a9h
inst ins_stosb,         ins_type_normal,    ins_op_none,    ins_op_none     ; aah
inst ins_stosw,         ins_type_normal,    ins_op_none,    ins_op_none     ; abh
inst ins_lodsb,         ins_type_normal,    ins_op_none,    ins_op_none     ; ach
inst ins_lodsw,         ins_type_normal,    ins_op_none,    ins_op_none     ; adh
inst ins_scasb,         ins_type_normal,    ins_op_none,    ins_op_none     ; aeh
inst ins_scasw,         ins_type_normal,    ins_op_none,    ins_op_none     ; afh
inst ins_mov,           ins_type_normal,    ins_op_al,      ins_op_imm8     ; b0h
inst ins_mov,           ins_type_normal,    ins_op_cl,      ins_op_imm8     ; b1h
inst ins_mov,           ins_type_normal,    ins_op_dl,      ins_op_imm8     ; b2h
inst ins_mov,           ins_type_normal,    ins_op_bl,      ins_op_imm8     ; b3h
inst ins_mov,           ins_type_normal,    ins_op_ah,      ins_op_imm8     ; b4h
inst ins_mov,           ins_type_normal,    ins_op_ch,      ins_op_imm8     ; b5h
inst ins_mov,           ins_type_normal,    ins_op_dh,      ins_op_imm8     ; b6h
inst ins_mov,           ins_type_normal,    ins_op_bh,      ins_op_imm8     ; b7h
inst ins_mov,           ins_type_normal,    ins_op_ax,      ins_op_imm16    ; b8h
inst ins_mov,           ins_type_normal,    ins_op_cx,      ins_op_imm16    ; b9h
inst ins_mov,           ins_type_normal,    ins_op_dx,      ins_op_imm16    ; bah
inst ins_mov,           ins_type_normal,    ins_op_bx,      ins_op_imm16    ; bbh
inst ins_mov,           ins_type_normal,    ins_op_sp,      ins_op_imm16    ; bch
inst ins_mov,           ins_type_normal,    ins_op_bp,      ins_op_imm16    ; bdh
inst ins_mov,           ins_type_normal,    ins_op_si,      ins_op_imm16    ; beh
inst ins_mov,           ins_type_normal,    ins_op_di,      ins_op_imm16    ; bfh
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; c0h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; c1h
inst ins_ret,           ins_type_normal,    ins_op_imm16,   ins_op_none     ; c2h
inst ins_ret,           ins_type_normal,    ins_op_none,    ins_op_none     ; c3h
inst ins_les,           ins_type_normal,    ins_op_r16,     ins_op_rm16     ; c4h
inst ins_lds,           ins_type_normal,    ins_op_r16,     ins_op_rm16     ; c5h
inst ins_mov,           ins_type_normal,    ins_op_rm8,     ins_op_imm8     ; c6h
inst ins_mov,           ins_type_normal,    ins_op_rm16,    ins_op_imm16    ; c7h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; c8h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; c9h
inst ins_retf,          ins_type_normal,    ins_op_imm16,   ins_op_none     ; cah
inst ins_retf,          ins_type_normal,    ins_op_none,    ins_op_none     ; cbh
inst ins_int,           ins_type_normal,    ins_op_const3,  ins_op_none     ; cch
inst ins_int,           ins_type_normal,    ins_op_imm8,    ins_op_none     ; cdh
inst ins_into,          ins_type_normal,    ins_op_none,    ins_op_none     ; ceh
inst ins_iret,          ins_type_normal,    ins_op_none,    ins_op_none     ; cfh
inst ins_ext_d0_d3,     ins_type_ext,       ins_op_rm8,     ins_op_const1   ; d0h
inst ins_ext_d0_d3,     ins_type_ext,       ins_op_rm16,    ins_op_const1   ; d1h
inst ins_ext_d0_d3,     ins_type_ext,       ins_op_rm8,     ins_op_cl       ; d2h
inst ins_ext_d0_d3,     ins_type_ext,       ins_op_rm16,    ins_op_cl       ; d3h
inst ins_aam,           ins_type_special,   ins_op_none,    ins_op_none     ; d4h
inst ins_aad,           ins_type_special,   ins_op_none,    ins_op_none     ; d5h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; d6h
inst ins_xlat,          ins_type_normal,    ins_op_none,    ins_op_none     ; d7h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; d8h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; d9h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; dah
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; dbh
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; dch
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; ddh
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; deh
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; dfh
inst ins_loopne,        ins_type_normal,    ins_op_short,   ins_op_none     ; e0h
inst ins_loope,         ins_type_normal,    ins_op_short,   ins_op_none     ; e1h
inst ins_loop,          ins_type_normal,    ins_op_short,   ins_op_none     ; e2h
inst ins_jcxz,          ins_type_normal,    ins_op_short,   ins_op_none     ; e3h
inst ins_in,            ins_type_normal,    ins_op_al,      ins_op_imm8     ; e4h
inst ins_in,            ins_type_normal,    ins_op_ax,      ins_op_imm16    ; e5h
inst ins_out,           ins_type_normal,    ins_op_imm8,    ins_op_al       ; e6h
inst ins_out,           ins_type_normal,    ins_op_imm8,    ins_op_ax       ; e7h
inst ins_call,          ins_type_normal,    ins_op_near,    ins_op_none     ; e8h
inst ins_jmp,           ins_type_normal,    ins_op_near,    ins_op_none     ; e9h
inst ins_jmp,           ins_type_normal,    ins_op_far,     ins_op_none     ; eah
inst ins_jmp,           ins_type_normal,    ins_op_short,   ins_op_none     ; ebh
inst ins_in,            ins_type_normal,    ins_op_al,      ins_op_dx       ; ech
inst ins_in,            ins_type_normal,    ins_op_ax,      ins_op_dx       ; edh
inst ins_out,           ins_type_normal,    ins_op_dx,      ins_op_al       ; eeh
inst ins_out,           ins_type_normal,    ins_op_dx,      ins_op_ax       ; efh
inst ins_lock,          ins_type_prefix,    ins_op_none,    ins_op_none     ; f0h
inst ins_unknown,       ins_type_unknown,   ins_op_none,    ins_op_none     ; f1h
inst ins_repne,         ins_type_prefix,    ins_op_none,    ins_op_none     ; f2h
inst ins_rep,           ins_type_prefix,    ins_op_none,    ins_op_none     ; f3h
inst ins_hlt,           ins_type_normal,    ins_op_none,    ins_op_none     ; f4h
inst ins_cmc,           ins_type_normal,    ins_op_none,    ins_op_none     ; f5h
inst ins_ext_f6_f7,     ins_type_ext,       ins_op_rm8,     ins_op_none     ; f6h
inst ins_ext_f6_f7,     ins_type_ext,       ins_op_rm16,    ins_op_none     ; f7h
inst ins_clc,           ins_type_normal,    ins_op_none,    ins_op_none     ; f8h
inst ins_stc,           ins_type_normal,    ins_op_none,    ins_op_none     ; f9h
inst ins_cli,           ins_type_normal,    ins_op_none,    ins_op_none     ; fah
inst ins_sti,           ins_type_normal,    ins_op_none,    ins_op_none     ; fbh
inst ins_cld,           ins_type_normal,    ins_op_none,    ins_op_none     ; fch
inst ins_std,           ins_type_normal,    ins_op_none,    ins_op_none     ; fdh
inst ins_ext_fe_ff,     ins_type_ext,       ins_op_rm8,     ins_op_none     ; feh
inst ins_ext_fe_ff,     ins_type_ext,       ins_op_rm16,    ins_op_none     ; ffh


; registers and address offsets

reg reg_al,             'al'
reg reg_cl,             'cl'
reg reg_dl,             'dl'
reg reg_bl,             'bl'
reg reg_ah,             'ah'
reg reg_ch,             'ch'
reg reg_dh,             'dh'
reg reg_bh,             'bh'

reg reg_ax,             'ax'
reg reg_cx,             'cx'
reg reg_dx,             'dx'
reg reg_bx,             'bx'
reg reg_sp,             'sp'
reg reg_bp,             'bp'
reg reg_si,             'si'
reg reg_di,             'di'

reg reg_es,             'es'
reg reg_cs,             'cs'
reg reg_ss,             'ss'
reg reg_ds,             'ds'

reg add_bx_si,          'bx+si'
reg add_bx_di,          'bx+di'
reg add_bp_si,          'bp+si'
reg add_bp_di,          'bp+di'
reg add_si,             'si'
reg add_di,             'di'
reg add_bp,             'bp'
reg add_bx,             'bx'


regs_table:

                        dw reg_al, reg_cl, reg_dl, reg_bl, reg_ah, reg_ch, reg_dh, reg_bh
                        dw reg_ax, reg_cx, reg_dx, reg_bx, reg_sp, reg_bp, reg_si, reg_di
                        dw reg_es, reg_cs, reg_ss, reg_ds

addr_table:

                        dw add_bx_si,   add_bx_di,  add_bp_si,  add_bp_di
                        dw add_si,      add_di,     add_bp,     add_bx

