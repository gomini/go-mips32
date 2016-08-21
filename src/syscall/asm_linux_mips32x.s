
// +build linux
// +build mips32 mips32le

#include "textflag.h"
#include "funcdata.h"

TEXT ·Syscall(SB),NOSPLIT,$0-28
	JAL	runtime·entersyscall(SB)
	MOVW	0(FP), R2
	MOVW	4(FP), R4
	MOVW	8(FP), R5
	MOVW	12(FP), R6
	MOVW	R29, R16
	ADDU	$-32, R29
	AND	$-8, R29
	MOVW	R29, R17
	MOVW	R0, R7
	MOVW	R2, 28(R29)

	NOSCHED
	MOVW	28(R29), R2
	SYSCALL
	SCHED

	BEQ	R17, R29, 2(PC)
	MOVW	R0, (R0)
	MOVW	R16, R29

	BEQ	R7, R0, ok

	MOVW	$0, R1		// R1 = 0
	SGT	R2, R1, R23 // R2 < R1 => R23 (errno < 0)
	BNE	R23, perrno // jump if R23 not true (errno > 0)
	MOVW	$-1, R1		// R1 = -1 (abs)
	MUL   R2, R1		// R2 * R1 (abs)
	MOVW  LO, R2		// R2 = LO (abs)
perrno:
	MOVW	R2, 24(FP)	// errno
	MOVW	$-1, R1
	MOVW	R1, 16(FP)
	MOVW	R0, 20(FP)
	JAL	runtime·exitsyscall(SB)
	RET
ok:
	MOVW	R2, 16(FP)
	MOVW	R3, 20(FP)
	MOVW	R0, 24(FP)	// errno
	JAL	runtime·exitsyscall(SB)
	RET

TEXT	·Syscall6(SB),NOSPLIT,$0-40
	JAL	runtime·entersyscall(SB)
	MOVW	0(FP), R2
	MOVW	4(FP), R4
	MOVW	8(FP), R5
	MOVW	12(FP), R6
	MOVW	16(FP), R7
	MOVW	20(FP), R8
	MOVW	24(FP), R9
	MOVW	R29, R16
	ADDU	$-32, R29
	AND	$-8, R29
	MOVW	R29, R17
	MOVW	R8, 16(R29)
	MOVW	R9, 20(R29)
	MOVW	R2, 28(R29)

	NOSCHED
	MOVW	28(R29), R2
	SYSCALL
	SCHED

	BEQ	R29, R17, 2(PC)
	MOVW	R0, (R0)

	MOVW	R16, R29
	BEQ	R7, R0, ok

	MOVW	$0, R1		// R1 = 0
	SGT	R2, R1, R23 // R2 < R1 => R23 (errno < 0)
	BNE	R23, perrno // jump if R23 not true (errno > 0)
	MOVW	$-1, R1		// R1 = -1 (abs)
	MUL   R2, R1		// R2 * R1 (abs)
	MOVW  LO, R2		// R2 = LO (abs)
perrno:	
	MOVW	R2, 36(FP)	// errno
	MOVW	$-1, R1
	MOVW	R1, 28(FP)
	MOVW	R0, 32(FP)
	JAL	runtime·exitsyscall(SB)
	RET
ok:
	MOVW	R2, 28(FP)
	MOVW	R3, 32(FP)
	MOVW	R0, 36(FP)	// errno
	JAL	runtime·exitsyscall(SB)
	RET

TEXT	·RawSyscall6(SB),NOSPLIT,$0-40
	MOVW	0(FP), R2
	MOVW	4(FP), R4
	MOVW	8(FP), R5
	MOVW	12(FP), R6
	MOVW	16(FP), R7
	MOVW	20(FP), R8
	MOVW	24(FP), R9
	MOVW	R29, R16
	ADDU	$-32, R29
	AND	$-8, R29
	MOVW	R29, R17
	MOVW	R8, 16(R29)
	MOVW	R9, 20(R29)
	MOVW	R2, 28(R29)

	NOSCHED
	MOVW	28(R29), R2
	SYSCALL
	SCHED

	BEQ	R29, R17, 2(PC)
	MOVW	R0, 0(R0)

	MOVW	R16, R29
	BEQ	R7, R0, ok

	MOVW	$0, R1		// R1 = 0
	SGT	R2, R1, R23 // R2 < R1 => R23 (errno < 0)
	BNE	R23, perrno // jump if R23 not true (errno > 0)
	MOVW	$-1, R1		// R1 = -1 (abs)
	MUL   R2, R1		// R2 * R1 (abs)
	MOVW  LO, R2		// R2 = LO (abs)
perrno:	
	MOVW	R2, 36(FP)	// errno
	MOVW	$-1, R1
	MOVW	R1, 28(FP)
	MOVW	R0, 32(FP)
	RET
ok:
	MOVW	R2, 28(FP)
	MOVW	R3, 32(FP)
	MOVW	R0, 36(FP)	// errno
	RET

TEXT ·RawSyscall(SB),NOSPLIT,$0-28
	MOVW	0(FP), R2
	MOVW	4(FP), R4
	MOVW	8(FP), R5
	MOVW	12(FP), R6
	MOVW	R29, R16
	ADDU	$-32, R29
	AND	$-8, R29
	MOVW	R29, R17
	MOVW	R0, R7
	MOVW	R2, 28(R29)

	NOSCHED
	MOVW	28(R29), R2
	SYSCALL
	SCHED

	BEQ	R29, R17, 2(PC)
	MOVW	R0, 0(R0)

	MOVW	R16, R29
	BEQ	R7, R0, ok

	MOVW	$0, R1		// R1 = 0
	SGT	R2, R1, R23 // R2 < R1 => R23 (errno < 0)
	BNE	R23, perrno // jump if R23 not true (errno > 0)
	MOVW	$-1, R1		// R1 = -1 (abs)
	MUL   R2, R1		// R2 * R1 (abs)
	MOVW  LO, R2		// R2 = LO (abs)
perrno:
	MOVW	R2, 24(FP)	// errno
	MOVW	$-1, R1
	MOVW	R1, 16(FP)
	MOVW	R0, 20(FP)
	RET
ok:
	MOVW	R2, 16(FP)
	MOVW	R3, 20(FP)
	MOVW	R0, 24(FP)	// errno
	RET

#define SYS__LLSEEK	4140

TEXT ·seek(SB),NOSPLIT,$0-28
	JAL	runtime·entersyscall(SB)
	MOVW	$SYS__LLSEEK, R2
	MOVW	0(FP), R4
#ifdef GOARCH_mips32le
	MOVW	4(FP), R6
	MOVW	8(FP), R5
#else
	MOVW	4(FP), R5
	MOVW	8(FP), R6
#endif
	MOVW	$16(FP), R7
	MOVW	12(FP), R8

	MOVW	R29, R16
	ADDU	$-32, R29
	AND	$-8, R29
	MOVW	R8, 16(R29)
	MOVW	R2, 28(R29)

	NOSCHED
	MOVW	28(R29), R2
	SYSCALL
	SCHED

	MOVW	R16, R29
	BEQ	R7, R0, ok
	MOVW	R0, 16(FP)
	MOVW	R0, 20(FP)
	MOVW	R2, 24(FP)	// errno
	JAL	runtime·exitsyscall(SB)
	RET
ok:
	MOVW	R0, 24(FP)	// errno
	JAL	runtime·exitsyscall(SB)
	RET
