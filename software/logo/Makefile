include $(TOPDIR)/Makefile.conf

NAME = logo.bin
CODE = logo.asm

all: $(NAME)

$(NAME): $(CODE)
	$(AS) $(CODE) -O9 -f bin -o $(NAME) -l logo.lst
	$(CP) $(NAME) $(TOPDIR)/rom

clean:
	rm -f $(NAME)
	rm -f *.bin *.lst *.hex
