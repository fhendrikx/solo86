include $(TOPDIR)/Makefile.conf

NAME = logo.bin
CODE = logo.asm
DEPS = ../monitor/const.inc ../monitor/delay.inc sound.inc xwing.inc

all: $(NAME)

$(NAME): $(CODE) $(DEPS)
	$(AS) $(CODE) -O9 -f bin -o $(NAME) -l logo.lst
	$(CP) $(NAME) $(TOPDIR)/rom

clean:
	rm -f $(NAME)
	rm -f *.bin *.lst *.hex
