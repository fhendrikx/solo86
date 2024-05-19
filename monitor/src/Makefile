
AS=nasm
DD=dd
SP=../../utils/split/split
BH=../../utils/hex/bin2hex

MON_SRC = monitor.asm
MON_BIN = monitor.bin
MON_LST = monitor.lst
MON_ROM = monitor.rom
MON_HEX = monitor.hex
ROM_BASE = rom_base

# rules

.PHONY: all clean

all: $(MON_BIN)

clean:
	rm -f *.bin *.lst *.rom *.hex monitor.0 monitor.1

$(MON_BIN): $(MON_SRC)
	$(AS) $(MON_SRC) -O9 -f bin -o $(MON_BIN) -l $(MON_LST)
#	$(DD) if=/dev/zero bs=1024 count=448 > $(MON_ROM)
#	cat $(MON_BIN) >> $(MON_ROM)
	cp $(ROM_BASE) $(MON_ROM)
	$(DD) if=$(MON_BIN) of=$(MON_ROM) seek=0 conv=sparse,nocreat,notrunc
	$(SP) --file=$(MON_ROM)
	$(BH) --file=$(MON_BIN) --seg=F000 --start=F000:FFF0 > $(MON_HEX)

