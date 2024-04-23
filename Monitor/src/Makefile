ASM=nasm

all: main.bin

main.bin: main.asm
	$(ASM) main.asm -O9 -f bin -o main.bin -l main.lst
	dd if=/dev/zero bs=1024 count=508 > monitor.rom
	cat main.bin >> monitor.rom
	#truncate -s 64k main.bin

clean:
	rm -f *.bin *.lst *.rom

zip:
	cd ..; tar zcf monitor.tar.gz monitor/
