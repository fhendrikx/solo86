
AS=nasm
BH=../../../bin/bin2hex

OUT_SRC = out16.asm
OUT_BIN = out16.bin
OUT_LST = out16.lst
OUT_HEX = out16.hex


# rules

.PHONY: all clean

all: $(OUT_BIN)

clean:
	rm -f *.bin *.lst *.hex

$(OUT_BIN): $(OUT_SRC)
	$(AS) $(OUT_SRC) -f bin -o $(OUT_BIN) -l $(OUT_LST)
	$(BH) --file=$(OUT_BIN) --seg=F000 --start=F000:0100 > $(OUT_HEX)

