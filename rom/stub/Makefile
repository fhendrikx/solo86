include $(TOPDIR)/Makefile.conf

NAME = stub.bin
CODE = stub.asm

all: $(NAME)

$(NAME): $(CODE)
	$(AS) $(CODE) -O9 -f bin -o $(NAME) -l stub.lst
	$(CP) $(NAME) $(TOPDIR)/out

clean:
	rm -f $(NAME)
	rm -f *.bin *.lst *.hex
