
mesg_welcome        db 0Ah, 0Ah, 'Welcome to Solo86 Monitor', 0Ah, 00h
mesg_welcome_stub   db 'Welcome to Solo86 Stub', 0Ah, 0Ah, 00h

mesg_upload         db '[ Upload file ]', 0Ah, 00h
mesg_upload_done    db '[ Upload complete ]', 0Ah, 00h
mesg_memdump        db '[ Memory dump ]', 0Ah, 00h
mesg_restart        db '[ Restart ]', 0Ah, 0Ah, 00h
mesg_ticks          db '[ Toggling timer ]', 0Ah, 00h

mesg_menu           db 0Ah, '(D) Dump Memory', 0Ah
                    db '(H) Halt', 0Ah
                    db '(R) Run Code', 0Ah
                    db '(T) Toggle Timer', 0Ah
                    db '(U) Upload Intel Hex File', 0Ah, 0Ah
                    db '> ', 00h

mesg_unknown_cmd    db 'Unknown Command', 0Ah, 00h

