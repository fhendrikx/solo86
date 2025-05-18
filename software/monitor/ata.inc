
%imacro wait_not_busy 0
    mov dx,ATA_STATUS
%%busy:
    in al,dx
    test al,080h
    jnz %%busy
%endm

%imacro wait_drq_err 0
    mov dx,ATA_STATUS
%%wait:
    in al,dx
    test al,1               ; ERR
    jnz .error
    test al,8               ; DRQ
    jz %%wait
%endm

%imacro check_err 0
    mov dx,ATA_STATUS
    in al,dx
    test al,1               ; ERR
    jnz .error
%endm


;=========================================================================
; ata_init
; Input:
;   None
; Output:
;   None
;-------------------------------------------------------------------------
ata_init:
    ret


;=========================================================================
; ata_reset
; Input:
;   None
; Output:
;   None
;-------------------------------------------------------------------------
ata_reset:
    push ax
    push dx

    ; control register; set nIEN and SRST
    xor al,al
    or al,00000110b
    mov dx,ATA_CTRL
    out dx,al

    call delay_10us

    ; control register; set nIEN
    xor al,al
    or al,00000010b
    mov dx,ATA_CTRL
    out dx,al

    call delay_10us

    pop dx
    pop ax
    ret


;=========================================================================
; ata_detect
; Input:
;   None
; Output:
;   AX - device type
;-------------------------------------------------------------------------
ata_detect:
    push bx
    push cx
    push dx

    wait_not_busy

    mov dx,ATA_DRVH
    mov al,0A0h             ; master A0, slave B0
    out dx,al

    wait_not_busy

    ; signature
    mov dx,ATA_LBA_MD
    in al,dx
    mov cl,al

    ; signature
    mov dx,ATA_LBA_HI
    in al,dx
    mov ch,al

    ; 0000: PATA
    ; 3cc3: SATA
    ; 14EB: PATAPI
    ; 6996: SATAPI

    mov ax,cx

.exit:
    pop dx
    pop cx
    pop bx
    ret


;=========================================================================
; ata_ident
; Input:
;   ES:DI - buffer
; Output:
;   AX - 0 - OK, 1 - no device, 2 - error
;-------------------------------------------------------------------------
ata_ident:
    push bx
    push cx
    push dx

    cld

    wait_not_busy

    mov dx,ATA_DRVH
    mov al,0A0h             ; master A0, slave B0
    out dx,al

    wait_not_busy

    mov dx,ATA_FEAT
    xor al,al
    out dx,al

    mov dx,ATA_COUNT
    out dx,al

    mov dx,ATA_LBA_LO
    out dx,al

    mov dx,ATA_LBA_MD
    out dx,al

    mov dx,ATA_LBA_HI
    out dx,al

    ; command
    mov dx,ATA_CMND
    mov al,0ECh
    out dx,al

    wait_not_busy
    wait_drq_err

    ; read sector into buffer at ES:DI
    mov dx,ATA_DATA
    mov cx,256              ; 256 words

.read:
    in ax,dx
    xchg al,ah              ; should this be here?
    stosw
    loop .read

    xor ax,ax
    jmp .exit

.error:
    mov ax,1

.exit:
    pop dx
    pop cx
    pop bx
    ret


;=========================================================================
; ata_read
; Input:
;   AX - LBA (00-15)
;   BX - LBA (16-27)
;   CL - count
;   ES:DI - buffer to write to
; Output:
;   1 sector in the buffer
;-------------------------------------------------------------------------
ata_read:
    push bx
    push cx
    push dx

    cld

    push ax
    push bx

    wait_not_busy

    ; LBA 24-27 (and drive setting)
    mov dx,ATA_DRVH
    shr bx,8
    or bl,11100000b
    mov al,bl
    out dx,al

    wait_not_busy

    ; error
    mov dx,ATA_FEAT
    xor al,al
    out dx,al

    ; count
    mov dx,ATA_COUNT
    mov al,1
    out dx,al

    pop bx
    pop ax

    ; LBA 00-07
    mov dx,ATA_LBA_LO
    out dx,al

    ; LBA 08-15
    mov dx,ATA_LBA_MD
    xchg al,ah
    out dx,al

    ; LBA 16-23
    mov dx,ATA_LBA_HI
    mov ax,bx
    out dx,al

    ; command
    mov dx,ATA_CMND
    mov al,020h
    out dx,al

    wait_not_busy
    wait_drq_err

    ; read sector into buffer at ES:DI
    mov dx,ATA_DATA
    mov cx,256              ; 256 words

.read:
    in ax,dx
    stosw
    loop .read

    xor ax,ax
    jmp .exit

.error:
    mov ax,1

.exit:
    pop dx
    pop cx
    pop bx
    ret


;=========================================================================
; ata_write
; Input:
;   AX - LBA (00-15)
;   BX - LBA (16-27)
;   DS:SI - buffer to read from
; Output:
;   1 sector written from buffer
;-------------------------------------------------------------------------
ata_write:
    push bx
    push cx
    push dx

    cld

    push ax
    push bx

    wait_not_busy

    ; LBA 24-27 (and drive setting)
    mov dx,ATA_DRVH
    shr bx,8
    or bl,11100000b
    mov al,bl
    out dx,al

    wait_not_busy

    ; error
    mov dx,ATA_FEAT
    xor al,al
    out dx,al

    ; count
    mov dx,ATA_COUNT
    mov al,1
    out dx,al

    pop bx
    pop ax

    ; LBA 00-07
    mov dx,ATA_LBA_LO
    out dx,al

    ; LBA 08-15
    mov dx,ATA_LBA_MD
    xchg al,ah
    out dx,al

    ; LBA 16-23
    mov dx,ATA_LBA_HI
    mov ax,bx
    out dx,al

    ; command
    mov dx,ATA_CMND
    mov al,030h
    out dx,al

    wait_not_busy
    wait_drq_err

    ; write sector from buffer at DS:SI
    mov dx,ATA_DATA
    mov cx,256              ; 256 words

.write:
    lodsw
    out dx,ax
    loop .write

    wait_not_busy
    check_err

    xor ax,ax
    jmp .exit

.error:
    mov ax,1

.exit:
    pop dx
    pop cx
    pop bx
    ret

