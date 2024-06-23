;======================================================================
; Ferry Hendrikx, June 2024
;======================================================================

;======================================================================
; ROM menu
;======================================================================

rom_menu:
    print rom_welcome

    push cs
    pop ds                  ; DS:SI
    mov si,rom_table

    xor bl,bl               ; counter

.next:
; segment
    lodsw
    cmp ax,0                ; are we done?
    je .loop

    push ax                 ; store seg

    mov al,bl               ; counter
    inc cl
    call print_hex

    mov al,' '
    call print_chr
    mov al,'-'
    call print_chr
    mov al,' '
    call print_chr

    pop ax                  ; restore seg

    call print_word         ; seg

    mov al,':'
    call print_chr

; offset
    lodsw
    call print_word         ; offset

    mov al,' '
    call print_chr
    mov al,'-'
    call print_chr
    mov al,' '
    call print_chr

; size/flags
    add si,4                ; skip size and flag

; name
    call print_str

    mov al,0Ah
    call print_chr

    add si,24               ; print char pops DS:SI
    jmp .next

; main loop
.loop:
    print rom_prompt

    call read_chr

    ; quit to monitor
    cmp al,'Q'
    je .exit

    ; number?
    cmp al,30h
    jae .number
    jmp .loop

.number:
    sub al,30h              ; '0'

    mov bl,[ cs:rom_cnt ]
    cmp al,bl               ; too high?
    ja .loop

    call rom_exec

.exit:
    ret


;======================================================================
; execute ROM
; Execute ROM number given in AL
;======================================================================

rom_exec:
    print rom_select

    push cs                 ; CS => DS
    pop ds

    mov bl,32               ; multiply by size of ROM table entry
    mul bl
    mov bx,ax
    mov si,rom_table
    add si,bx

; segment
    lodsw
    mov [ rom_seg ],ax

; offset
    lodsw
    mov [ rom_ofs ],ax

; size
    lodsw
    push ax                 ; store size

; flags
    lodsw
    cmp ax,0
    je .call

; copy ROM to RAM
    mov ax,[ rom_seg ]
    mov ds,ax
    xor si,si

    mov ax,rom_relocate
    mov es,ax               ; ES:DI
    xor di,di

    pop ax                  ; restore size

.copy:
    mov cx,16384

.copy_rom:
    movsw
    loop .copy_rom
    dec ax
    cmp ax,0
    jg .copy

.call:
    push cs                 ; return segment and offset
    mov bx,.return
    push bx

    push cs                 ; CS => DS
    pop ds

    call far [ rom_ofs ]

;; alternative to CALL FAR
;; push the segment and offset onto the stack and call retf
;    mov ax,[ rom_seg ]
;    push ax
;    mov ax,[ rom_ofs ]
;    push ax
;    retf

.return:
    ret


;======================================================================
; scan ROM
; Scans from segment rom_first to segment rom_last and store the details
; of any ROMs it finds in the rom_table
;======================================================================

rom_scan:
    push ax
    push bx
    push cx
    push dx
    push ds
    push es

    xor dl,dl

    mov bx,rom_first
    mov ds,bx               ; DS:SI
    xor si,si

    push cs
    pop es                  ; ES:DI
    mov di,rom_table

.scan:
    lodsw
    cmp ax,0AA55h           ; ROM signature found?
    jne .next
    inc dl
    mov ax,bx               ; store segment
    stosw
    mov cx,30/2

; copy part of signature block
.copy:
    movsw                   ; DS:SI => ES:DI
    loop .copy

.next:
    add bx,rom_size         ; next segment to check
    mov ds,bx               ; DS:SI
    xor si,si
    cmp bx,rom_last
    jb .scan

.exit:
    mov byte [ cs:rom_cnt ],dl

    pop es
    pop ds
    pop dx
    pop cx
    pop bx
    pop ax
    ret


;======================================================================
; data
;======================================================================

; ROM count
rom_cnt:                    db 000h


