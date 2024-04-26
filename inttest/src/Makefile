ASM=nasm

all: main.bin inttest.hex

main.bin: main.asm
	$(ASM) main.asm -O9 -f bin -o main.bin -l main.lst
	dd if=/dev/zero bs=1024 count=508 > inttest.rom
	cat main.bin >> inttest.rom

inttest.hex:
	$(ASM) main.asm -O9 -f ith -o inttest.hex -l main.lst


clean:
	rm -f *.bin *.lst *.rom *.hex

zip:
	cd ..; tar zcf monitor.tar.gz monitor/
