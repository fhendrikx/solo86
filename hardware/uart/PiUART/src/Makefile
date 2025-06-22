#
# Makefile
#

CIRCLEHOME = ../circle

OBJS =  main.o kernel.o i2clogger.o keyboardtask.o \
        tcplistenertask.o tcptelnettask.o tcprawtask.o \
        libtelnet.o strbits.o \
        ter-i16n.o ter-i24b.o ter-i32b.o \
        cp437.o

LIBS =  $(CIRCLEHOME)/lib/usb/libusb.a \
        $(CIRCLEHOME)/lib/input/libinput.a \
        $(CIRCLEHOME)/lib/fs/libfs.a \
        $(CIRCLEHOME)/addon/display/libdisplay.a \
        $(CIRCLEHOME)/addon/wlan/hostap/wpa_supplicant/libwpa_supplicant.a \
        $(CIRCLEHOME)/addon/wlan/libwlan.a \
        $(CIRCLEHOME)/addon/fatfs/libfatfs.a \
        $(CIRCLEHOME)/addon/SDCard/libsdcard.a \
        $(CIRCLEHOME)/lib/net/libnet.a \
        $(CIRCLEHOME)/lib/sched/libsched.a \
        $(CIRCLEHOME)/lib/libcircle.a

include $(CIRCLEHOME)/Rules.mk

-include $(DEPS)
