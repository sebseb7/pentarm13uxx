PROJECT=template


ifeq ($(OSTYPE),)
OSTYPE      = $(shell uname)
endif
ifneq ($(findstring Darwin,$(OSTYPE)),)
USB_DEVICE = $(shell ls /dev/cu.usbserial-A*)
else
USB_DEVICE = /dev/ttyUSB0
endif

LSCRIPT=core/lpc1347.ld

OPTIMIZATION = 2
DEBUG = -g

#########################################################################

SRC=$(wildcard core/*.c *.c drivers/*.c libs/*.c usb/*.c lpc_drivers/*.c) 
ASRC=$(wildcard core/*.s)
OBJECTS= $(SRC:.c=.o) $(ASRC:.s=.o)
HEADERS=$(wildcard core/*.h drivers/*.h libs/*.h usb/*.h lpc_drivers/*.h *.h) 

#  Compiler Options
GCFLAGS = -std=gnu99 -Wall -fno-common -mcpu=cortex-m3 -mthumb -O$(OPTIMIZATION) $(DEBUG) -Ilpc_drivers -I. -Iusb -Idrivers -Icore -Ilibs 
# -ffunction-sections -fdata-sections -fmessage-length=0   -fno-builtin
GCFLAGS += -D__RAM_MODE__=0  -D__BUILD_WITH_EXAMPLE__ 
LDFLAGS = -mcpu=cortex-m3 -mthumb -O$(OPTIMIZATION) -nostartfiles #-T$(LSCRIPT) 
ASFLAGS = -mcpu=cortex-m3 --defsym RAM_MODE=0

#  Compiler/Assembler Paths
GCC = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
REMOVE = rm -f
SIZE = arm-none-eabi-size

#########################################################################

all: tools/lpcrc/lpcrc firmware.bin Makefile stats

tools/lpcrc/lpcrc:
	make -C tools/lpcrc

firmware.bin: $(PROJECT).elf Makefile
	$(OBJCOPY) -R .stack -O binary $(PROJECT).elf firmware.bin
	tools/lpcrc/lpcrc firmware.bin
$(PROJECT).elf: $(OBJECTS) Makefile
	$(GCC) $(LDFLAGS) $(OBJECTS) -o $(PROJECT).elf

stats: $(PROJECT).elf Makefile
	$(SIZE) $(PROJECT).elf

clean:
	$(REMOVE) $(OBJECTS)
	$(REMOVE) firmware.bin
	$(REMOVE) $(PROJECT).elf
	make -C tools/lpcrc clean

#########################################################################

%.o: %.c Makefile $(HEADERS)
	$(GCC) $(GCFLAGS) -o $@ -c $<

%.o: %.s Makefile 
	$(AS) $(ASFLAGS) -o $@  $< 

#########################################################################

flash: firmware.bin
	cp firmware.bin /Volumes/CRP\ DISABLD/
	sync
	diskutil eject /dev/disk2

