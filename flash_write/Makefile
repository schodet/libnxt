WARNINGS = -W -Wall
OPTIMIZE = -O3
CFLAGS = -mcpu=arm7tdmi -msoft-float -mapcs $(WARNINGS) $(OPTIMIZE)
ASFLAGS = --warn -mfpu=softfpa -mcpu=arm7tdmi -mapcs-32
LDFLAGS = --gc-sections $(OPTIMIZE)

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

all: flash.bin

flash.elf: crt0.o flash.o
	$(LD) $(LDFLAGS) $^ -o $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f flash.o crt0.o flash.elf flash.bin
