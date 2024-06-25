
;======================================================================
; data
;======================================================================

; ROM address
rom_ofs:                    dw 00h
rom_seg:                    dw 00h

; ROM found count
rom_cnt:                    db 000h

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
buffer      times 40        db 000h

; debug address
jmp_ofs:                    dw 00h
jmp_seg:                    dw 00h


; IHF location
ihf_loc:                    dw 00h

; IHF address
ihf_ofs:                    dw 00h
ihf_seg:                    dw 00h


; current instruction

curr_inst               db 000h
curr_mnem               dw 000h
curr_type               db 000h

; addressing
addr_load               db 000h
curr_mod                db 000h
curr_reg                db 000h
curr_rxm                db 000h

; operands
curr_opr1               db 000h
curr_opr2               db 000h

curr_imm                dw 000h
curr_off                dw 000h

; current byte count
curr_cnt                db 000h


