;=========================================================================
; print
;=========================================================================
%imacro print 1
    mov si,%1
    call print_str
%endm

;=========================================================================
; setloc - Set location.
; Copyright (C) 2010 - 2020 Sergey Kiselev.
;=========================================================================
%imacro setloc  1.nolist
%assign pad_bytes (%1-($-$$)-0F000h)
%if pad_bytes < 0
%assign over_bytes -pad_bytes
%error Preceding code extends beyond setloc location by over_bytes bytes
%endif
%if pad_bytes > 0
%warning Inserting pad_bytes bytes
 times  pad_bytes db 0FFh
%endif
%endm
