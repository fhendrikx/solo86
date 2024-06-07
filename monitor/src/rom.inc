;======================================================================
; Ferry Hendrikx, June 2024
;======================================================================

; segment first and last
rom_first       equ 08000h
rom_last        equ 0F000h

rom_size        equ 0400h
rom_max         equ 8

; segment for relocation
rom_relocate    equ 01000h


;======================================================================
; scan ROM
; Scans from segment rom_first to segment rom_last and store the details
; of any ROMs it finds in the rom_table
;======================================================================

rom_scan:
    push bx
    push cx
    push di
    push ds
    push es
    push si

    mov bx,rom_first
    mov ds,bx           ; DS:SI
    xor si,si

    push cs
    pop es              ; ES:DI
    mov di,rom_table

.scan:
    lodsw
    cmp ax,0AA55h       ; ROM signature found?
    jne .next
    mov ax,bx           ; store segment
    stosw
    mov cx,30/2

; copy part of signature block
.copy:
    movsw               ; DS:SI => ES:DI
    loop .copy

.next:
    add bx,rom_size     ; next segment to check
    mov ds,bx           ; DS:SI
    xor si,si
    cmp bx,rom_last
    jb .scan

.exit:
    pop si
    pop es
    pop ds
    pop di
    pop cx
    pop bx
    ret


;======================================================================
; display ROM table
; Displays ROMs found and stored in the ROM table.
;======================================================================

rom_display:
    push ax
    push bx
    push ds
    push si

    push cs
    pop ds              ; DS:SI
    mov si,rom_table

    xor bl,bl           ; counter

.next:
; segment
    lodsw
    cmp ax,0            ; are we done?
    je .exit

    push ax             ; store seg

    mov al,bl           ; counter
    inc cl
    call print_hex

    mov al,' '
    call print_chr

    pop ax              ; restore seg

    call print_word

    mov al,':'
    call print_chr

; offset
    lodsw
    call print_word

    mov al,' '
    call print_chr

; size/flags
    add si,4            ; skip size and flag

; name
    call print_str

    mov al,0Ah
    call print_chr

    add si,24           ; print char restores DS:SI
    jmp .next

.exit:
    pop si
    pop ds
    pop ax
    ret


;======================================================================
; call ROM
; Execute ROM number given in AL
;======================================================================

rom_call:
    push ax
    push bx
    push ds
    push di
    push si

    push cs             ; CS => DS
    pop ds

    mov bl,32           ; multiply by size of ROM table entry
    mul bl
    mov bx,ax
    mov si,[ rom_table+bx ]

; segment
    lodsw
    mov [ rom_seg ],ax

; offset
    lodsw
    mov [ rom_ofs ],ax

; size
    lodsw
    mov bx,16384        ; multiply by 16384
    mul bx
    push ax             ; size for later

; flags
    lodsw
    cmp ax,0
    je .call

; copy ROM to RAM
    mov ax,[ rom_seg ]
    mov ds,ax
    xor si,si

    mov ax,rom_relocate
    mov es,ax
    xor di,di

    pop cx              ; size

.copy_rom:
    movsw
    loop .copy_rom

.call:
    push cs             ; return segment and offset
    mov bx,.return
    push bx

    call far [ rom_ofs ]

;; we don't have a CALL FAR segment offset in 8086; so we emulate it by
;; pushing the segment and offset onto the stack and calling retf
;    push es             ; ROM segment and offset
;    push bx
;    retf

.return:
    pop si
    pop di
    pop ds
    pop bx
    pop ax
    ret

