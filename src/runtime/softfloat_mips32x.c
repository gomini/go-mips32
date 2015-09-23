
// +build mips32 mips32le


#include "runtime.h"
#include "textflag.h"

void	runtime·abort(void);
void	_sfloat(void);

enum {
	trace = 0,
};

static void
fabort(void)
{
	if (1) {
		runtime·printf("Unsupported floating point instruction\n");
		runtime·abort();
	}
}


static void
putf(uint32 reg, uint32 val)
{
	if(reg & 1)
		g->m->freglo[reg>>1] = val;
	else
		g->m->freghi[reg>>1] = val;
}

static uint32
getf(uint32 reg)
{
	if(reg & 1)
		return g->m->freglo[reg>>1];
	return g->m->freghi[reg>>1];
}

static void
putd(uint32 reg, uint64 val)
{
	if(trace && (reg&1)) {
		runtime·printf("using FR%d as double in O32\n", reg);
		runtime·abort();
	}

	reg = reg>>1;
	g->m->freglo[reg] = (uint32)val;
	g->m->freghi[reg] = (uint32)(val>>32);
}

static uint64
getd(uint32 reg)
{
	if(trace && (reg&1)) {
		runtime·printf("using FR%d as double in O32\n", reg);
		runtime·abort();
	}

	reg = reg>>1;
	return (uint64)g->m->freglo[reg] | ((uint64)g->m->freghi[reg]<<32);
}

static void
storedreg(uint32 *addr, uint32 reg)
{
	if(trace && (reg&1)) {
		runtime·printf("using FR%d as double in O32\n", reg);
		runtime·abort();
	}
	((uint64*)addr)[0] = getd(reg);
}

static void
fprint(void)
{
	uint32 i;
	for (i = 0; i < 16; i++) {
		runtime·printf("\tf%d:\t%X %X\n", i, g->m->freghi[i], g->m->freglo[i]);
	}
}

static uint32
d2f(uint64 d)
{
	uint32 x;

	runtime·f64to32c(d, &x);
	return x;
}

static uint64
f2d(uint32 f)
{
	uint64 x;

	runtime·f32to64c(f, &x);
	return x;
}

static uint32
getreg(uint32 *regs, uint32 n)
{
	if(n == 0)
		return 0;
	if(n <= 25 || n == 29 || n == 30)
		return regs[n-1];
	runtime·printf("invalid read: R%d in fp\n", n);
	runtime·abort();
	return 0;
}

static void
setreg(uint32 *regs, uint32 n, uint32 v)
{
	if(n==0)
		return;
	if(n <= 25 || n == 29 || n == 30) {
		regs[n-1] = v;
		return;
	}
	runtime·printf("invalid write: R%d in fp\n", n);
	runtime·abort();
}

#define FAULT		(0x80000000U) // impossible PC offset

#define	OP(x,y)		(((x)<<3)|((y)<<0))
#define	SP(x,y)		(((x)<<29)|((y)<<26))

#define	FPF(x,y)	(SP(2,1)|(16<<21)|((x)<<3)|((y)<<0))
#define	FPD(x,y)	(SP(2,1)|(17<<21)|((x)<<3)|((y)<<0))
#define	FPW(x,y)	(SP(2,1)|(20<<21)|((x)<<3)|((y)<<0))

#define FT(i)		(((i) & (31<<16))>>16)
#define FS(i)		(((i) & (31<<11))>>11)
#define FD(i)		(((i) & (31<<6))>>6)
#define FB(i)		(((i) & (31<<21))>>21)

#define DELTA(i)	((((int32)(i) & 0xffff)<<16)>>16)

enum {
	OR_31_0_25	= (31<<21)|(25<<11)|37,
	OR_0_31_25	= (31<<16)|(25<<11)|37,
};

static uint32
stepflt(uint32 *pc, uint32 *regs, uint32 delay)
{
	uint32 i, ii, b, s, t, d;
	int32 delta;
	uint32 *addr;
	uint64 uval;
	int64 sval;
	bool nan, ok;
	int32 cmp;
	M *m;

	m = g->m;
	i = *pc;


	if(i == 4 || (i & ~(31<<6)) == 0) {
		if(trace)
			runtime·printf("\t%p\tnop\n", pc);
		return 1;
	}

	if(i == (SP(0,3) | (((uint32)_sfloat)>>2))) {
		ii = ((uint32)_sfloat)>>2;
		ii |= SP(0,3);
		if(ii == pc[1]) {
			if(trace)
				runtime·printf("\t%p\tENTER _sfloat(SB)\n", pc);
			return 2;
		}
	}

	if((i & (SP(7,7)|(31<<21))) == SP(1,7)) {
		d = (i & (31<<16))>>16;
		setreg(regs, d, (i&0xffff)<<16);

		if(trace)
			runtime·printf("\t%p\tLUI $%x, R%d\n", pc, i&0xffff, d);
		return 1;
	}

	if((i & (SP(7,7)|63)) == 33) {
		s = (i & (31<<21))>>21;
		t = (i & (31<<16))>>16;
		d = (i & (31<<11))>>11;
		setreg(regs, d, getreg(regs, s) + getreg(regs, t));

		if(trace)
			runtime·printf("\t%p\tADDU R%d, R%d, R%d\n", pc, s, t, d);
		return 1;
	}
	if((i & SP(7,7)) == SP(1,1)) {
		s = (i & (31<<21))>>21;
		d = (i & (31<<16))>>16;
		delta = DELTA(i);
		setreg(regs, d, getreg(regs, s) + delta);

		if(trace)
			runtime·printf("\t%p\tADDU $%d, R%d, R%d\n", pc, delta, s, d);
		return 1;
	}
	if((i & SP(7,7)) == SP(1,5)) {
		s = (i & (31<<21))>>21;
		d = (i & (31<<16))>>16;
		setreg(regs, d, getreg(regs, s)|(i&0xffff));

		if(trace)
			runtime·printf("\t%p\tOR $%d, R%d, R%d\n", pc, i&0xffff, s, d);
		return 1;
	}

	if(trace)
		runtime·printf("stepflt %p %x [%d, %d, %d, %d, %d, %d]\n",
			pc, i,
			(i&(63<<26))>>26, (i&(31<<21))>>21, (i&(31<<16))>>16,
			(i&(31<<11))>>11, (i&(31<<6))>>6, i&63);

	b = FB(i);
	t = FT(i);
	switch(i & SP(7,7)) {
	case SP(7,1):
		delta = DELTA(i);
		addr = (uint32*)(getreg(regs, b) + delta);
		if(((uintptr)addr & 3) != 0 || (uintptr)addr < 4096) {
			if(trace)
				runtime·printf("\tS.S @%p (FAULT)\n", addr);
			return FAULT;
		}
		addr[0] = getf(t);
		if(trace)
			runtime·printf("\tS.S F%d, %d(R%d) @%p = %x\n",
				t, delta, b, addr, addr[0]);
		return 1;

	case SP(7,5):
		delta = DELTA(i);
		addr = (uint32*)(getreg(regs, b) + delta);
		if(((uintptr)addr & 7) != 0 || (uintptr)addr < 4096) {
			if(trace)
				runtime·printf("\tS.D @%p [FAULT]\n", addr);
			return FAULT;
		}
		storedreg(addr, t);
		if(trace)
			runtime·printf("\tS.D F%d, %d(R%d) @%p = %x-%x\n",
				t, delta, b, addr, m->freghi[t>>1], m->freglo[t>>1]);
		return 1;

	case SP(6,1):
		delta = DELTA(i);
		addr = (uint32*)(getreg(regs, b) + delta);
		if(((uintptr)addr & 3) != 0 || (uintptr)addr < 4096) {
			if(trace)
				runtime·printf("\tL.S @%p =>fault\n", addr);
			return FAULT;
		}
		putf(t, addr[0]);
		if(trace)
			runtime·printf("\tL.S %d(R%d), F%d @%p = %x\n",
				delta, b, t, addr, addr[0]);
		return 1;

	case SP(6,5):
		delta = DELTA(i);
		addr = (uint32*)(getreg(regs, b) + delta);
		if(((uintptr)addr & 7) != 0 || (uintptr)addr < 4096) {
			if(trace)
				runtime·printf("\tL.D @%p [FAULT]\n", addr);
			return FAULT;
		}
		putd(t, *(uint64*)addr);
		if(trace)
			runtime·printf("\tL.D %d(R%d), F%d @%p = %x-%x\n",
				delta, b, t, addr, m->freghi[t>>1], m->freglo[t>>1]);
		return 1;
	}

	switch(i & (SP(7,7) | (31<<21))) {
	case SP(2,1) | (4<<21):
		s = (i & (31<<16))>>16;
		d = (i & (31<<11))>>11;
		putf(d, getreg(regs, s));
		if(trace)
			runtime·printf("\tMTC1 R%d, FR%d\n", s, d);
		return 1;

	case SP(2,1):
		s = (i & (31<<16))>>16;
		d = (i & (31<<11))>>11;
		setreg(regs, s, getf(d));
		if(trace)
			runtime·printf("\tMFC1 f%d, r%d\n", d, s);
		return 1;

	case SP(2,1) | (2<<21):
		if(trace)
			runtime·printf("\tCFC1 (no effect)\n");
		return 1;
	case SP(2,1) | (6<<21):
		if(trace)
			runtime·printf("\tCTC1 (no effect)\n");
		return 1;

	case SP(2,1) | (8<<21):
		if(delay) {
			return FAULT;
		}
		delta = 2;
		if((i & (31<<16)) == 0) {
			if((m->fflag & 1) == 0)
				delta = DELTA(i) + 1;
			
			if(trace)
				runtime·printf("\tBC1F delta=%d, dst=%p\n", delta, pc+delta);

			if(stepflt(pc+1, regs, 1)!=1)
				return FAULT;
			return delta;
		} else if((i & (31<<16)) == (1<<16)) {
			if(m->fflag & 1)
				delta = DELTA(i) + 1;

			if(trace)
				runtime·printf("\tBC1T delta=%x, dst=%p\n", delta, pc+delta);

			if(stepflt(pc+1, regs, 1)!=1)
				return FAULT;
			return delta;
		}
		if(trace)
			runtime·printf("unimpl. BC1x\n");
		return FAULT;
	}

	s = FS(i);
	t = FT(i);
	d = FD(i);
	switch(i & (SP(7,7) | (31<<21) | 63)) {
	case FPF(0,3):
		if(trace)
			runtime·printf("\tDIVF F%d = F%d / FR%d %x\n",
				d, s, t, getf(d));
		runtime·fdiv64c(f2d(getf(s)), f2d(getf(t)), &uval);
		putf(d, d2f(uval));
		if(trace)
			runtime·printf("\tDIVF F%d = F%d / FR%d %x\n",
				d, s, t, getf(d));
		return 1;

	case FPF(0,2):
		runtime·fmul64c(f2d(getf(s)), f2d(getf(t)), &uval);
		putf(d, d2f(uval));
		if(trace)
			runtime·printf("\tMULF F%d = F%d * F%d %x\n",
				d, s, t, getf(d));
		return 1;

	case FPF(0,1):
		runtime·fsub64c(f2d(getf(s)), f2d(getf(t)), &uval);
		if(trace)
			runtime·printf("SUBF=%X\n", uval);
		putf(d, d2f(uval));
		if(trace)
			runtime·printf("\tSUBF F%d [%x] = F%d [%x] - F%d [%x]\n", d, getf(d), s, getf(s), t, getf(t));
		return 1;

	case FPF(0,0):
		runtime·fadd64c(f2d(getf(s)), f2d(getf(t)), &uval);
		putf(d, d2f(uval));
		if(trace)
			runtime·printf("\tADDF F%d = F%d + F%d %x\n", d, s, t, getf(d));
		return 1;

	case FPF(0,5):
		putf(d, getf(s) & ~(1U<<31));
		if(trace)
			runtime·printf("\tABSF F%d = F%d %x\n", d, s, getf(d));
		return 1;

	case FPF(0,7):
		putf(d, getf(s) ^ (1U<<31));
		if(trace)
			runtime·printf("\tNEGF F%d = F%d %x\n", d, s, getf(d));
		return 1;

	case FPF(0,6):
		putf(d, getf(s));
		if(trace)
			runtime·printf("\tMOVF F%d = F%d %x\n", d, s, getf(d));
		return 1;

	case FPD(0,3):
		runtime·fdiv64c(getd(s), getd(t), &uval);
		putd(d, uval);
		if(trace)
			runtime·printf("\tDIVD F%d = F%d / F%d %x-%x\n",
				d, s, t, m->freghi[d>>1], m->freglo[d>>1]);
		return 1;

	case FPD(0,2):
		runtime·fmul64c(getd(s), getd(t), &uval);
		putd(d, uval);
		if(trace)
			runtime·printf("\tMULD F%d = F%d * F%d %x-%x\n",
				d, s, t, m->freghi[d>>1], m->freglo[d>>1]);
		return 1;

	case FPD(0,1):
		runtime·fsub64c(getd(s), getd(t), &uval);
		putd(d, uval);
		if(trace)
			runtime·printf("\tSUBD F%d = F%d - F%d %x-%x\n",
				d, s, t, m->freghi[d>>1], m->freglo[d>>1]);
		return 1;

	case FPD(0,0):
		runtime·fadd64c(getd(s), getd(t), &uval);
		putd(d, uval);
		if(trace)
			runtime·printf("\tADDD F%d = F%d + F%d %x-%x\n",
				d, s, t, m->freghi[d>>1], m->freglo[d>>1]);
		return 1;

	case FPD(4,0):
		putf(d, d2f(getd(s)));
		if(trace)
			runtime·printf("\tMOVDF F%d, F%d %x\n", s, d, getf(d));
		return 1;

	case FPF(4,1):
		putd(d, f2d(getf(s)));
		if(trace)
			runtime·printf("\tMOVFD F%d, F%d %x-%x\n",
				s, d, m->freghi[d>>1], m->freglo[d>>1]);
		return 1;

	case FPF(4,4):
		runtime·f64tointc(f2d(getf(s)), &sval, &ok);
		if(!ok || (int32)sval != sval)
			sval = 0;
		putf(d, sval);
		if(trace)
			runtime·printf("\tMOVFW F%d, F%d %x\n",
				s, d, getf(d));
		return 1;

	case FPW(4,0):
		cmp = getf(s);
		if(cmp < 0) {
			runtime·fintto64c(-cmp, &uval);
			putf(d, d2f(uval) ^ (1U<<31));
		} else {
			runtime·fintto64c(cmp, &uval);
			putf(d, d2f(uval));
		}
		if(trace)
			runtime·printf("\tMOVWF F%d, F%d %x\n", s, d, getf(d));
		return 1;

	case FPD(4,4):
		runtime·f64tointc(getd(s), &sval, &ok);
		if(!ok || (int32)sval != sval)
			sval = 0;
		putf(d, sval);
		if(trace)
			runtime·printf("\tMOVDW F%d, F%d %x\n", s, d, getf(d));
		return 1;

	case FPW(4,1):
		cmp = getf(s);
		if(cmp < 0) {
			runtime·fintto64c(-cmp, &uval);
			putd(d, uval);
			m->freghi[d>>1] ^= 0x80000000U;
		} else {
			runtime·fintto64c(cmp, &uval);
			putd(d, uval);
		}
		if(trace)
			runtime·printf("\tMOVWD F%d, F%d %x-%x\n",
				s, d, m->freghi[d>>1], m->freglo[d>>1]);
		return 1;

	case FPD(0,5):
		if(trace && ((d & 1) || (s & 1))) {
			runtime·printf("ABSD %d, %d (illegal in O32)\n", s, d);
			runtime·abort();
		}
		d = d>>1;
		s = s>>1;
		m->freglo[d] = m->freglo[s];
		m->freghi[d] = m->freghi[s] & 0x7fffffffU;
		if(trace)
			runtime·printf("\tABSD F%d, F%d %x-%x\n",
				s<<1, d<<1, m->freghi[s], m->freglo[d]);
		return 1;

	case FPD(0,7):
		if(trace && ((d & 1) || (s & 1))) {
			runtime·printf("NEGD %d, %d (illegal in O32)\n", s, d);
			runtime·abort();
		}
		d = d>>1;
		s = s>>1;
		m->freglo[d] = m->freglo[s];
		m->freghi[d] = m->freghi[s] ^ (1U<<31);
		if(trace)
			runtime·printf("\tNEGD F%d, F%d %x-%x\n",
				s<<1, d<<1, m->freghi[s], m->freglo[d]);
		return 1;

	case FPD(0,6):
		if(trace && ((d & 1) || (s & 1))) {
			runtime·printf("MOVD %d, %d (illegal in O32)\n", s, d);
			runtime·abort();
		}
		d = d>>1;
		s = s>>1;
		m->freglo[d] = m->freglo[s];
		m->freghi[d] = m->freghi[s];
		if(trace)
			runtime·printf("\tMOVD F%d, F%d %x\n", s<<1, d<<1, m->freghi[s]);
		return 1;

	case FPF(6,2):
		runtime·fcmp64c(f2d(getf(t)), f2d(getf(s)), &cmp, &nan);
		if(cmp == 0 && !nan)
			m->fflag |= 1;
		else
			m->fflag &= ~1;
		if(trace)
			runtime·printf("\tCMPEQF F%d, F%d\t[%c]\n",
				s, t, m->fflag ? 'T':'F');
		return 1;

	case FPF(7,4):
		runtime·fcmp64c(f2d(getf(t)), f2d(getf(s)), &cmp, &nan);
		if(cmp > 0 && !nan)
			m->fflag |= 1;
		else
			m->fflag &= ~1;
		if(trace)
			runtime·printf("\tCMPGTF F%d, F%d\t[%c]\n",
				s, t, m->fflag ? 'T':'F');
		return 1;

	case FPF(7,6):
		runtime·fcmp64c(f2d(getf(t)), f2d(getf(s)), &cmp, &nan);
		if(cmp >= 0 && !nan)
			m->fflag |= 1;
		else
			m->fflag &= ~1;
		if(trace)
			runtime·printf("\tCMPGEF F%d, F%d\t[%c]\n",
				s, t, m->fflag ? 'T':'F');
		return 1;

	case FPD(6,2):
		runtime·fcmp64c(getd(t), getd(s), &cmp, &nan);
		if(cmp == 0 && !nan)
			m->fflag |= 1;
		else
			m->fflag &= ~1;
		if(trace)
			runtime·printf("\tCMPEQD F%d, F%d\t[%c]\n",
				s, t, m->fflag ? 'T':'F');
		return 1;

	case FPD(7,4):
		runtime·fcmp64c(getd(t), getd(s), &cmp, &nan);
		if(cmp > 0 && !nan)
			m->fflag |= 1;
		else
			m->fflag &= ~1;
		if(trace)
			runtime·printf("\tCMPGTD F%d, F%d\t[%c]\n",
				s, t, m->fflag ? 'T':'F');
		return 1;

	case FPD(7,6):
		runtime·fcmp64c(getd(t), getd(s), &cmp, &nan);
		if(cmp >= 0 && !nan)
			m->fflag |= 1;
		else
			m->fflag &= ~1;
		if(trace)
			runtime·printf("\tCMPGED F%d, F%d\t[%c]\n",
				s, t, m->fflag ? 'T':'F');
		return 1;
	}
	if(trace)
		runtime·printf("NOT FP\n");
	return 0;
}

typedef struct Sregs Sregs;
struct Sregs
{
	uint32	*r1;
	uint32	*r2;
	uint32	*r3;
	uint32	*r4;
	uint32	*r5;
	uint32	*r6;
	uint32	*r7;
	uint32	*r8;
	uint32	*r9;
	uint32	*r10;
	uint32	*r11;
	uint32	*r12;
	uint32	*r13;
	uint32	*r14;
	uint32	*r15;
	uint32	*r16;
	uint32	*r17;
	uint32	*r18;
	uint32	*r19;
	uint32	*r20;
	uint32	*r21;
	uint32	*r22;
	uint32	*r23;
	uint32	*r24;
	uint32	*r25;
	uint32	*r26;
	uint32	*r27;
	uint32	*r28;
	uint32	*r29;
	uint32	*r30;
	uint32	*hi;
	uint32	*lo;
	uint32	*oldlr;
};

static void sfloat2(void);
void _sfloatpanic(void);

#pragma textflag NOSPLIT
uint32*
runtime·_sfloat2(uint32 *pc, Sregs regs)
{
	void (*fn)(void);
	
	g->m->ptrarg[0] = pc;
	g->m->ptrarg[1] = &regs;
	fn = sfloat2;
	runtime·onM(&fn);
	pc = g->m->ptrarg[0];
	g->m->ptrarg[0] = nil;
	return pc;
}

static void
sfloat2(void)
{
	uint32 *pc;
	G *curg;
	Sregs *regs;
	int32 skip;
	bool first;
	
	pc = g->m->ptrarg[0];
	regs = g->m->ptrarg[1];
	g->m->ptrarg[0] = nil;
	g->m->ptrarg[1] = nil;

	first = true;
	while(skip = stepflt(pc, (uint32*)&regs->r1, 0)) {
		first = false;
		if(skip == FAULT) {
			enum { SIGSEGV = 11 };
			curg = g->m->curg;
			curg->sig = SIGSEGV;
			curg->sigcode0 = 0;
			curg->sigcode1 = 0;
			curg->sigpc = (uint32)pc;
			pc = (uint32*)_sfloatpanic;
			break;
		}
		pc += skip;
	}
	if(first) {
		runtime·printf("sfloat2 %p %x\n", pc, *pc);
		fabort();
	}
		
	g->m->ptrarg[0] = pc;
}
