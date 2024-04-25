
AS=nasm
DD=dd

MON_SRC = monitor.asm
MON_BIN = monitor.bin
MON_LST = monitor.lst

STB_SRC = stub.asm
STB_BIN = stub.bin
STB_LST = stub.lst


# rules

.PHONY: all clean

all: $(MON_BIN) $(STB_BIN)

clean:
	rm -f *.bin *.lst *.rom

$(MON_BIN): $(MON_SRC)
	$(AS) $(MON_SRC) -O9 -f bin -o $(MON_BIN) -l $(MON_LST)
	$(DD) if=/dev/zero bs=1024 count=448 > monitor.rom
	cat $(MON_BIN) >> monitor.rom

$(STB_BIN): $(STB_SRC)
	$(AS) $(STB_SRC) -O9 -f bin -o $(STB_BIN) -l $(STB_LST)

