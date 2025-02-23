include $(TOPDIR)/Makefile.conf

NAME = monitor.bin
CODE = monitor.asm

all: $(NAME)

$(NAME): $(CODE)
	$(AS) $(CODE) -O9 -f bin -o $(NAME) -l monitor.lst
	$(CP) $(NAME) $(TOPDIR)/rom

clean:
	rm -f $(NAME)
	rm -f *.bin *.lst *.hex
