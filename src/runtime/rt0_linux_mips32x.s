
// +build mips32 mips32le

#include "zasm_GOOS_GOARCH.h"
#include "funcdata.h"
#include "textflag.h"



TEXT _rt0_mips32_linux(SB),NOSPLIT, $-4
	JMP	_main(SB)

TEXT _rt0_mips32le_linux(SB),NOSPLIT,$-4
	JMP	_main(SB)

TEXT _main(SB),NOSPLIT,$-4
	MOVW	0(R29), R1
	MOVW	$4(R29), R2
	ADDU	$-4, R29
	JMP	runtime·rt0_go(SB)

TEXT main(SB),NOSPLIT,$-4
	JMP	_main(SB)
