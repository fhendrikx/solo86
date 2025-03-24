
; monitor

mon_welcome             db 0Ah, 'Solo/86 Release 1.0.0',0Ah,0Ah
                        db ' _____       _         _____   ____ ',0Ah
                        db '/  ___|     | |       |  _  | / ___|',0Ah
                        db '\ `--.  ___ | | ___    \ V / / /___ ',0Ah
                        db ' `--. \/ _ \| |/ _ \   / _ \ | ___ \',0Ah
                        db '/\__/ / (_) | | (_) | | |_| || \_/ |',0Ah
                        db '\____/ \___/|_|\___/  \_____/\_____/',0Ah
                        db 0Ah, '(c) 2025 Dylan Hall / Ferry Hendrikx', 0Ah
                        db 0Ah, 'Build ', __DATE__, 20h, 40h, 20h, __TIME__, 0Ah
                        db 0Ah, 00h


; roms

rom_welcome             db 0Ah, '[ ROM Selection ]', 0Ah, 00h
rom_prompt              db 0Ah, 'ROM$ ', 00h
rom_select              db 0Ah, '[ Select ]', 0Ah, 0Ah, 00h


; debug

dbg_welcome             db 0Ah, '[ MON Debugger ]', 0Ah, 00h
dbg_prompt              db 0Ah, 'MON$ ', 00h
dbg_syntax              db 0Ah, '[ Syntax Error ]', 0Ah, 00h

dbg_cmd_help            db 0Ah, '[ MON Help ]', 0Ah
                        db 'A                   - SYS info', 0Ah

                        db 'D SSSS:OOOO         - MEM DASM addr SSSS:OOOO', 0Ah
                        db 'S                   - MEM DASM type', 0Ah
                        db 'F SSSS:OOOO CC XX   - MEM FILL addr SSSS:OOOO count CC byte XX', 0Ah
                        db 'R SSSS:OOOO         - MEM READ addr SSSS:OOOO', 0Ah

                        db 'I PP                - INP port PP', 0Ah
                        db 'O PP XX             - OUT port PP byte XX', 0Ah
                        db 'J SSSS:OOOO         - JMP addr SSSS:OOOO', 0Ah
                        db 'T SS                - TMR stat', 0Ah
                        db 'X                   - CPU HALT', 0Ah

                        db 'L                   - IHF LOAD', 0Ah
                        db 'E                   - IHF EXEC', 0Ah

                        db 'H                   - HELP', 0Ah
                        db 'Q                   - QUIT', 0Ah
                        db 0Ah, 'All values are given in HEX', 0Ah
                        db 00h

dbg_cmd_dasm            db 0Ah, '[ MEM DASM ]', 0Ah, 00h
dbg_cmd_dasm_intel      db 0Ah, '[ MEM DASM ]: Style: INTEL', 0Ah, 00h
dbg_cmd_dasm_att        db 0Ah, '[ MEM DASM ]: Style: AT&T/GNU', 0Ah, 00h
dbg_cmd_fill            db 0Ah, '[ MEM FILL ]', 0Ah, 00h
dbg_cmd_read            db 0Ah, '[ MEM READ ]', 0Ah, 00h

dbg_cmd_inp             db '[ INP ]: ', 00h
dbg_cmd_out             db '[ OUT ]: ', 00h
dbg_cmd_jump            db 0Ah, '[ JMP ]', 0Ah, 00h

dbg_cmd_timer           db 0Ah, '[ TMR ]: On', 0Ah, 00h
dbg_cmd_timer_off       db 0Ah, '[ TMR ]: Off', 0Ah, 00h

dbg_cmd_load            db 0Ah, '[ IHF LOAD ]', 0Ah, 00h
dbg_cmd_load_loc        db 0Ah, '[ IHF SEGMENT ]: ', 00h
dbg_cmd_load_jmp        db 0Ah, '[ IHF SEG:OFS ]: ', 00h
dbg_cmd_exec            db 0Ah, '[ IHF EXEC ]', 0Ah, 00h
dbg_cmd_exec_err        db 0Ah, '[ IHF EXEC ERR ]', 0Ah, 00h

dbg_cmd_info            db 0Ah, '[ SYS INFO ]', 0Ah, 00h
dbg_cmd_info_exp        db 0Ah, 'Expansion', 00h
dbg_cmd_info_exp_epoch  db 0Ah, '    EPOCH: ', 00h
dbg_cmd_info_exp_uart   db 0Ah, '     UART: ', 00h
dbg_cmd_info_exp_panel  db 0Ah, '    PANEL: ', 00h
dbg_cmd_info_irq        db 0Ah, 'IRQ', 00h
dbg_cmd_info_irq_0      db 0Ah, '    IRQ 0: ', 00h
dbg_cmd_info_irq_1      db 0Ah, '    IRQ 1: ', 00h
dbg_cmd_info_irq_2      db 0Ah, '    IRQ 2: ', 00h
dbg_cmd_info_irq_3      db 0Ah, '    IRQ 3: ', 00h
dbg_cmd_info_rom        db 0Ah, 'ROM', 00h
dbg_cmd_info_rom_found  db 0Ah, '    Found: ', 00h

dbg_cmd_halt            db 0Ah, '[ CPU HALT ]', 0Ah, 00h

