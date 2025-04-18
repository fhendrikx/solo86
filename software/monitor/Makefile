include $(TOPDIR)/Makefile.conf

NAME = monitor.bin
CODE = monitor.asm
DEPS = beep.inc const.inc data.inc debug.inc delay.inc ihx.inc \
	   macro.inc mem.inc messages.inc rom.inc rtc.inc sound.inc \
	   stdio.inc system.inc test.inc unasm.inc vectors.inc version.inc

all: $(NAME)

version.inc:
	rm -f version.inc
	printf >> version.inc \
		"mon_version db 0Ah, 'Solo/86 Release 1.0.0, $(shell git log --abbrev-commit | head -1)', 0Ah\n"
	printf >> version.inc \
		"            db 'Built on $(shell date '+%Y-%b-%d @ %T %z')', 0Ah, 0Ah, 00h\n"

$(NAME): $(CODE) $(DEPS)
	$(AS) $(CODE) -O9 -f bin -o $(NAME) -l monitor.lst
	$(CP) $(NAME) $(TOPDIR)/rom

clean:
	rm -f $(NAME)
	rm -f *.bin *.lst *.hex
	rm -f version.inc
