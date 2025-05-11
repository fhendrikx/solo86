
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
; ata_wait
; Input:
;   None
; Output:
;   None
;-------------------------------------------------------------------------
ata_wait:
    push ax
    push dx
    mov dx,ATA_CMND

.busy:
    in al,dx
    test al,080h
    jnz .busy

    pop dx
    pop ax
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

    mov dx,ATA_DRV_HEAD
    mov al,0A0h             ; master A0, slave B0
    out dx,al

    call ata_wait

    ; signature
    mov dx,ATA_CYL_LO
    in al,dx
    mov cl,al

    ; signature
    mov dx,ATA_CYL_HI
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

    call ata_wait

    mov dx,ATA_ERROR
    xor al,al
    out dx,al

    mov dx,ATA_SEC_COUNT
    out dx,al

    mov dx,ATA_SEC_NUM
    out dx,al

    mov dx,ATA_CYL_LO
    out dx,al

    mov dx,ATA_CYL_HI
    out dx,al

    mov dx,ATA_DRV_HEAD
    mov al,0A0h             ; master A0, slave B0
    out dx,al

    ; command
    mov dx,ATA_CMND
    mov al,0ECh
    out dx,al

    call ata_wait

    mov dx,ATA_CMND
.wait:
    in al,dx
    test al,1               ; err
    jnz .error
    test al,8               ; drq
    jz .wait

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
;   Data in buffer
;-------------------------------------------------------------------------
ata_read:
    push bx
    push cx
    push dx

    cld

    push ax

    call ata_wait

    ; error
    mov dx,ATA_ERROR
    xor al,al
    out dx,al

    ; count
    mov dx,ATA_SEC_COUNT
    mov al,cl
    out dx,al

    pop ax

    ; LBA 00-07
    mov dx,ATA_SEC_NUM
    out dx,al

    ; LBA 08-15
    mov dx,ATA_CYL_LO
    xchg al,ah
    out dx,al

    ; LBA 16-23
    mov dx,ATA_CYL_HI
    mov ax,bx
    out dx,al

    ; LBA 24-27 (and drive setting)
    mov dx,ATA_DRV_HEAD
    shr bx,8
    or bl,11100000b
    mov al,bl
    out dx,al

    ; command
    mov dx,ATA_CMND
    mov al,020h
    out dx,al

    call ata_wait

    mov dx,ATA_CMND
.wait:
    in al,dx
    test al,1               ; err
    jnz .error
    test al,8               ; drq
    jz .wait

    ; sector count loop
    xor ch,ch
    mov bx,cx

    ; read sector into buffer at ES:DI
    mov dx,ATA_DATA

.start:
    mov cx,256              ; 256 words

.read:
    in ax,dx
    stosw
    loop .read

    dec bx
    jnz .start

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
;   CL - count
;   DS:SI - buffer to read from
; Output:
;   None
;-------------------------------------------------------------------------
ata_write:
    push bx
    push cx
    push dx

    cld

    push ax

    call ata_wait

    ; error
    mov dx,ATA_ERROR
    xor al,al
    out dx,al

    ; count
    mov dx,ATA_SEC_COUNT
    mov al,cl
    out dx,al

    pop ax

    ; LBA 00-07
    mov dx,ATA_SEC_NUM
    out dx,al

    ; LBA 08-15
    mov dx,ATA_CYL_LO
    xchg al,ah
    out dx,al

    ; LBA 16-23
    mov dx,ATA_CYL_HI
    mov ax,bx
    out dx,al

    ; LBA 24-27 (and drive setting)
    mov dx,ATA_DRV_HEAD
    shr bx,8
    or bl,11100000b
    mov al,bl
    out dx,al

    ; command
    mov dx,ATA_CMND
    mov al,030h
    out dx,al

    call ata_wait

    mov dx,ATA_CMND
.wait:
    in al,dx
    test al,1               ; err
    jnz .error
    test al,8               ; drq
    jz .wait

    ; sector count loop
    xor ch,ch
    mov bx,cx

    ; write sector from buffer at DS:SI
    mov dx,ATA_DATA

.start:
    mov cx,256              ; 256 words

.write:
    lodsw
    out dx,ax
    loop .write

    dec bx
    jnz .start

    call ata_wait

    xor ax,ax
    jmp .exit

.error:
    mov ax,1

.exit:
    pop dx
    pop cx
    pop bx
    ret

