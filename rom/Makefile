include $(TOPDIR)/Makefile.conf

all: rom

rom:
	$(PD) > solo.rom
	$(DD) if=monitor.bin of=solo.rom bs=1k seek=0 conv=sparse,nocreat,notrunc
	$(DD) if=basic.bin of=solo.rom bs=1k seek=64 conv=sparse,nocreat,notrunc
	$(DD) if=stub.bin of=solo.rom bs=1k seek=128 conv=sparse,nocreat,notrunc
	$(DD) if=out16.bin of=solo.rom bs=1k seek=192 conv=sparse,nocreat,notrunc
	$(DD) if=logo.bin of=solo.rom bs=1k seek=256 conv=sparse,nocreat,notrunc
	$(SP) --file=solo.rom

burn0:
	@echo "Please put solo.0 (EVEN) into the burner; please ENTER when ready"
	@read DUMMY
	minipro -p "SST39SF040" -w solo.0

burn1:
	@echo "Please put solo.1 (ODD)  into the burner; please ENTER when ready"
	@read DUMMY
	minipro -p "SST39SF040" -w solo.1

clean:
	rm -f *.bin *.rom *.0 *.1

