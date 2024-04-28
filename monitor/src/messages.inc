
mesg_welcome        db 0Ah, 0Ah, 'Welcome to Solo86 Monitor', 0Ah, 00h
mesg_welcome_stub   db 'Welcome to Solo86 Stub', 0Ah, 0Ah, 00h

mesg_upload         db '[ Upload file ]', 0Ah, 00h
mesg_upload_done    db '[ Upload complete ]', 0Ah, 00h
mesg_memdump        db '[ Memory dump ]', 0Ah, 00h
mesg_restart        db '[ Restart ]', 0Ah, 0Ah, 00h

mesg_menu:
mesg_menu1          db 0Ah, '(D) Dump Memory', 0Ah
mesg_menu2          db '(H) Halt', 0Ah
mesg_menu3          db '(R) Run Code', 0Ah
mesg_menu4          db '(U) Upload Intel Hex File', 0Ah, 0Ah
mesg_menu5          db '> ', 00h

mesg_unknown_cmd    db 'Unknown Command', 0Ah, 00h

