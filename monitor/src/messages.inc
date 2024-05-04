
mesg_welcome        db 0Ah, 0Ah, 'Solo86 Monitor', 0Ah, 00h
mesg_copyright      db 'Copyright (c) 2024 Dylan Hall and Ferry Hendrikx', 0Ah, 00h
mesg_build          db 'Build ', __DATE__, 20h, 40h, 20h, __TIME__, 0Ah, 00h

mesg_prompt         db 0Ah, '> ', 00h
mesg_help           db 0Ah
                    db 'd ssss:oooo - Dump address SSSS:OOOO', 0Ah
                    db 'h           - Help', 0Ah
                    db 'i pp        - INP port PP', 0Ah
                    db 'j ssss:oooo - Jump address SSSS:OOOO', 0Ah, 0Ah
                    db 'l           - Load Intel Hex File', 0Ah
                    db 'o pp xx     - OUT port PP byte XX', 0Ah
                    db 'r ssss:oooo - Dump address SSSS:OOOO', 0Ah
                    db 'x           - Jump address (intel hex file specified)', 0Ah
                    db 0Ah, 'All values specified in HEX', 0Ah, 0Ah
                    db 00h

mesg_dump           db '[ Dump ]', 0Ah, 00h
mesg_load           db '[ Load File ]', 0Ah, 00h
mesg_load_done      db '[ Load File Complete ]', 0Ah, 00h

mesg_inp            db '[ INP ]', 0Ah, 00h
mesg_jump           db '[ JMP ]', 0Ah, 00h
mesg_out            db '[ OUT ]', 0Ah, 00h

