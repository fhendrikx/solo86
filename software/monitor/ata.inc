
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

    mov dx,ATA_DRV_HEAD
    mov al,0A0h             ; master A0, slave B0
    out dx,al

    call delay_10us

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

    mov dx,ATA_DRV_HEAD
    mov al,0A0h             ; master A0, slave B0
    out dx,al

    mov dx,ATA_SEC_COUNT
    xor al,al
    out dx,al

    mov dx,ATA_SEC_NUM
    out dx,al

    mov dx,ATA_CYL_LO
    out dx,al

    mov dx,ATA_CYL_HI
    out dx,al

    ; command
    mov dx,ATA_CMND
    mov al,0ECh
    out dx,al

    ; does drive exist?
    in al,dx
    cmp al,0
    je .nodev

    ; wait for ATA
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
    stosw
    loop .read

    xor ax,ax
    jmp .exit

.nodev:
    mov ax,1
    jmp .exit

.error:
    mov ax,2

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
;   ES:DI - buffer
; Output:
;   Data in buffer
;-------------------------------------------------------------------------
ata_read:
    push ax
    push bx
    push cx
    push dx

    push ax
    push bx

    ; LBA 24-27 (and drive setting)
    mov dx,ATA_DRV_HEAD
    shr bx,1
    shr bx,1
    shr bx,1
    shr bx,1
    shr bx,1
    shr bx,1
    shr bx,1
    shr bx,1
    or bl,11100000b
    out dx,al

    ; count
    mov dx,ATA_SEC_COUNT
    mov al,cl
    out dx,al

    pop bx
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

    ; command
    mov dx,ATA_CMND
    mov al,020h
    out dx,al

    ; wait for ATA
.wait:
    in al,dx
    test al,1               ; err
    jnz .exit
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

.exit:
    pop dx
    pop cx
    pop bx
    pop ax
    ret

