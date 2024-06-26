
AS=nasm
BH=../../utils/hex/bin2hex

STB_SRC = stub.asm
STB_BIN = stub.bin
STB_LST = stub.lst
STB_HEX = stub.hex


# rules

.PHONY: all clean

all: $(STB_BIN)

clean:
	rm -f *.bin *.lst *.hex

$(STB_BIN): $(STB_SRC)
	$(AS) $(STB_SRC) -O9 -f bin -o $(STB_BIN) -l $(STB_LST)
	$(BH) --file=$(STB_BIN) --seg=F000 --start=F000:FFF0 > $(STB_HEX)

