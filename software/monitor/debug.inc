
;======================================================================
; debug menu
;======================================================================

debug_menu:
    print dbg_welcome

; main loop
.loop:
    print dbg_prompt

    push cs
    pop ax
    mov ds,ax
    mov es,ax

    mov di,buffer
    call read_str           ; ES:DI

    mov si,buffer           ; DS:SI
    lodsb


    ; repeat last command?

    cmp al,0Ah
    je  cmd_last


    ; last command

    mov byte [cs:dbg_last],al


    ; which command?

    cmp al,'A'
    je  cmd_sys_info

    cmp al,'D'
    je  cmd_mem_dump
    cmp al,'F'
    je  cmd_mem_fill
    cmp al,'R'
    je  cmd_mem_remap
    cmp al,'S'
    je  cmd_unasm_style
    cmp al,'U'
    je  cmd_unasm

    cmp al,'I'
    je  cmd_inp
    cmp al,'O'
    je  cmd_out
    cmp al,'J'
    je  cmd_jmp
    cmp al,'Y'
    je  cmd_timer
    cmp al,'X'
    je  cmd_halt
    cmp al,'Z'
    je  cmd_shutdown

    cmp al,'t'
    je  cmd_clock_time
    cmp al,'d'
    je  cmd_clock_date

    cmp al,'c'
    je  cmd_ata_detect
    cmp al,'i'
    je  cmd_ata_ident
    cmp al,'r'
    je  cmd_ata_reset
    cmp al,'e'
    je  cmd_ata_read
    cmp al,'f'
    je  cmd_ata_write

    cmp al,'L'
    je  cmd_ihx_load
    cmp al,'E'
    je  cmd_ihx_exec

    cmp al,'K'
    je  cmd_echo

    cmp al,'h'
    je  cmd_help
    cmp al,'?'
    je  cmd_help
    cmp al,'v'
    je  cmd_version
    cmp al,'Q'
    je .exit

    jmp .loop

.exit:
    ret


syntax:
    ; syntax error
    print dbg_syntax
    jmp debug_menu.loop


;======================================================================
; debug menu commands
;======================================================================

cmd_last:
    mov al,byte [cs:dbg_last]

    cmp al,'D'
    je .read
    cmp al,'U'
    je .unasm

    jmp debug_menu.loop

.read:
    push ds

    ; get last address
    mov bx,[cs:dbg_ofs]
    mov si,bx
    mov bx,[cs:dbg_seg]
    mov ds,bx

    mov cx,256

    call mem_dump

    ; store last address
    mov [cs:dbg_ofs],bx
    mov bx,ds
    mov [cs:dbg_seg],bx

    pop ds

    jmp debug_menu.loop

.unasm:
    push ds

    ; get last address
    mov bx,[cs:dbg_ofs]
    mov si,bx
    mov bx,[cs:dbg_seg]
    mov ds,bx

    mov cx,16

    call unasm

    ; store last address
    mov [cs:dbg_ofs],bx
    mov bx,ds
    mov [cs:dbg_seg],bx

    pop ds

    jmp debug_menu.loop


cmd_sys_info:
    print dbg_info

    call info_sys
    call info_irq
    call info_rtc
    call info_rom

    jmp debug_menu.loop


cmd_unasm:
    call spaces
    call inp_word           ; seg
    mov bx,ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov si,ax

    push ds
    mov ds,bx
    mov cx,16

    print dbg_unasm
    call unasm

    ; store last address
    mov [cs:dbg_ofs],bx
    mov bx,ds
    mov [cs:dbg_seg],bx

    pop ds

    jmp debug_menu.loop


cmd_unasm_style:
    mov al,byte [cs:unasm_style]
    not al
    mov byte [cs:unasm_style],al

    cmp al,0
    je  .intel

    print dbg_unasm_att
    jmp debug_menu.loop

.intel:
    print dbg_unasm_intel
    jmp debug_menu.loop


cmd_mem_fill:
    call spaces
    call inp_word           ; seg
    mov bx,ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov di,ax

    call spaces
    call inp_hex            ; count
    xor cx,cx
    mov cl,al

    call spaces
    call inp_hex            ; byte

    print dbg_fill

    push es
    mov es,bx
    call mem_fill
    pop es

    jmp debug_menu.loop


cmd_mem_dump:
    call spaces
    call inp_word           ; seg
    mov bx,ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov si,ax

    print dbg_dump

    push ds
    mov cx,256
    mov ds,bx

    call mem_dump

    ; store last address
    mov [cs:dbg_ofs],bx
    mov bx,ds
    mov [cs:dbg_seg],bx

    pop ds

    jmp debug_menu.loop


cmd_mem_remap:
    call spaces
    call inp_word           ; seg
    push ax

    call spaces

    call inp_hex            ; count
    xor cx,cx
    mov cl,al
    pop ax

    call sys_remap

    print dbg_remap
    call print_word

    mov al,0Ah
    call print_chr

    jmp debug_menu.loop


cmd_inp:
    call spaces
    call inp_hex            ; port

    xor dx,dx
    mov dl,al

    print dbg_inp
    call print_hex

    mov al,'='
    call print_chr

    in al,dx                ; INP
    call print_hex

    jmp debug_menu.loop


cmd_out:
    call spaces
    call inp_hex            ; byte

    xor cx,cx
    mov cl,al

    print dbg_out

    call spaces
    call inp_hex            ; port

    xor dx,dx
    mov dl,al

    call print_hex

    mov al,'='
    call print_chr

    mov al,cl
    call print_hex

    out dx,al               ; OUT

    jmp debug_menu.loop


cmd_jmp:
    call spaces
    call inp_word           ; seg
    mov [cs:jmp_seg],ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov [cs:jmp_ofs],ax

    print dbg_jump

    ; indirect far jump to CS:IP
    jmp far [cs:jmp_ofs]


cmd_timer:
    call spaces
    call inp_hex            ; enable/disable

    cmp al,0
    je .disable

    print dbg_timer

    xor ax,ax               ; reset counter
    mov [ cs:irq0_cnt ],ax

    mov al,034h             ; timer 0, lsb/msb, mode 2, binary counter
    out PIT_CTRL,al

    mov ax,PIT_TICK
    out PIT_CNTR0,al        ; lsb
    xchg al,ah
    out PIT_CNTR0,al        ; msb

    mov al,01h              ; enable
    out PIT_CNTR0_EN,al

    jmp debug_menu.loop

.disable:
    print dbg_timer_off

    xor ax,ax               ; disable
    out PIT_CNTR0_EN,al

    jmp debug_menu.loop


cmd_clock_time:
    call spaces
    call inp_hex            ; hour
    mov ch,al

    mov al,':'
    call expect
    jne syntax

    call inp_hex            ; mins
    mov cl,al

    mov al,':'
    call expect
    jne syntax

    call inp_hex            ; secs
    mov dh,al

    call rtc_time_set

    jmp debug_menu.loop


cmd_clock_date:
    call spaces
    call inp_hex            ; century
    mov ch,al

    call inp_hex            ; year
    mov cl,al

    mov al,'-'
    call expect
    jne syntax

    call inp_hex            ; month
    mov dh,al

    mov al,'-'
    call expect
    jne syntax

    call inp_hex            ; date
    mov dl,al

    call rtc_date_set

    jmp debug_menu.loop


cmd_halt:
    call spaces

    print dbg_halt

    hlt

    jmp debug_menu.loop


cmd_shutdown:
    call spaces

    print dbg_shutdown

    ; From the Harris 80C286 datasheet
    ; Shutdown occurs when a severe error is detected that prevents
    ; further instruction processing by the CPU. Shutdown and halt are
    ; externally signalled via a halt bus operation. They can be distin-
    ; guished by A1 HIGH for halt and A1 LOW for shutdown. In real
    ; address mode, shutdown can occur under two conditions:
    ;
    ; * Exceptions 8 or 13 happen and the IDT limit does not
    ;   include the interrupt vector.
    ; * A CALL INT or PUSH instruction attempts to wrap around
    ;   the stack segment when SP is not even.
    ;
    ; An NMI input can bring the CPU out of shutdown if the IDT limit
    ; is at least 000F(H) and SP is greater than 0005(H), otherwise
    ; shutdown can only be exited via the RESET input.

    ; Second method above
    ; set the stack pointer to one so the INT instruction forces a wrap
    mov ax,1
    mov sp,ax
    int 0

    ; we shouldn't get here
    ; however, if we do, the stack pointer is broken, do nothing
.loop:
    jmp cmd_shutdown.loop


cmd_ata_detect:
    call ata_detect

    print dbg_detect

    call print_word

    mov al,0Ah
    call print_chr

    jmp debug_menu.loop


cmd_ata_ident:
    call spaces
    call inp_word           ; seg
    mov bx,ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov di,ax

    push es
    mov es,bx

    call ata_ident

    pop es

    cmp al,0
    je .good

    print dbg_ident_err
    jmp debug_menu.loop

.good:
    print dbg_ident
    jmp debug_menu.loop


cmd_ata_reset:
    print dbg_reset

    jmp debug_menu.loop


cmd_ata_read:
    call spaces
    call inp_word           ; seg
    mov bx,ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov di,ax

    push es
    mov es,bx

    call spaces
    call inp_word           ; LBA (0-16 bits)
    call print_word

    xor bx,bx

    call ata_read

    pop es

    cmp al,0
    je .good

    print dbg_read_err
    jmp debug_menu.loop

.good:
    print dbg_read
    jmp debug_menu.loop


cmd_ata_write:
    call spaces
    call inp_word           ; seg
    mov bx,ax

    mov al,':'
    call expect
    jne syntax

    call inp_word           ; offset
    mov cx,ax

    call spaces
    call inp_word           ; LBA (0-16 bits)
    call print_word

    push ds
    mov ds,bx
    mov si,cx

    xor bx,bx

    call ata_write

    pop ds

    cmp al,0
    je .good

    print dbg_write_err
    jmp debug_menu.loop

.good:
    print dbg_write
    jmp debug_menu.loop


cmd_ihx_load:
    print dbg_load

    call ihx_load

    print dbg_load_addr
    mov ax,[cs:ihx_loc]
    call print_word

    mov al,' '
    call print_chr
    mov al,'['
    call print_chr

    mov ax,[cs:ihx_seg]
    call print_word

    mov al,':'
    call print_chr

    mov ax,[cs:ihx_ofs]
    call print_word

    mov al,']'
    call print_chr
    mov al,0Ah
    call print_chr

    jmp debug_menu.loop


cmd_ihx_exec:
    ; sanity check
    mov ax,[cs:ihx_seg]
    cmp ax,0
    je .err

    print dbg_exec          ; okay

    ; simulate cold start
    xor ax,ax
    mov ds,ax
    mov es,ax
    mov ss,ax

    mov ax,2h
    push ax
    popf

    ; indirect far jump to CS:IP
    jmp far [ ihx_ofs ]

.err:
    print dbg_exec_err      ; error
    jmp debug_menu.loop

cmd_echo:
    call spaces

    print dbg_echo

.echo_loop:
    call read_chr
    cmp al,'Q'
    jne .echo_loop

    jmp debug_menu.loop


cmd_help:
    print dbg_help
    jmp debug_menu.loop


cmd_version:
    print mon_version
    jmp debug_menu.loop


;======================================================================
; debug information
;======================================================================

info_sys:
    print dbg_info_sys

    print dbg_info_sys_bnk
    mov ax,01
    call info_sys_single

    print dbg_info_sys_pit
    mov ax,02
    call info_sys_single

    print dbg_info_sys_rtc
    mov ax,10
    call info_sys_single

    print dbg_info_sys_com
    mov ax,20
    call info_sys_single

    print dbg_info_sys_pnl
    mov ax,80
    call info_sys_single

    ret


info_sys_single:
    test ax,[ cs:hardware ]
    jnz .found
    mov al,0Ah
    call print_chr
    ret

.found:
    mov al,'Y'
    call print_chr
    mov al,0Ah
    call print_chr
    ret


info_irq:
    print dbg_info_irq

    print dbg_info_irq_0
    mov ax,[ cs:irq0_cnt ]
    call info_irq_single

    print dbg_info_irq_1
    mov ax,[ cs:irq1_cnt ]
    call info_irq_single

    print dbg_info_irq_2
    mov ax,[ cs:irq2_cnt ]
    call info_irq_single

    print dbg_info_irq_3
    mov ax,[ cs:irq3_cnt ]
    call info_irq_single

    ret


info_irq_single:
    call print_word

    mov al,0Ah
    call print_chr
    ret


info_rtc:
    print dbg_info_rtc

    ; date
    print dbg_info_rtc_date
    call rtc_date_get

    mov al,ch
    call print_hex
    mov al,cl
    call print_hex

    mov al,'-'
    call print_chr

    mov al,dh
    call print_hex

    mov al,'-'
    call print_chr

    mov al,dl
    call print_hex

    mov al,' '
    call print_chr

    mov al,0Ah
    call print_chr

    ; time
    print dbg_info_rtc_time
    call rtc_time_get

    mov al,ch
    call print_hex

    mov al,':'
    call print_chr

    mov al,cl
    call print_hex

    mov al,':'
    call print_chr

    mov al,dh
    call print_hex

    mov al,0Ah
    call print_chr
    ret


info_rom:
    print dbg_info_rom

    push cs
    pop ds                  ; DS:SI
    mov si,rom_table

.next:
    ; segment

    lodsw
    cmp ax,0                ; are we done?
    je .exit

    print dbg_info_rom_found
    call print_word         ; seg

    mov al,':'
    call print_chr

    ; offset

    lodsw
    call print_word         ; offset

    mov al,' '
    call print_chr
    mov al,'['
    call print_chr

    ; size

    lodsw
    call print_word         ; size

    mov al,']'
    call print_chr
    mov al,' '
    call print_chr
    mov al,'>'
    call print_chr
    mov al,' '
    call print_chr

    ; seg dest

    lodsw
    call print_word         ; seg dest

    mov al,':'
    call print_chr
    mov al,' '
    call print_chr

    add si,8

    ; name

    call print_str

    mov al,0Ah
    call print_chr

    add si,16               ; print char pops DS:SI
    jmp .next

.exit:
    ret


;======================================================================
; functions
;======================================================================

; skip over any spaces
;
spaces:
    lodsb
    cmp al,20h
    je spaces               ; keep going if we had a space
    dec si                  ; undo the last load
    ret


; expect character AL
;
expect:
    push bx
    mov bl,al
    lodsb
    cmp al,bl               ; is it what we expected?
    pop bx
    ret                     ; flag not changed on ret


; read 2 hex characters into AL from ES:DI
;
inp_hex:
    call .nibble
    mov ah,al
    shl ah,4
    call .nibble
    or al,ah
    ret

.nibble:
    lodsb                   ; read char
    cmp al,41h
    jae .letter
    sub al,30h              ; '0'
    ret

.letter:
    sub al,37h              ; 'A' - 10
    ret


; read 4 hex characters into AX from ES:DI
;
inp_word:
    push bx
    call inp_hex
    mov bh,al
    call inp_hex
    mov bl,al
    mov ax,bx
    pop bx
    ret

