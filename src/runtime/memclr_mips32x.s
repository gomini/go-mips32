
// +build mips32 mips32le

#include "textflag.h"

TEXT ·memclr(SB),NOSPLIT,$0-8
	MOVW	ptr+0(FP), R1
	MOVW	n+4(FP), R2
	ADDU	R1, R2, R3

	SGTU	$4, R2, R4
	BNE	R4, R0, tailx

align:
	AND	$3, R1, R4
	BEQ	R4, R0, aligned

	MOVB	R0, 0(R1)
	ADDU	$1, R1
	JMP	align

aligned:
	ADDU	$-31, R3, R4
aloop:
	SGTU	R4, R1, R5
	BEQ	R5, R0, tail

	MOVW	R0, 0(R1)
	MOVW	R0, 1(R1)
	MOVW	R0, 2(R1)
	MOVW	R0, 3(R1)
	ADDU	$8, R1
	MOVW	R0, -4(R1)
	MOVW	R0, -3(R1)
	MOVW	R0, -2(R1)
	MOVW	R0, -1(R1)

	JMP	aloop

tail:
	ADDU	$-3, R3, R4
tloop:
	SGTU	R4, R1, R5
	BEQ	R5, R0, tailx
	MOVW	R0, 0(R1)
	ADDU	$4, R1
	JMP	tloop

tailx:
	BEQ	R1, R3, end
	MOVB	R0, 0(R1)
	ADDU	$1, R1
	JMP	tailx

end:
	RET
