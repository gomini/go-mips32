
// +build mips32 mips32le

#include "textflag.h"
#include "funcdata.h"

TEXT ·makeFuncStub(SB),(NOSPLIT|WRAPPER),$8
	NO_LOCAL_POINTERS
	MOVW	R7, 4(R29)
	MOVW	$argframe+0(FP), R1
	MOVW	R1, 8(R29)
	JAL	·callReflect(SB)
	RET

TEXT ·methodValueCall(SB),(NOSPLIT|WRAPPER),$8
	NO_LOCAL_POINTERS
	MOVW	R7, 4(R29)
	MOVW	$argframe+0(FP), R1
	MOVW	R1, 8(R29)
	JAL	·callMethod(SB)
	RET
