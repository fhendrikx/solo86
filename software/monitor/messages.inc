
; monitor

mon_welcome         db 0Ah
                    db ' _____       _          _______   ____ ',0Ah
                    db '/  ___|     | |        / /  _  | / ___|',0Ah
                    db '\ `--.  ___ | | ___   / / \ V / / /___ ',0Ah
                    db ' `--. \/ _ \| |/ _ \ / /  / _ \ | ___ \',0Ah
                    db '/\__/ / (_) | | (_) / /  | |_| || \_/ |',0Ah
                    db '\____/ \___/|_|\___/_/   \_____/\_____/',0Ah,0Ah
                    db '[ (c) 2025 Dylan Hall / Ferry Hendrikx ]',0Ah,00h


; roms

rom_welcome         db 0Ah, '[ ROM Selection ]', 0Ah, 00h
rom_prompt          db 0Ah, 'ROM$ ', 00h
rom_select          db 0Ah, '[ Select ]', 0Ah, 0Ah, 00h


; exceptions

exc_00              db 0Ah, '[ EXCEPTION 00 ] Divide by zero', 0Ah, 00h
exc_01              db 0Ah, '[ EXCEPTION 01 ] Single step', 0Ah, 00h
exc_02              db 0Ah, '[ EXCEPTION 02 ] NMI', 0Ah, 00h
exc_03              db 0Ah, '[ EXCEPTION 03 ] Breakpoint', 0Ah, 00h
exc_04              db 0Ah, '[ EXCEPTION 04 ] INTO overflow', 0Ah, 00h
exc_05              db 0Ah, '[ EXCEPTION 05 ] BOUND range', 0Ah, 00h
exc_06              db 0Ah, '[ EXCEPTION 06 ] Invalid opcode', 0Ah, 00h
exc_07              db 0Ah, '[ EXCEPTION 07 ] Extension not available', 0Ah, 00h
exc_08              db 0Ah, '[ EXCEPTION 08 ] Interrupt table too small', 0Ah, 00h
exc_09              db 0Ah, '[ EXCEPTION 09 ] Segment overrun', 0Ah, 00h
exc_0A              db 0Ah, '[ EXCEPTION 0A ] Reserved', 0Ah, 00h
exc_0B              db 0Ah, '[ EXCEPTION 0B ] Reserved', 0Ah, 00h
exc_0C              db 0Ah, '[ EXCEPTION 0C ] Reserved', 0Ah, 00h
exc_0D              db 0Ah, '[ EXCEPTION 0D ] Segment overrun', 0Ah, 00h
exc_0E              db 0Ah, '[ EXCEPTION 0E ] Reserved', 0Ah, 00h
exc_0F              db 0Ah, '[ EXCEPTION 0F ] Reserved', 0Ah, 00h
exc_10              db 0Ah, '[ EXCEPTION 10 ] Extension error', 0Ah, 00h
exc_11              db 0Ah, '[ EXCEPTION 11 ] Reserved', 0Ah, 00h
exc_12              db 0Ah, '[ EXCEPTION 12 ] Reserved', 0Ah, 00h
exc_13              db 0Ah, '[ EXCEPTION 13 ] Reserved', 0Ah, 00h
exc_14              db 0Ah, '[ EXCEPTION 14 ] Reserved', 0Ah, 00h
exc_15              db 0Ah, '[ EXCEPTION 15 ] Reserved', 0Ah, 00h
exc_16              db 0Ah, '[ EXCEPTION 16 ] Reserved', 0Ah, 00h
exc_17              db 0Ah, '[ EXCEPTION 17 ] Reserved', 0Ah, 00h
exc_18              db 0Ah, '[ EXCEPTION 18 ] Reserved', 0Ah, 00h
exc_19              db 0Ah, '[ EXCEPTION 19 ] Reserved', 0Ah, 00h
exc_1A              db 0Ah, '[ EXCEPTION 1A ] Reserved', 0Ah, 00h
exc_1B              db 0Ah, '[ EXCEPTION 1B ] Reserved', 0Ah, 00h
exc_1C              db 0Ah, '[ EXCEPTION 1C ] Reserved', 0Ah, 00h
exc_1D              db 0Ah, '[ EXCEPTION 1D ] Reserved', 0Ah, 00h
exc_1E              db 0Ah, '[ EXCEPTION 1E ] Reserved', 0Ah, 00h
exc_1F              db 0Ah, '[ EXCEPTION 1F ] Reserved', 0Ah, 00h


; debug

dbg_welcome         db 0Ah, '[ MON Debugger ]', 0Ah, 00h
dbg_prompt          db 0Ah, 'MON$ ', 00h
dbg_syntax          db 0Ah, '[ Syntax Error ]', 0Ah, 00h

dbg_help            db 0Ah, '[ MON Help ]', 0Ah
                    db 'D SSSS:OOOO         - MEM DUMP addr SSSS:OOOO', 0Ah
                    db 'F SSSS:OOOO CC XX   - MEM FILL addr SSSS:OOOO count CC byte XX', 0Ah
                    db 'R SSSS CC           - MEM RMAP ROM segment SSSS (+ count CC) as RAM', 0Ah
                    db 'S                   - MEM UASM style', 0Ah
                    db 'U SSSS:OOOO         - MEM UASM addr SSSS:OOOO', 0Ah

                    db 'I PP                - CPU INP port PP', 0Ah
                    db 'O PP XX             - CPU OUT port PP byte XX', 0Ah
                    db 'J SSSS:OOOO         - CPU JMP addr SSSS:OOOO', 0Ah
                    db 'Y SS                - CPU TMR state', 0Ah
                    db 'X                   - CPU HLT', 0Ah
                    db 'Z                   - CPU SHUTDOWN (generate triple fault)', 0Ah

                    db 't HH:MM:SS          - CLK time set', 0Ah
                    db 'd YYYY-MM-DD        - CLK date set', 0Ah

                    db 'c                   - ATA device detect', 0Ah
                    db 'i SSSS:OOOO         - ATA device identify', 0Ah
                    db 'r                   - ATA device reset', 0Ah

                    db 'L                   - IHX LOAD', 0Ah
                    db 'E                   - IHX EXEC', 0Ah

                    db 'A                   - SYS INFO', 0Ah

                    db 'K                   - ECHO', 0Ah

                    db 'h                   - Help', 0Ah
                    db 'v                   - Version', 0Ah
                    db 'Q                   - Quit', 0Ah
                    db 0Ah, 'All values are given in HEX', 0Ah
                    db 00h

dbg_dump            db '[ MEM DUMP ]', 0Ah, 00h
dbg_fill            db '[ MEM FILL ]', 0Ah, 00h
dbg_remap           db '[ MEM REMAP ] Segment: ', 00h
dbg_unasm           db '[ MEM UNASM ]', 0Ah, 00h
dbg_unasm_att       db '[ MEM UNASM ] Style: AT&T/GNU', 0Ah, 00h
dbg_unasm_intel     db '[ MEM UNASM ] Style: INTEL', 0Ah, 00h

dbg_inp             db '[ CPU INP ] Port: ', 00h
dbg_out             db '[ CPU OUT ] Port: ', 00h
dbg_jump            db '[ CPU JMP ]', 0Ah, 00h
dbg_timer           db '[ CPU TMR ] State: On', 0Ah, 00h
dbg_timer_off       db '[ CPU TMR ] State: Off', 0Ah, 00h
dbg_halt            db '[ CPU HLT ]', 0Ah, 00h
dbg_shutdown        db '[ CPU SHUTDOWN ]', 0Ah, 00h

dbg_detect          db '[ ATA DETECT ] Type: ', 00h
dbg_ident           db '[ ATA IDENT ]', 0Ah, 00h
dbg_ident_nodev     db '[ ATA IDENT No Device ]', 0Ah, 00h
dbg_ident_err       db '[ ATA IDENT ERR ]', 0Ah, 00h
dbg_reset           db '[ ATA RESET ]', 0Ah, 00h

dbg_load            db '[ IHX LOAD ]', 0Ah, 00h
dbg_load_addr       db '[ IHX LOAD ] Segment: ', 00h
dbg_load_err        db '[ IHX LOAD ERR ]', 0Ah, 00h
dbg_exec            db '[ IHX EXEC ]', 0Ah, 00h
dbg_exec_err        db '[ IHX EXEC ERR ]', 0Ah, 00h

dbg_echo            db '[ ECHO ]', 0Ah, 00h

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

