include $(TOPDIR)/Makefile.conf

all: rom

rom:
#	banking works in 128k blocks
#	at power up the first 128k of ROM is repeated from 0x8000 -> 0xF000
#	place the monitor in the second half of the block so it ends up in 0xF000
	$(PD) > solo.rom
#	00000 -> 1FFFF in ROM (0-128k) (power up image)
	$(DD) if=monitor.bin of=solo.rom bs=1k seek=64 conv=nocreat,notrunc
#	20000 -> 3FFFF in ROM (128-256k)
	$(DD) if=basic.bin of=solo.rom bs=1k seek=128 conv=nocreat,notrunc
	$(DD) if=elks.bin of=solo.rom bs=1k seek=192 conv=nocreat,notrunc
	$(DD) if=out16.bin of=solo.rom bs=1k seek=208 conv=nocreat,notrunc
#	40000 -> 5FFFF in ROM (256-384k)
	$(DD) if=stub.bin of=solo.rom bs=1k seek=256 conv=nocreat,notrunc
	$(DD) if=logo.bin of=solo.rom bs=1k seek=320 conv=nocreat,notrunc
#	60000 -> 7FFFF in ROM (384-512k)
#	80000 -> DFFFF in ROM (512-896k) (3 x 128k blocks)
	$(DD) if=$(TOPDIR)/elks/romfs.bin of=solo.rom bs=1k seek=512 conv=nocreat,notrunc
#	E0000 -> FFFFF in ROM (896-1024k)
	$(DD) if=$(TOPDIR)/elks/Image.bin of=solo.rom bs=1k seek=896 conv=nocreat,notrunc
	$(SP) --file=solo.rom

rom0:
	@echo "Please put solo.0 (EVEN) into the burner; please ENTER when ready"
	@read DUMMY
	minipro -p "SST39SF040" -w solo.0

rom1:
	@echo "Please put solo.1 (ODD)  into the burner; please ENTER when ready"
	@read DUMMY
	minipro -p "SST39SF040" -w solo.1

clean:
	rm -f *.bin *.rom *.0 *.1

