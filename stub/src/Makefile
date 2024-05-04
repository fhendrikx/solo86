
AS=nasm
DD=dd
SP=../../utils/split/split
BH=../../utils/hex/bin2hex

STB_SRC = stub.asm
STB_BIN = stub.bin
STB_LST = stub.lst
STB_ROM = stub.rom
STB_HEX = stub.hex


# rules

.PHONY: all clean

all: $(STB_BIN)

clean:
	rm -f *.bin *.lst *.rom *.hex

$(STB_BIN): $(STB_SRC)
	$(AS) $(STB_SRC) -O9 -f bin -o $(STB_BIN) -l $(STB_LST)
	$(DD) if=/dev/zero bs=1024 count=448 > $(STB_ROM)
	cat $(STB_BIN) >> $(STB_ROM)
	$(BH) --file=$(STB_BIN) > $(STB_HEX)

