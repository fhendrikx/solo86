
AS=nasm
DD=dd

BAS_SRC = basic.asm
BAS_BIN = basic.bin
BAS_LST = basic.lst
BAS_ROM = basic.rom


.PHONY: all clean

all: $(BAS_BIN)

clean:
	rm -f *.bin *.lst *.rom

$(BAS_BIN): $(BAS_SRC)
	$(AS) $(BAS_SRC) -O9 -f bin -o $(BAS_BIN) -l $(BAS_LST)

