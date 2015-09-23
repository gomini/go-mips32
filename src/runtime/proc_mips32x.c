
// +build mips32 mips32le

#include "runtime.h"
#include "arch_GOARCH.h"
#include "zaexperiment.h"
#include "malloc.h"
#include "stack.h"
#include "race.h"
#include "type.h"
#include "mgc0.h"
#include "textflag.h"

extern M runtime·m0;

#ifdef GOARCH_mips32le
static uint8 fregs[] = {
	0x0,0x0,0x0,0x0,0x0,0x0,0xe0,0x3f,
	0x0,0x0,0x0,0x0,0x0,0x0,0xf0,0x3f,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40,
};
#else
static uint8 fregs[] = {
	0x3f,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x3f,0xf0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
};
#endif

static uint32
high(int32 n)
{
	uint64 *p;

	p = (uint64*)fregs;
	return (uint32)(p[n]>>32);
}

static uint32
low(int32 n)
{
	uint64 *p;

	p = (uint64*)fregs;
	return (uint32)(p[n]);
}

#pragma textflag NOSPLIT
void
runtime·minitfregs(M *mp)
{
	mp->freglo[24>>1] = 0;
	mp->freghi[24>>1] = 0;

	mp->freglo[26>>1] = low(0);
	mp->freghi[26>>1] = high(0);

	mp->freglo[28>>1] = low(1);
	mp->freghi[28>>1] = high(1);

	mp->freglo[30>>1] = low(2);
	mp->freghi[30>>1] = high(2);
}

#pragma textflag NOSPLIT
void
runtime·initm0(void)
{
	runtime·minitfregs(&runtime·m0);
}
