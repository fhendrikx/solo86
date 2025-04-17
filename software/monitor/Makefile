include $(TOPDIR)/Makefile.conf

NAME = monitor.bin
CODE = monitor.asm
DEPS = beep.inc const.inc data.inc debug.inc delay.inc ihx.inc \
	   macro.inc mem.inc messages.inc rom.inc rtc.inc sound.inc \
	   stdio.inc system.inc test.inc unasm.inc vectors.inc

all: $(NAME)

$(NAME): $(CODE) $(DEPS)
	$(AS) $(CODE) -O9 -f bin -o $(NAME) -l monitor.lst
	$(CP) $(NAME) $(TOPDIR)/rom

clean:
	rm -f $(NAME)
	rm -f *.bin *.lst *.hex
