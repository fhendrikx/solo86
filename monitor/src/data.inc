
;======================================================================
; data
;======================================================================

; ROM address
rom_ofs:                    dw 00h
rom_seg:                    dw 00h

; ROM found count
rom_cnt:                    db 00h

; ROM found data (8 entries)
rom_table:
%rep rom_max
                            dw 00h  ; segment
                            dw 00h  ; offset
                            dw 00h  ; size
                            dw 00h  ; flags
    TIMES 24                db 00h  ; name (24 bytes)
%endrep


; debug command line buffer
buffer      times 40        db 00h

; debug last command
dbg_last                    db 00h
dbg_ofs:                    dw 00h
dbg_seg:                    dw 00h

; debug address
jmp_ofs:                    dw 00h
jmp_seg:                    dw 00h

; IHF location
ihf_loc:                    dw 00h

; IHF address
ihf_ofs:                    dw 00h
ihf_seg:                    dw 00h

; dasm style
dasm_style                  db 00h


; current instruction

inst_curr                   db 00h  ; inst opcode
inst_mnem                   dw 00h  ; mnemonic
inst_type                   db 00h  ; inst type

; addressing
inst_add                    db 00h  ; addressing loaded?
inst_mod                    db 00h  ; MOD
inst_reg                    db 00h  ; REG
inst_rxm                    db 00h  ; R/M

; operands
inst_op1                    db 00h
inst_op2                    db 00h

inst_imm                    dw 00h  ; immediate
inst_off                    dw 00h  ; offset

; current byte count (used for padding display)
inst_cnt                    db 00h  ; bytes read

