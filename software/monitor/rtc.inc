
; RTC addresses
cmos_secs       equ 00h ; seconds location in RTC
cmos_secs_alarm equ 01h ; alarm seconds location in RTC
cmos_mins       equ 02h ; minutes location in RTC
cmos_mins_alarm equ 03h ; alarm minutes location in RTC
cmos_hour       equ 04h ; hours locaiton in RTC
cmos_hour_alarm equ 05h ; alarm hours location in RTC
cmos_day        equ 06h ; day location in RTC
cmos_date       equ 07h ; date location in RTC
cmos_month      equ 08h ; month location in RTC
cmos_year       equ 09h ; year location in RTC

cmos_ctrl_a     equ 0Ah ; RTC control A register
cmos_uip        equ 80h ; RTC update in progress bit

cmos_ctrl_b     equ 0Bh ; RTC control B register
cmos_dse        equ 01h ; RTC daylight savings enable bit
cmos_24hours    equ 02h ; RTC 24 hours format (1 = 24 hours, 0 = 12)
cmos_uie        equ 10h ; RTC update ended interrupt enable bit
cmos_aie        equ 20h ; RTC alarm interrupt enable bit
cmos_pie        equ 40h ; RTC periodic interrupt enable bit
cmos_set        equ 80h ; RTC set bit (0 = normal operation, 1 = set)

cmos_ctrl_c     equ 0Ch ; RTC control C register
cmos_uf         equ 20h ; RTC update ended interrupt flag
cmos_af         equ 40h ; RTC alarm interrupt flag
cmos_pf         equ 80h ; RTC periodic interrupt flag

cmos_ctrl_d     equ 0Dh ; RTC control D register
cmos_vrt        equ 80h ; RTC vrt bit (1 = battery is OK)

cmos_century    equ 32h ; century location in RTC


;=========================================================================
; rtc_init
; ensures RTC is correct and running
;=========================================================================
rtc_init:
    ; initialise RTC

    ; ctrl a
    mov al,cmos_ctrl_a
    mov ah,26h          ; Oscillator to 1024KHz
    call rtc_write

    ; ctrl b
    mov al,cmos_ctrl_b
    call rtc_read
    xchg al,ah
    and ah,cmos_dse     ; clear everything but DSE
    or ah,cmos_24hours  ; set 24 hours
    mov al,cmos_ctrl_b
    call rtc_write

    ; ctrl d
    mov al,cmos_ctrl_d
    call rtc_read
    test al,cmos_vrt
    jnz .exit

.defaults:
    ; set default time and date
    xor cx,cx
    xor dx,dx
    call rtc_settime

    mov cx,2020h
    mov dx,0101h
    call rtc_setdate

.exit:
    ret

;=========================================================================
; rtc_read
; Input:
;   AL - address
; Output:
;   AL - data byte
;-------------------------------------------------------------------------
rtc_read:
    cli
    out RTC_CTRL,al
    jmp $+2
    jmp $+2
    jmp $+2
    jmp $+2
    in  al,RTC_DATA
    sti
    ret

;=========================================================================
; rtc_write
; Input:
;   AL - address
;   AH - data byte
; Output:
;   None
;-------------------------------------------------------------------------
rtc_write:
    cli
    out RTC_CTRL,al
    jmp $+2
    jmp $+2
    jmp $+2
    jmp $+2
    xchg ah,al
    out RTC_DATA,al
    xchg ah,al
    sti
    ret

;=========================================================================
; rtc_setdate
; Input:
;   CH - BCD century
;   CL - BCD year
;   DH - BCD month
;   DL - BCD date
; Output:
;   None
;-------------------------------------------------------------------------
rtc_setdate:
    push ax

    ; tell RTC we're setting the date
    mov al,cmos_ctrl_b
    call rtc_read
    xchg al,ah
    or ah,cmos_set      ; set SET bit
    mov al,cmos_ctrl_b
    call rtc_write

    ; date
    mov al,cmos_date
    mov ah,dl
    call rtc_write

    ; month
    mov al,cmos_month
    mov ah,dh
    call rtc_write

    ; year
    mov al,cmos_year
    mov ah,cl
    call rtc_write

    ; century
    mov al,cmos_century
    mov ah,ch
    call rtc_write

    ; tell RTC we're done setting the date
    mov al,cmos_ctrl_b
    call rtc_read
    xchg al,ah
    and ah,~cmos_set    ; clear SET bit
    mov al,cmos_ctrl_b
    call rtc_write

    pop ax
    ret

;=========================================================================
; rtc_settime
; Input:
;   CH - BCD hours
;   CL - BCD minutes
;   DH - BCD seconds
; Output:
;   None
;-------------------------------------------------------------------------
rtc_settime:
    push ax

    ; tell RTC we're setting the time
    mov al,cmos_ctrl_b
    call rtc_read
    xchg al,ah
    or ah,cmos_set      ; set SET bit
    mov al,cmos_ctrl_b
    call rtc_write

    ; seconds
    mov al,cmos_secs
    mov ah,dh
    call rtc_write

    ; minutes
    mov al,cmos_mins
    mov ah,cl
    call rtc_write

    ; hours
    mov al,cmos_hour
    mov ah,ch
    call rtc_write

    ; tell RTC we're done setting the time
    mov al,cmos_ctrl_b
    call rtc_read
    xchg al,ah
    and ah,~cmos_set    ; clear SET bit
    mov al,cmos_ctrl_b
    call rtc_write

    pop ax
    ret

