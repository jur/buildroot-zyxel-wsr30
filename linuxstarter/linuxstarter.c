#include "uart.h"

#define START_ADDR 0x80000000
#define KSEG_START_ADDR ((void *) 0xA0000000)
#define REG32(reg) (*(volatile unsigned int *)(reg))

#define CONFIG_SYS_CACHELINE_SIZE 32
#define _ICACHE_SIZE (64 * 1024)
#define _DCACHE_SIZE (32 * 1024)

/*
 * Cache Operations available on all MIPS processors with R4000-style caches
 */
#define Hit_Invalidate_I	0x10
#define Hit_Invalidate_D	0x11
#define Hit_Writeback_Inv_D	0x15

#define cache_op(op,addr) \
	__asm__ __volatile__( \
	"	.set	push\n" \
	"	.set	noreorder\n" \
	"	.set	mips3\n" \
	"	cache	%0, %1\n" \
	"	.set	pop\n" \
	: \
	: "i" (op), "R" (*(unsigned char *)(addr)))

unsigned char *linux_addr;
unsigned int linux_size;

void flush_cache(void);
void mymemcpy(unsigned char *dst, unsigned char *src, unsigned int size);

void _start(void)
{
	void (*entry)(int a0, int a1, int a2, int a3);
	volatile unsigned int *gimr0 = (void *) 0xB8003000;

	prints("Linuxstarter...\n");

	/* Disable interrupts */
	__asm__ __volatile__("di\n");
	*gimr0 = 0;

	/* disable ethernet switch */
	REG32(0xb8000010)= REG32(0xb8000010)&(~(1<<11));

	prints("Flushing cache...\n");
	flush_cache();

	/* Copy linux kernel */
	mymemcpy(KSEG_START_ADDR, linux_addr, linux_size);

	prints("Starting uncached.");
	entry = KSEG_START_ADDR;

	prints("Flushing cache...\n");
	flush_cache();

	prints("Jumping to ");
	printx((unsigned long) entry);
	prints("\n");

	/* watchdog ??? */
	REG32(0xb8003114)=0;
	/* disable ethernet switch */
	REG32(0xb8000010)&=~(1<<11);
	REG32(0xbbdc0300)=0xffffffff;
	REG32(0xbbdc0304)=0xffffffff;

	entry(0, 0, 0, 0);
}

void mymemcpy(unsigned char *dst, unsigned char *src, unsigned int size)
{
	unsigned int n;

	for (n = 0; n < size; n++) {
		dst[n] = src[n];
	}
}

void invalidate_dcache_range(unsigned long start_addr, unsigned long stop)
{
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	unsigned long addr = start_addr & ~(lsize - 1);
	unsigned long aend = (stop - 1) & ~(lsize - 1);

	while (1)
	{
		cache_op(Hit_Invalidate_D, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
}

void invalidate_icache_range(unsigned long start_addr, unsigned long stop)
{
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	unsigned long addr = start_addr & ~(lsize - 1);
	unsigned long aend = (stop - 1) & ~(lsize - 1);

	while (1)
	{
		cache_op(Hit_Invalidate_I, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
}

void invalidate_cache(void)
{
	invalidate_dcache_range(START_ADDR, START_ADDR + _DCACHE_SIZE);
	invalidate_icache_range(START_ADDR, START_ADDR + _ICACHE_SIZE);
}

void flush_cache_range(unsigned long start_addr, unsigned long size)
{
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	unsigned long addr = start_addr & ~(lsize - 1);
	unsigned long aend = (start_addr + size - 1) & ~(lsize - 1);

	/* aend will be miscalculated when size is zero, so we return here */
	if (size == 0)
		return;

	while (1)
	{
		cache_op(Hit_Writeback_Inv_D, addr);
		cache_op(Hit_Invalidate_I, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
}

void flush_cache(void)
{
	flush_cache_range(START_ADDR, _DCACHE_SIZE);

	invalidate_cache();
}

