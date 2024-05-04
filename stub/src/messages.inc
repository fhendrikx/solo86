
mesg_welcome        db 0Ah, 0Ah, 'Solo86 Testing Stub', 0Ah, 00h
mesg_copyright      db 'Copyright (c) 2024 Dylan Hall and Ferry Hendrikx', 0Ah, 00h
mesg_build          db 'Build: ', __DATE__, 20h, 40h, 20h, __TIME__, 0Ah, 00h

mesg_memdump        db '[ Memory Dump ]', 0Ah, 00h
mesg_restart        db '[ Restart ]', 0Ah, 0Ah, 00h
mesg_ticks          db '[ Toggling Timer ]', 0Ah, 00h
mesg_pi             db '[ Press ESC to return to menu ]', 0Ah, 00h

mesg_menu           db 0Ah
                    db '(D) Dump Memory', 0Ah
                    db '(H) Halt', 0Ah
                    db '(P) PiUART Interrupt Test', 0Ah
                    db '(T) Toggle Timer', 0Ah
                    db '> ', 00h

mesg_unknown_cmd    db 'Unknown Command', 0Ah, 00h

