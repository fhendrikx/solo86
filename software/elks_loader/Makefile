include $(TOPDIR)/Makefile.conf

NAME = elks.bin
CODE = elks.asm
DEPS = ../monitor/const.inc

all: $(NAME)

$(NAME): $(CODE) $(DEPS)
	$(AS) $(CODE) -O9 -f bin -o $(NAME) -l elks.lst
	$(CP) $(NAME) $(TOPDIR)/rom

clean:
	rm -f $(NAME)
	rm -f *.bin *.lst *.hex
