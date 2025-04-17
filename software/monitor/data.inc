
;======================================================================
; data
;======================================================================

; hardware detected (01=BNK,02=PIT,04=RES,08=RES,RTC=10,COM=20,RES=40,PNL=80)
hardware:                   dw 00h

; IRQ counters
irq0_cnt:                   dw 00h
irq1_cnt:                   dw 00h
irq2_cnt:                   dw 00h
irq3_cnt:                   dw 00h

; ROM address
rom_ofs:                    dw 00h
rom_seg:                    dw 00h

; ROM found count
rom_cnt:                    db 00h

; ROM found data (8 entries)
rom_table:
%rep ROM_CNT
                            dw 00h  ; src segment
                            dw 00h  ; src offset
                            dw 00h  ; size
                            dw 00h  ; dst segment
            TIMES 24        db 00h  ; name (24 bytes)
%endrep


; debug command line buffer
buffer      TIMES 64        db 00h

; debug last command
dbg_last                    db 00h
dbg_ofs:                    dw 00h
dbg_seg:                    dw 00h

; debug address
jmp_ofs:                    dw 00h
jmp_seg:                    dw 00h

; IHX address
ihx_ofs:                    dw 00h
ihx_seg:                    dw 00h

; IHX segment location
ihx_loc:                    dw 00h

; IHX checksum correct?
ihx_crc:                    db 00h

; unasm style?
unasm_style                 db 00h


; current instruction

inst_curr                   db 00h  ; inst opcode
inst_mnem                   dw 00h  ; mnemonic
inst_type                   db 00h  ; inst type

; addressing
inst_add                    db 00h  ; addressing loaded?
inst_mod                    db 00h  ; MOD
inst_reg                    db 00h  ; REG
inst_rxm                    db 00h  ; R/M
inst_ovr                    db 00h  ; override

; operands
inst_op1                    db 00h
inst_op2                    db 00h

inst_imm                    dw 00h  ; immediate
inst_off                    dw 00h  ; offset

; current byte count (used for padding display)
inst_cnt                    db 00h  ; bytes read

