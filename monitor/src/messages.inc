
; monitor
mon_welcome         db 0Ah, 0Ah, 'Solo86 Monitor', 0Ah, 00h
mon_copyright       db '(c) 2024 Dylan Hall and Ferry Hendrikx', 0Ah, 00h
mon_build           db 'Build ', __DATE__, 20h, 40h, 20h, __TIME__, 0Ah, 0Ah, 00h

; roms
rom_welcome         db 0Ah, '[ ROM Menu ]', 0Ah, 0Ah, 00h
rom_prompt          db 0Ah, '> ', 00h
rom_select          db 0Ah, '[ ROM Select ]', 0Ah, 0Ah, 00h

; debug
dbg_welcome         db 0Ah, '[ DBG Menu ]', 0Ah, 0Ah, 00h
dbg_prompt          db 0Ah, '> ', 00h
dbg_syntax          db 0Ah, '[ DBG Syntax ]', 0Ah, 0Ah, 00h

dbg_cmd_help        db 0Ah, '[ DBG Help ]', 0Ah
                    db 'D SSSS:OOOO         - DUMP address SSSS:OOOO', 0Ah
                    db 'F SSSS:OOOO CC XX   - FILL address SSSS:OOOO CC x byte XX', 0Ah
                    db 'H                   - HELP', 0Ah
                    db 'I PP                -  INP port PP', 0Ah
                    db 'J SSSS:OOOO         - JUMP address SSSS:OOOO', 0Ah
                    db 'L                   - LOAD Intel Hex File', 0Ah
                    db 'O PP XX             -  OUT port PP byte XX', 0Ah
                    db 'R SSSS:OOOO         - READ address SSSS:OOOO', 0Ah
                    db 'X                   - JUMP address (specified in Intel Hex File)', 0Ah
                    db 0Ah, 'All values are given in HEX', 0Ah
                    db 00h

dbg_cmd_dump        db 0Ah, '[ DBG MEM DUMP ]', 0Ah, 00h
dbg_cmd_fill        db 0Ah, '[ DBG MEM FILL ]', 0Ah, 00h
dbg_cmd_load        db 0Ah, '[ DBG IHF LOAD ]', 0Ah, 00h
dbg_cmd_load_done   db 0Ah, '[ DBG IHF DONE ]', 0Ah, 00h
dbg_cmd_inp         db 0Ah, '[ DBG INP ]', 0Ah, 00h
dbg_cmd_jump        db 0Ah, '[ DBG JMP ]', 0Ah, 00h
dbg_cmd_out         db 0Ah, '[ DBG OUT ]', 0Ah, 00h
dbg_cmd_read        db 0Ah, '[ DBG MEM READ ]', 0Ah, 00h
dbg_cmd_exe         db 0Ah, '[ DBG EXE ]', 0Ah, 00h

