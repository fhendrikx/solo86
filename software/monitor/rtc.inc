
; RTC addresses
cmos_secs       equ 00h ; seconds
cmos_secs_alarm equ 01h ; alarm seconds
cmos_mins       equ 02h ; minutes
cmos_mins_alarm equ 03h ; alarm minutes
cmos_hour       equ 04h ; hours
cmos_hour_alarm equ 05h ; alarm hours
cmos_day        equ 06h ; day
cmos_date       equ 07h ; date
cmos_month      equ 08h ; month
cmos_year       equ 09h ; year

cmos_ctrl_a     equ 0Ah ; control A register
cmos_uip        equ 80h ; update in progress bit

cmos_ctrl_b     equ 0Bh ; control B register
cmos_dse        equ 01h ; daylight savings enable bit
cmos_24hours    equ 02h ; 24 hours format (1 = 24 hours, 0 = 12)
cmos_uie        equ 10h ; update ended interrupt enable bit
cmos_aie        equ 20h ; alarm interrupt enable bit
cmos_pie        equ 40h ; periodic interrupt enable bit
cmos_set        equ 80h ; set bit (0 = normal operation, 1 = set)

cmos_ctrl_c     equ 0Ch ; control C register
cmos_uf         equ 20h ; update ended interrupt flag
cmos_af         equ 40h ; alarm interrupt flag
cmos_pf         equ 80h ; periodic interrupt flag

cmos_ctrl_d     equ 0Dh ; control D register
cmos_vrt        equ 80h ; vrt bit (1 = battery is OK)

cmos_century    equ 32h ; century


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

    ; sanity check
    mov al,cmos_ctrl_a
    call rtc_read
    cmp al,26h
    jne .exit

    ; ctrl b
    mov al,cmos_ctrl_b
    call rtc_read
    xchg al,ah
    and ah,cmos_dse     ; clear everything but DSE
    or ah,cmos_24hours  ; set 24 hours
    mov al,cmos_ctrl_b
    call rtc_write

    ; ctrl c
    mov al,cmos_ctrl_c
    call rtc_read

    ; ctrl d
    mov al,cmos_ctrl_d
    call rtc_read

    ; sanity check
    ;mov al,cmos_secs
    ;call rtc_read
    ;cmp al,59
    ;ja .defaults

    ;mov al,cmos_mins
    ;call rtc_read
    ;cmp al,59
    ;ja .defaults

    ;mov al,cmos_hour
    ;call rtc_read
    ;cmp al,23
    ;ja .defaults

    ;mov al,cmos_date
    ;call rtc_read
    ;cmp al,31
    ;ja .defaults

    ;mov al,cmos_month
    ;call rtc_read
    ;cmp al,12
    ;ja .defaults

    ;mov al,cmos_year
    ;call rtc_read
    ;cmp al,99
    ;ja .defaults

    jmp .exit

.defaults:
    ; set default time and date
    xor cx,cx
    xor dx,dx
    call rtc_time_set

    mov cx,2020h
    mov dx,0101h
    call rtc_date_set

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
    push ax
    mov ax,10
    call delay_ms
    pop ax
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
    push ax
    mov ax,10
    call delay_ms
    pop ax
    xchg al,ah
    out RTC_DATA,al
    sti
    ret

;=========================================================================
; rtc_date_get
; Input:
;   None
; Output:
;   CH - BCD century
;   CL - BCD year
;   DH - BCD month
;   DL - BCD date
;-------------------------------------------------------------------------
rtc_date_get:
    push ax

    mov al,cmos_date
    call rtc_read
    mov dl,al
    mov al,cmos_month
    call rtc_read
    mov dh,al
    mov al,cmos_year
    call rtc_read
    mov cl,al
    mov al,cmos_century
    call rtc_read
    mov ch,al

    pop ax
    ret

;=========================================================================
; rtc_date_set
; Input:
;   CH - BCD century
;   CL - BCD year
;   DH - BCD month
;   DL - BCD date
; Output:
;   None
;-------------------------------------------------------------------------
rtc_date_set:
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
; rtc_time_get
; Input:
;   None
; Output:
;   CH - BCD hours
;   CL - BCD minutes
;   DH - BCD seconds
;   DL - daylight savings flag
;-------------------------------------------------------------------------
rtc_time_get:
    push ax

    mov al,cmos_ctrl_b
    call rtc_read
    and al,cmos_dse
    mov dl,al

    mov al,cmos_secs
    call rtc_read
    mov dh,al

    mov al,cmos_mins
    call rtc_read
    mov cl,al

    mov al,cmos_hour
    call rtc_read
    mov ch,al

    pop ax
    ret

;=========================================================================
; rtc_time_set
; Input:
;   CH - BCD hours
;   CL - BCD minutes
;   DH - BCD seconds
; Output:
;   None
;-------------------------------------------------------------------------
rtc_time_set:
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

