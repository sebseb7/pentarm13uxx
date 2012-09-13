PROJECT=template

LDCRIPT=core/lpc1347.ld

OPTIMIZATION = 2
DEBUG = -g

#########################################################################

SRC=$(wildcard core/*.c *.c) 
OBJECTS= $(SRC:.c=.o) 
HEADERS=$(wildcard core/*.h *.h) 

#  Compiler Options
GCFLAGS = -std=gnu99 -Wall -fno-common -mcpu=cortex-m3 -mthumb -O$(OPTIMIZATION) $(DEBUG) -I. -Icore  
# -ffunction-sections -fdata-sections -fmessage-length=0   -fno-builtin
LDFLAGS = -mcpu=cortex-m3 -mthumb -O$(OPTIMIZATION) -nostartfiles -T$(LDCRIPT) 

#  Compiler/Assembler Paths
GCC = arm-none-eabi-gcc
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

#########################################################################

flash: firmware.bin
	cp firmware.bin /Volumes/CRP\ DISABLD/
	diskutil eject /dev/disk5

