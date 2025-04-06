
; monitor

mon_welcome         db 0Ah, 'Solo/86 Release 1.0.0',0Ah,0Ah
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

rom_welcome         db 0Ah, '[ ROM Selection ]', 0Ah, 00h
rom_prompt          db 0Ah, 'ROM$ ', 00h
rom_select          db 0Ah, '[ Select ]', 0Ah, 0Ah, 00h


; debug

dbg_welcome         db 0Ah, '[ MON Debugger ]', 0Ah, 00h
dbg_prompt          db 0Ah, 'MON$ ', 00h
dbg_syntax          db 0Ah, '[ Syntax Error ]', 0Ah, 00h

dbg_help            db 0Ah, '[ MON Help ]', 0Ah
                    db 'A                   - SYS info', 0Ah

                    db 'D SSSS:OOOO         - DISassemble addr SSSS:OOOO', 0Ah
                    db 'S                   - DISassemble style', 0Ah
                    db 'F SSSS:OOOO CC XX   - MEM FILL addr SSSS:OOOO count CC byte XX', 0Ah
                    db 'R SSSS:OOOO         - MEM READ addr SSSS:OOOO', 0Ah

                    db 'I PP                - INP port PP', 0Ah
                    db 'O PP XX             - OUT port PP byte XX', 0Ah
                    db 'J SSSS:OOOO         - JMP addr SSSS:OOOO', 0Ah
                    db 'Y SS                - TMR state', 0Ah
                    db 'T HH:MM:SS          - CLK time set', 0Ah
                    db 'C YYYY-MM-DD        - CLK date set', 0Ah
                    db 'X                   - CPU HALT', 0Ah

                    db 'L                   - IHF LOAD', 0Ah
                    db 'E                   - IHF EXEC', 0Ah

                    db 'H                   - HELP', 0Ah
                    db 'Q                   - QUIT', 0Ah
                    db 0Ah, 'All values are given in HEX', 0Ah
                    db 00h

dbg_dasm            db '[ MEM DASM ]', 0Ah, 00h
dbg_dasm_intel      db '[ MEM DASM ] Style: INTEL', 0Ah, 00h
dbg_dasm_att        db '[ MEM DASM ] Style: AT&T/GNU', 0Ah, 00h
dbg_fill            db '[ MEM FILL ]', 0Ah, 00h
dbg_read            db '[ MEM READ ]', 0Ah, 00h

dbg_inp             db '[ INP ] Port: ', 00h
dbg_out             db '[ OUT ] Port: ', 00h
dbg_jump            db '[ JMP ]', 0Ah, 00h

dbg_timer           db '[ TMR ] State: On', 0Ah, 00h
dbg_timer_off       db '[ TMR ] State: Off', 0Ah, 00h

dbg_load            db '[ IHF LOAD ]', 0Ah, 00h
dbg_load_addr       db '[ IHF LOAD ] Address: ', 00h
dbg_load_err        db '[ IHF LOAD ERR ]', 0Ah, 00h
dbg_exec            db '[ IHF EXEC ]', 0Ah, 00h
dbg_exec_err        db '[ IHF EXEC ERR ]', 0Ah, 00h

dbg_info            db '[ SYS INFO ]', 0Ah, 00h
dbg_info_sys        db 'System', 0Ah, 00h
dbg_info_sys_bnk    db '  Banking: ', 00h
dbg_info_sys_pit    db '      PIT: ', 00h
dbg_info_sys_rtc    db '      RTC: ', 00h
dbg_info_sys_com    db '      COM: ', 00h
dbg_info_sys_pnl    db '    PANEL: ', 00h
dbg_info_irq        db 'IRQ', 0Ah, 00h
dbg_info_irq_0      db '    IRQ 0: ', 00h
dbg_info_irq_1      db '    IRQ 1: ', 00h
dbg_info_irq_2      db '    IRQ 2: ', 00h
dbg_info_irq_3      db '    IRQ 3: ', 00h
dbg_info_rtc        db 'RTC', 0Ah, 00h
dbg_info_rtc_date   db '     Date: ', 00h
dbg_info_rtc_time   db '     Time: ', 00h
dbg_info_rom        db 'ROM', 0Ah, 00h
dbg_info_rom_found  db '    Found: ', 00h

dbg_halt            db '[ CPU HALT ]', 0Ah, 00h

