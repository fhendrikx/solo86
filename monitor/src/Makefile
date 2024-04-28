
AS=nasm
DD=dd
SPLIT=../../utils/split/split
BIN2HEX=../../utils/hex/bin2hex

MON_SRC = monitor.asm
MON_BIN = monitor.bin
MON_LST = monitor.lst
MON_ROM = monitor.rom
MON_HEX = monitor.hex

STB_SRC = stub.asm
STB_BIN = stub.bin
STB_LST = stub.lst
STB_HEX = stub.hex


# rules

.PHONY: all clean

all: $(MON_BIN) $(STB_BIN)

clean:
	rm -f *.bin *.lst *.rom *.hex monitor.0 monitor.1

$(MON_BIN): $(MON_SRC)
	$(AS) $(MON_SRC) -O9 -f bin -o $(MON_BIN) -l $(MON_LST)
	$(DD) if=/dev/zero bs=1024 count=448 > $(MON_ROM)
	cat $(MON_BIN) >> $(MON_ROM)
	$(SPLIT) --file=$(MON_ROM)
	$(BIN2HEX) --file=$(MON_BIN) > $(MON_HEX)

$(STB_BIN): $(STB_SRC)
	$(AS) $(STB_SRC) -O9 -f bin -o $(STB_BIN) -l $(STB_LST)
	$(BIN2HEX) --file=$(STB_BIN) > $(STB_HEX)

