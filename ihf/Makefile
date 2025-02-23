include $(TOPDIR)/Makefile.conf

all: clean basic monitor stub

basic:
	$(BH) --file=$(TOPDIR)/out/basic.bin --seg=1000 --start=1000:0100 > basic.hex

monitor:
	$(BH) --file=$(TOPDIR)/out/monitor.bin --seg=F000 --start=F000:FFF0 > monitor.hex

stub:
	$(BH) --file=$(TOPDIR)/out/stub.bin --seg=F000 --start=F000:FFF0 > stub.hex

clean:
	rm -f *.hex

