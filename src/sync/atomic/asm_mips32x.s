
// +build !race
// +build mips32 mips32le

#include "textflag.h"

TEXT ·SwapUint32(SB),NOSPLIT,$0-12
	MOVW	addr+0(FP), R2
	MOVW	new+4(FP), R3
retry:
	LL	0(R2), R1
	MOVW	R3, R4
	SC	R4, 0(R2)
	BEQ	R4, R0, retry
	MOVW	R1, ret+8(FP)
	RET

TEXT ·SwapInt32(SB),NOSPLIT,$0-12
	JMP	·SwapUint32(SB)

TEXT ·SwapUintptr(SB),NOSPLIT,$0-12
	JMP	·SwapUint32(SB)

TEXT ·SwapPointer(SB),NOSPLIT,$0-12
	JMP	·SwapUint32(SB)

TEXT cas<>(SB),NOSPLIT,$0-0
	MOVW	R0, R1
retry:
	LL	0(R2), R5
	BNE	R5, R3, stop
	MOVW	R4, R1
	SC	R1, 0(R2)
	BEQ	R1, R0, retry
stop:
	RET

TEXT ·CompareAndSwapUint32(SB),NOSPLIT,$0-13
	MOVW	addr+0(FP), R2
	MOVW	old+4(FP), R3
	MOVW	new+8(FP), R4
	JAL	cas<>(SB)
	MOVB	R1, ret+12(FP)
	RET

TEXT ·CompareAndSwapInt32(SB),NOSPLIT,$0-13
	JMP	·CompareAndSwapUint32(SB)

TEXT ·CompareAndSwapUintptr(SB),NOSPLIT,$0-13
	JMP	·CompareAndSwapUint32(SB)

TEXT ·CompareAndSwapPointer(SB),NOSPLIT,$0-13
	JMP	·CompareAndSwapUint32(SB)

TEXT ·AddInt32(SB),NOSPLIT,$0-12
	JMP	·AddUint32(SB)

TEXT ·AddUint32(SB),NOSPLIT,$0-12
	MOVW	addr+0(FP), R2
	MOVW	delta+4(FP), R3
retry:
	LL	0(R2), R1
	ADDU	R3, R1
	MOVW	R1, R4
	SC	R4, 0(R2)
	BEQ	R4, R0, retry
	MOVW	R1, ret+8(FP)
	RET

TEXT ·AddUintptr(SB),NOSPLIT,$0-12
	JMP	·AddUint32(SB)

TEXT ·LoadInt32(SB),NOSPLIT,$0-8
	JMP	·LoadUint32(SB)

TEXT ·LoadUint32(SB),NOSPLIT,$0-8
	MOVW	addr+0(FP), R2
	MOVW	0(R2), R1
	MOVW	R1, val+4(FP)
	RET

TEXT ·LoadUintptr(SB),NOSPLIT,$0-8
	JMP	·LoadUint32(SB)

TEXT ·LoadPointer(SB),NOSPLIT,$0-8
	JMP	·LoadUint32(SB)

TEXT ·StoreInt32(SB),NOSPLIT,$0-8
	JMP	·StoreUint32(SB)

TEXT ·StoreUint32(SB),NOSPLIT,$0-8
	MOVW	addr+0(FP), R2
	MOVW	val+4(FP), R1
	MOVW	R1, 0(R2)
	RET

TEXT ·StoreUintptr(SB),NOSPLIT,$0-8
	JMP	·StoreUint32(SB)

TEXT ·StorePointer(SB),NOSPLIT,$0-8
	JMP	·StoreUint32(SB)

TEXT ·SwapInt64(SB),NOSPLIT,$0-20
	JMP	·SwapUint64(SB)

TEXT ·CompareAndSwapInt64(SB),NOSPLIT,$0-21
	JMP	·CompareAndSwapUint64(SB)

TEXT ·AddInt64(SB),NOSPLIT,$0-20
	JMP	·AddUint64(SB)

TEXT ·LoadInt64(SB),NOSPLIT,$0-12
	JMP	·LoadUint64(SB)

TEXT ·StoreInt64(SB),NOSPLIT,$0-12
	JMP	·StoreUint64(SB)



TEXT ·SwapUint64(SB),NOSPLIT,$0-20
	MOVW	addr+0(FP), R1
	AND	$7, R1, R2
	BEQ	R2, 2(PC)
	MOVW	R0, (R0)
	MOVW	(R1), R1
	JMP	·swapUint64(SB)

TEXT ·CompareAndSwapUint64(SB),NOSPLIT,$0-21
	MOVW	addr+0(FP), R1
	AND	$7, R1, R2
	BEQ	R2, 2(PC)
	MOVW	R0, (R0)
	MOVW	(R1), R1
	JMP	·compareAndSwapUint64(SB)

TEXT ·AddUint64(SB),NOSPLIT,$0-20
	MOVW	addr+0(FP), R1
	AND	$7, R1, R2
	BEQ	R2, 2(PC)
	MOVW	R0, (R0)
	MOVW	(R1), R1
	JMP	·addUint64(SB)

TEXT ·LoadUint64(SB),NOSPLIT,$0-12
	MOVW	addr+0(FP), R1
	AND	$7, R1, R2
	BEQ	R2, 2(PC)
	MOVW	R0, (R0)
	MOVW	(R1), R1
	JMP	·loadUint64(SB)

TEXT ·StoreUint64(SB),NOSPLIT,$0-12
	MOVW	addr+0(FP), R1
	AND	$7, R1, R2
	BEQ	R2, 2(PC)
	MOVW	R0, (R0)
	MOVW	(R1), R1
	JMP	·storeUint64(SB)
