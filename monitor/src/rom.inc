

returns list of roms


    mov dx,8000h
    mov bx,F000h

.scan:
    mov es,dx
es  cmp word[0],0AA55h
    jnz .next



.next:
    add dx,4000h
    cmp dx,bx
    jb .scan

.exit:
    ret

