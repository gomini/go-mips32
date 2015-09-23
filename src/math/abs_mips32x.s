
// +build mips32 mips32le

#include "textflag.h"

#ifdef GOARCH_mips32le
#define lo	0
#define hi	4
#else
#define lo	4
#define hi	0
#endif

TEXT ·Abs(SB),NOSPLIT,$0
	MOVW	lo(FP), R1
	MOVW	hi(FP), R2
	SLL	$1, R2, R2
	SRL	$1, R2, R2
	MOVW	R1, (lo+8)(FP)
	MOVW	R2, (hi+8)(FP)
	RET

