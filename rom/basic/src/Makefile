
AS=nasm
BH=../../utils/hex/bin2hex

BAS_SRC = basic.asm
BAS_BIN = basic.bin
BAS_LST = basic.lst
BAS_HEX = basic.hex

.PHONY: all clean

all: $(BAS_BIN)

clean:
	rm -f *.bin *.lst *.hex

$(BAS_BIN): $(BAS_SRC)
	$(AS) $(BAS_SRC) -O9 -f bin -o $(BAS_BIN) -l $(BAS_LST)
	$(BH) --file=$(BAS_BIN) --seg=1000 --start=1000:0100 > $(BAS_HEX)
