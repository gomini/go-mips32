
// +build mips32 mips32le

#include "textflag.h"

TEXT ·memmove(SB),NOSPLIT,$-4-12
	MOVW	t+0(FP), R1
	MOVW	s+4(FP), R2
	MOVW	n+8(FP), R3
	BNE	R3, R0, check
	RET
check:
	SGTU	R2, R1, R4
	BEQ	R4, R0, backward

	ADDU	R2, R3, R3
loop:
	MOVBU	0(R2), R4
	ADDU	$1, R2
	MOVB	R4, 0(R1)
	ADDU	$1, R1
	BNE	R2, R3, loop
	RET

backward:
	ADDU	R3, R1
	ADDU	R2, R3
loop1:
	MOVBU	-1(R3), R4
	ADDU	$-1, R3
	MOVBU	R4, -1(R1)
	ADDU	$-1, R1
	BNE	R3, R2, loop1
	RET
