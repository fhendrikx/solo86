include $(TOPDIR)/Makefile.conf

all: clean monitor basic stub logo

monitor:
	$(BH) --file=$(TOPDIR)/rom/monitor.bin --seg=F000 --start=F000:FFF0 > monitor.hex

basic:
	$(BH) --file=$(TOPDIR)/rom/basic.bin --seg=1000 --start=1000:0100 > basic.hex

stub:
	$(BH) --file=$(TOPDIR)/rom/stub.bin --seg=F000 --start=F000:FFF0 > stub.hex

logo:
	$(BH) --file=$(TOPDIR)/rom/logo.bin --seg=F000 --start=F000:0100 > logo.hex

clean:
	rm -f *.hex

