
// +build linux
// +build mips32 mips32le

#include "runtime.h"

void
runtime·gostartcall(Gobuf *gobuf, void (*fn)(void), void *ctxt)
{
	if(gobuf->lr != 0)
		runtime·throw("invalid use of gostartcall");
	gobuf->lr = gobuf->pc;
	gobuf->pc = (uintptr)fn;
	gobuf->ctxt = ctxt;
}

void
runtime·rewindmorestack(Gobuf *gobuf)
{
	USED(gobuf);
}
