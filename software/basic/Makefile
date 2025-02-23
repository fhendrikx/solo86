include $(TOPDIR)/Makefile.conf

NAME = basic.bin
CODE = basic.asm

all: $(NAME)

$(NAME): $(CODE)
	$(AS) $(CODE) -O9 -f bin -o $(NAME) -l basic.lst
	$(CP) $(NAME) $(TOPDIR)/out

clean:
	rm -f $(NAME)
	rm -f *.bin *.lst *.hex
