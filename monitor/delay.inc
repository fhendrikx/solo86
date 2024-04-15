;======================================================================
; delay by a give number of milliseconds
; Dylan Hall, April 2024
;======================================================================
; CLK timings from https://zsmith.co/intel.php
; @ 20Mhz 1ms = 20000 CLK
; 13 CLK per inner loop, 8 for final inner loop
; 12 CLK per outer loop, 7 for final outer loop
; 19 + m once
; delay = 19 + m + ((ax - 1) * 12) + 7 + (ax * 13 * (bx - 1)) + (ax * 8)
; ax=1, bx = 1538 => 19 + m + 7 + (13 * 1537) + 8 => 20015 + m
; ax=1, bx = 1537 => 19 + m + 7 + (13 * 1536) + 8 => 20002 + m
; ax=2, bx = 1538 => 19 + m + (12 * 1) + 7 + (2 * 13 * 1537) + (2 * 8) => 40016 + m
; ax=2, bx = 1537 => 19 + m + (12 * 1) + 7 + (2 * 13 * 1536) + (2 * 8) => 39990 + m
; ax=60000, bx = 1538 => 19 + m + (12 * 59999) + 7 + (60000 * 13 * 1537) + (60000 * 8) => 1200060014 + m => 60003ms
; ax=60000, bx = 1537 => 19 + m + (12 * 59999) + 7 + (60000 * 13 * 1536) + (60000 * 8) => 1199280014 + m => 59964ms
; going with bx = 1538
;======================================================================

delay_ms:
    push bx         ; CLK = 3
delay_ms1:
    mov bx,1538     ; CLK = 2
delay_ms2:
    nop             ; CLK = 3
    dec bx          ; CLK = 2
    jnz delay_ms2   ; CLK = 7 + 1 OR 3
    dec ax          ; CLK = 2
    jnz delay_ms1   ; CLK = 7 + 1 OR 3
    pop bx          ; CLK = 5
    ret             ; CLK = 11 + m

