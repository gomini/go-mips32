
// +build linux
// +build mips32 mips32le

#include "runtime.h"
#include "defs_GOOS_GOARCH.h"
#include "os_GOOS.h"
#include "signals_GOOS.h"

#define REGG	30

#define SIG_CTXT(ctxt)	(((Ucontext*)(ctxt))->uc_mcontext)
#define SIG_REGS(ctxt)	(SIG_CTXT(ctxt).sc_regs)

#define SIG_Rx(ctxt, x)	(SIG_REGS(ctxt)[x])
#define SIG_FRx(ctxt, x)	(SIG_CTXT(ctxt).sc_fpregs[x])

#define SIG_LR(ctxt)	(SIG_REGS(ctxt)[31])
#define SIG_SP(ctxt)	(SIG_REGS(ctxt)[29])

#define SIG_FPCSR(ctxt)	(SIG_CTXT(ctxt).sc_fpc_csr)
#define SIG_PC(ctxt)	(SIG_CTXT(ctxt).sc_pc)
#define SIG_USEDMATH(ctxt)	(SIG_CTXT(ctxt).sc_used_math)
#define SIG_FPOWNED(ctxt)	(SIG_CTXT(ctxt).sc_acx)
#define SIG_HI(ctxt)	(SIG_CTXT(ctxt).sc_mdhi)
#define SIG_LO(ctxt)	(SIG_CTXT(ctxt).sc_mdlo)
#define SIG_CODE0(inf)	((uintptr)(info)->si_code)

void
runtime·dumpregs(Siginfo *info, void *ctxt)
{
	int32 i;

	USED(info);

	runtime·printf("pc	%x\n", (uint32)SIG_PC(ctxt));
	runtime·printf("fpowned	%x\n", (uint32)SIG_FPOWNED(ctxt));
	runtime·printf("fpcsr	%x\n", (uint32)SIG_FPCSR(ctxt));
	runtime·printf("fpused	%x\n", (uint32)SIG_USEDMATH(ctxt));

	for(i=0; i<nelem(SIG_REGS(ctxt)); i++) {
		switch(i) {
		case 29:
			runtime·printf("sp\t%x\n", (uint32)SIG_Rx(ctxt, i));
			break;
		case 31:
			runtime·printf("lr\t%x\n", (uint32)SIG_Rx(ctxt, i));
			break;
		default:
			runtime·printf("r%d\t%x\n", i, (uint32)SIG_Rx(ctxt, i));
			break;
		}
	}
	runtime·printf("hi	%x\n", (uint32)SIG_HI(ctxt));
	runtime·printf("lo	%x\n", (uint32)SIG_LO(ctxt));

}

void
runtime·sighandler(int32 sig, Siginfo *info, void *ctxt, G *gp)
{
	SigTab *t;
	bool crash;

	if(sig == SIGPROF) {
		runtime·sigprof((uint8*)(uintptr)SIG_PC(ctxt),
				(uint8*)(uintptr)SIG_SP(ctxt),
				(uint8*)(uintptr)SIG_LR(ctxt),
				gp,
				g->m);
		return;
	}

	t = &runtime·sigtab[sig];
	if(SIG_CODE0(info) != SI_USER && (t->flags & SigPanic)) {
		gp->sig = sig;
		gp->sigcode0 = SIG_CODE0(info);
		gp->sigcode1 = 0;
		gp->sigpc = SIG_PC(ctxt);

		SIG_SP(ctxt) -= 4;
		*(uint32*)(uintptr)SIG_SP(ctxt) = (uint32)SIG_LR(ctxt);
		if(gp->sigpc != 0)
			SIG_LR(ctxt) = gp->sigpc;
		SIG_Rx(ctxt, REGG) = (uint64)(uintptr)gp;
		SIG_PC(ctxt) = (uint64)(uintptr)runtime·sigpanic;
		return;
	}

	if(SIG_CODE0(info) == SI_USER || (t->flags & SigNotify))
		if(runtime·sigsend(sig))
			return;
	if(t->flags & SigKill)
		runtime·exit(2);
	if(!(t->flags & SigThrow))
		return;

	g->m->throwing = 1;
	g->m->caughtsig = gp;
	if(runtime·panicking)
		runtime·exit(2);
	runtime·panicking = 1;

	if(sig < 0 || sig >= NSIG)
		runtime·printf("Signal %d\n", sig);
	else
		runtime·printf("%s\n", runtime·sigtab[sig].name);

	runtime·printf("PC=%x\n", (uint32)SIG_PC(ctxt));
	if(g->m->lockedg != nil && g->m->ncgo > 0 && gp == g->m->g0) {
		runtime·printf("signal arrived during cgo execution\n");
		gp = g->m->lockedg;
	}
	runtime·printf("\n");

	if(runtime·gotraceback(&crash)){
		runtime·goroutineheader(gp);
		runtime·tracebacktrap(SIG_PC(ctxt), SIG_SP(ctxt), SIG_LR(ctxt), gp);
		runtime·tracebackothers(gp);
		runtime·printf("\n");
		runtime·dumpregs(info, ctxt);
	}
	
	if(crash)
		runtime·crash();

	runtime·exit(2);
}
