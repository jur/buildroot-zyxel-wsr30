CROSSCOMPILE = mips-linux-gnu-
CC = $(CROSSCOMPILE)gcc
AS = $(CROSSCOMPILE)as
CPP = $(CROSSCOMPILE)cpp
LD = $(CROSSCOMPILE)ld
OBJCOPY = $(CROSSCOMPILE)objcopy

MODS = linuxstarter uart
OBJS = $(addsuffix .o,$(MODS))

LOADERADDR = 0xA1F00000
LINUX_ADDR = 0xa0c00000
LINUX_SIZE = 0x00800000

CPPFLAGS += -mno-abicalls -fno-pic -O2

PROGRAM = linuxstarter_$(LOADERADDR)

.PHONY: all clean

all: $(PROGRAM).bin

vmlinux.bin: vmlinux
	$(CROSSCOMPILE)objcopy -O binary $^ $@

ld-script: ld-script.in
	sed <$^ >$@ -e "s/LOADERADDR/$(LOADERADDR)/g"

clean:
	rm -f $(PROGRAM).elf $(PROGRAM).bin $(PROGRAM).map $(OBJS) vmlinux.bin ld-script

$(PROGRAM).bin: $(PROGRAM).elf
	$(OBJCOPY) -O binary -j .text -j .data -j .rodata -j .bss $^ $(PROGRAM).bin

$(PROGRAM).elf: $(OBJS) ld-script
	$(LD) $(LDFLAGS) -Tld-script -Map=$(PROGRAM).map -o $@ $(OBJS)
