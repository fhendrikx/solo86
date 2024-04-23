
AS=nasm
DD=dd

MON_BIN = monitor.bin
MON_SRC = monitor.asm

STB_BIN = stub.bin
STB_SRC = stub.asm


# rules

.PHONY: all clean

all: $(MON_BIN) $(STB_BIN)

clean:
	rm -f *.bin *.lst *.rom

$(MON_BIN): $(MON_SRC)
	$(AS) $(MON_SRC) -O9 -f bin -o $(MON_BIN)
	$(DD) if=/dev/zero bs=1024 count=448 > monitor.rom
	cat $(MON_BIN) >> monitor.rom

$(STB_BIN): $(STB_SRC)
	$(AS) $(STB_SRC) -O9 -f bin -o $(STB_BIN)

