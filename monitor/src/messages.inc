
mesg_welcome        db 0Ah, 0Ah, 'Solo86 Monitor', 0Ah, 00h
mesg_copyright      db 'Copyright (c) 2024 Dylan Hall and Ferry Hendrikx', 0Ah, 00h
mesg_build          db 'Build ', __DATE__, 20h, 40h, 20h, __TIME__, 0Ah, 0Ah, 00h

mesg_prompt         db 0Ah, '> ', 00h
mesg_help           db 0Ah
                    db 'D SSSS:OOOO         - Dump address SSSS:OOOO', 0Ah
                    db 'F SSSS:OOOO CC XX   - Fill address SSSS:OOOO CC x byte XX', 0Ah
                    db 'H                   - Help', 0Ah
                    db 'I PP                - INP port PP', 0Ah
                    db 'J SSSS:OOOO         - Jump address SSSS:OOOO', 0Ah
                    db 'L                   - Load Intel Hex File', 0Ah
                    db 'O PP XX             - OUT port PP byte XX', 0Ah
                    db 'X                   - Jump address (specified in Intel Hex File)', 0Ah
                    db 0Ah, 'All values are given in HEX', 0Ah
                    db 00h

mesg_dump           db 0Ah, '[ Memory Dump ]', 0Ah, 00h
mesg_fill           db 0Ah, '[ Memory Fill ]', 0Ah, 00h

mesg_load           db 0Ah, '[ Load File ]', 0Ah, 00h
mesg_load_done      db 0Ah, '[ Load File Complete ]', 0Ah, 00h

mesg_inp            db 0Ah, '[ INP ]', 0Ah, 00h
mesg_jump           db 0Ah, '[ JMP ]', 0Ah, 00h
mesg_out            db 0Ah, '[ OUT ]', 0Ah, 00h
mesg_exe            db 0Ah, '[ EXE ]', 0Ah, 00h

