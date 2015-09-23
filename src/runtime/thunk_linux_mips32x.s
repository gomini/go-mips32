
// +build linux
// +build mips32 mips32le


#include "zasm_GOOS_GOARCH.h"
#include "textflag.h"

TEXT sync∕atomic·swapUint64(SB),NOSPLIT,$0-20
	JMP	runtime·xchg64(SB)

TEXT sync∕atomic·compareAndSwapUint64(SB),NOSPLIT,$0-21
	JMP	runtime·cas64(SB)

TEXT sync∕atomic·addUint64(SB),NOSPLIT,$0-20
	JMP	runtime·xadd64(SB)

TEXT sync∕atomic·loadUint64(SB),NOSPLIT,$0-12
	JMP	runtime·atomicload64(SB)

TEXT sync∕atomic·storeUint64(SB),NOSPLIT,$0-12
	JMP	runtime·atomicstore64(SB)
