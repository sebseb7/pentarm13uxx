PROJECT=template

LDCRIPT=core/lpc1347.ld

OPTIMIZATION = 1

#########################################################################

SRC=$(wildcard core/*.c usb/*.c *.c) 
OBJECTS= $(SRC:.c=.o) 
LSSFILES= $(SRC:.c=.lst) 
HEADERS=$(wildcard core/*.h usb/*.h *.h) 

#  Compiler Options
GCFLAGS = -ffreestanding -std=gnu99 -mcpu=cortex-m3 -mthumb -O$(OPTIMIZATION) -I. -Icore -Iusb
# Warnings
GCFLAGS += -Wstrict-prototypes -Wundef -Wall -Wextra -Wunreachable-code  
# Optimizazions
GCFLAGS += -fsingle-precision-constant -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fno-builtin -ffunction-sections -fdata-sections -fno-common
# Debug stuff
GCFLAGS += -Wa,-adhlns=$(<:.c=.lst),-gstabs -g 


LDFLAGS =  -mcpu=cortex-m3 -mthumb -O$(OPTIMIZATION) -nostartfiles -nostdlib -nodefaultlibs -T$(LDCRIPT) 


#  Compiler/Linker Paths
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
	$(REMOVE) $(LSSFILES)
	$(REMOVE) firmware.bin
	$(REMOVE) $(PROJECT).elf
	$(REMOVE) $(PROJECT).map
	make -C tools/lpcrc clean

#########################################################################

%.o: %.c Makefile $(HEADERS)
	$(GCC) $(GCFLAGS) -o $@ -c $<

#########################################################################

flash: firmware.bin
	cp firmware.bin /Volumes/CRP\ DISABLD/
	diskutil eject `diskutil list | grep -B 2 CRP | grep dev`

