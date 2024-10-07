
AS=nasm
BH=../../utils/hex/bin2hex

MON_SRC = monitor.asm
MON_BIN = monitor.bin
MON_LST = monitor.lst
MON_HEX = monitor.hex

# rules

.PHONY: all clean

all: $(MON_BIN)

clean:
	rm -f *.bin *.lst *.hex

$(MON_BIN): $(MON_SRC)
	$(AS) $(MON_SRC) -O9 -f bin -o $(MON_BIN) -l $(MON_LST)
	$(BH) --file=$(MON_BIN) --seg=F000 --start=F000:FFF0 > $(MON_HEX)

