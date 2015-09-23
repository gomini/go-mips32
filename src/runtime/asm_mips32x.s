
// +build mips32 mips32le

#include "zasm_GOOS_GOARCH.h"
#include "funcdata.h"
#include "textflag.h"

TEXT runtime·rt0_go(SB),NOSPLIT,$0
	ADDU	$-12, R29
	MOVW	R1, 4(R29)
	MOVW	R2, 8(R29)

	MOVW	$runtime·g0(SB), g
	MOVW	$runtime·m0(SB), R3
	MOVW	g, m_g0(R3)
	MOVW	R3, g_m(g)

	JAL	runtime·reginit(SB)

	MOVW	$-8192(R29), R4
	MOVW	R4, g_stackguard0(g)
	MOVW	R4, g_stackguard1(g)
	MOVW	R4, (g_stack+stack_lo)(g)
	MOVW	R29, (g_stack+stack_hi)(g)

	JAL	runtime·emptyfunc(SB)

nocgo:
	MOVW	(g_stack+stack_lo)(g), R1
	ADDU	$const_StackGuard, R1
	MOVW	R1, g_stackguard0(g)
	MOVW	R1, g_stackguard1(g)

	JAL	runtime·check(SB)
	JAL	runtime·args(SB)
	JAL	runtime·osinit(SB)
	JAL	runtime·schedinit(SB)

	ADDU	$-12, R29
	MOVW	$runtime·main·f(SB), R1
	MOVW	R0, 0(R29)
	MOVW	R0, 4(R29)
	MOVW	R1, 8(R29)
	JAL	runtime·newproc(SB)
	ADDU	$12, R29

	JAL	runtime·mstart(SB)
	MOVW	R0, (R0)

DATA	runtime·main·f+0(SB)/4,$runtime·main(SB)
GLOBL	runtime·main·f(SB),RODATA,$4

TEXT runtime·breakpoint(SB),NOSPLIT,$-4-0
	MOVW	R0, 2(R0)
	RET

TEXT runtime·asminit(SB),NOSPLIT,$-4-0
	RET

DATA	runtime·_mips32_sfloat(SB)/4,$1
GLOBL	runtime·_mips32_sfloat(SB),NOPTR,$4

TEXT runtime·reginit(SB),NOSPLIT,$0-0
	MOVW	runtime·_mips32_sfloat(SB), R8
	BNE	R8, R0, soft
	MOVD	$0.5, F26
	SUBD	F26, F26, F24
	ADDD	F26, F26, F28
	ADDD	F28, F28, F30
	RET
soft:
	JAL	runtime·initm0(SB)
	RET

TEXT runtime·emptyfunc(SB),0,$0-0
	RET


TEXT runtime·gosave(SB),NOSPLIT,$-4-4
	MOVW	0(FP), R1
	MOVW	R29, gobuf_sp(R1)
	MOVW	R31, gobuf_pc(R1)
	MOVW	g, gobuf_g(R1)
	MOVW	R0, gobuf_lr(R1)
	MOVW	R0, gobuf_ret(R1)
	MOVW	R0, gobuf_ctxt(R1)
	RET

TEXT runtime·gogo(SB),NOSPLIT,$-4-4
	MOVW	0(FP), R2
	MOVW	gobuf_g(R2), g
	MOVW	0(g), R1
	MOVW	gobuf_sp(R2), R29
	MOVW	gobuf_lr(R2), R31
	MOVW	gobuf_ret(R2), R1
	MOVW	gobuf_ctxt(R2), R7
	MOVW	gobuf_pc(R2), R3
	MOVW	R0, gobuf_sp(R2)
	MOVW	R0, gobuf_ret(R2)
	MOVW	R0, gobuf_lr(R2)
	MOVW	R0, gobuf_ctxt(R2)
	JMP	(R3)
	SLL	R0, R0, R0
	SLL	R0, R0, R0

TEXT runtime·mcall(SB),NOSPLIT,$-4-4
	MOVW	R29, (g_sched+gobuf_sp)(g)
	MOVW	R31, (g_sched+gobuf_pc)(g)
	MOVW	R0, (g_sched+gobuf_lr)(g)
	MOVW	g, (g_sched+gobuf_g)(g)

	MOVW	g, R2
	MOVW	g_m(g), R3
	MOVW	m_g0(R3), g
	BNE	g, R2, 2(PC)
	JMP	runtime·badmcall(SB)
	MOVW	0(FP), R7
	MOVW	0(R7), R1
	MOVW	(g_sched+gobuf_sp)(g), R29
	ADDU	$-8, R29
	MOVW	R0, 0(R29)
	MOVW	R2, 4(R29)
	JAL	(R1)
	JMP	runtime·badmcall(SB)

TEXT runtime·switchtoM(SB),NOSPLIT,$0-0
	JAL	(R0)
	RET

TEXT runtime·onM_signalok(SB),NOSPLIT,$4-4
	MOVW	g_m(g), R2
	MOVW	m_gsignal(R2), R1
	MOVW	0(FP), R7
	BEQ	g, R1, ongsignal
	MOVW	R7, 4(R29)
	JAL	runtime·onM(SB)
	RET
ongsignal:
	MOVW	0(R7), R1
	JAL	(R1)
	RET

TEXT runtime·onM(SB),NOSPLIT,$0-4
	MOVW	0(FP), R7
	MOVW	g_m(g), R1

	MOVW	m_g0(R1), R2
	BEQ	g, R2, onm

	MOVW	m_curg(R1), R3
	BEQ	g, R3, oncurg

	MOVW	$runtime·badonm(SB), R1
	JAL	(R1)

oncurg:
	MOVW	$runtime·switchtoM(SB), R1
	ADDU	$8, R1
	MOVW	R1, (g_sched+gobuf_pc)(g)
	MOVW	R29, (g_sched+gobuf_sp)(g)
	MOVW	R31, (g_sched+gobuf_lr)(g)
	MOVW	g, (g_sched+gobuf_g)(g)

	MOVW	R2, g
	MOVW	(g_sched+gobuf_sp)(g), R1
	ADDU	$-4, R1
	MOVW	$runtime·mstart(SB), R4
	MOVW	R4, 0(R1)
	MOVW	R1, R29

	MOVW	0(R7), R1
	JAL	(R1)

	MOVW	g_m(g), R1
	MOVW	m_curg(R1), g
	MOVW	(g_sched+gobuf_sp)(g), R29
	MOVW	R0, (g_sched+gobuf_sp)(g)
	RET

onm:
	MOVW	0(R7), R1
	JAL	(R1)
	RET



TEXT runtime·morestack(SB),NOSPLIT,$-4-0
	MOVW	g_m(g), R8
	MOVW	m_g0(R8), R1
	BNE	g, R1, 2(PC)
	JAL	runtime·abort(SB)
	MOVW	m_gsignal(R8), R1
	BNE	g, R1, 2(PC)
	JAL	runtime·abort(SB)

	MOVW	R7, (g_sched+gobuf_ctxt)(g)
	MOVW	R29, (g_sched+gobuf_sp)(g)
	MOVW	R5, (g_sched+gobuf_pc)(g)
	MOVW	R4, (g_sched+gobuf_lr)(g)
	MOVW	R4, (m_morebuf+gobuf_pc)(R8)
	MOVW	R29, (m_morebuf+gobuf_sp)(R8)
	MOVW	g, (m_morebuf+gobuf_g)(R8)

	MOVW	m_g0(R8), g
	MOVW	(g_sched+gobuf_sp)(g), R29
	JAL	runtime·newstack(SB)

	UNDEF

TEXT runtime·morestack_noctxt(SB),NOSPLIT,$-4-0
	MOVW	R0, R7
	JMP	runtime·morestack(SB)


#define DISPATCH(NAME,MAXSIZE)		\
	SGTU	$MAXSIZE, R2, R3;	\
	BEQ	R3, R0, 3(PC);		\
	MOVW	$NAME(SB), R2;		\
	JMP	(R2)

TEXT ·reflectcall(SB),NOSPLIT,$-4-16
	MOVW	argsize+8(FP), R2
	DISPATCH(runtime·call16, 16)
	DISPATCH(runtime·call32, 32)
	DISPATCH(runtime·call64, 64)
	DISPATCH(runtime·call128, 128)
	DISPATCH(runtime·call256, 256)
	DISPATCH(runtime·call512, 512)
	DISPATCH(runtime·call1024, 1024)
	DISPATCH(runtime·call2048, 2048)
	DISPATCH(runtime·call4096, 4096)
	DISPATCH(runtime·call8192, 8192)
	DISPATCH(runtime·call16384, 16384)
	DISPATCH(runtime·call32768, 32768)
	DISPATCH(runtime·call65536, 65536)
	DISPATCH(runtime·call131072, 131072)
	DISPATCH(runtime·call262144, 262144)
	DISPATCH(runtime·call524288, 524288)
	DISPATCH(runtime·call1048576, 1048576)
	DISPATCH(runtime·call2097152, 2097152)
	DISPATCH(runtime·call4194304, 4194304)
	DISPATCH(runtime·call8388608, 8388608)
	DISPATCH(runtime·call16777216, 16777216)
	DISPATCH(runtime·call33554432, 33554432)
	DISPATCH(runtime·call67108864, 67108864)
	DISPATCH(runtime·call134217728, 134217728)
	DISPATCH(runtime·call268435456, 268435456)
	DISPATCH(runtime·call536870912, 536870912)
	DISPATCH(runtime·call1073741824, 1073741824)
	MOVW	$runtime·badreflectcall(SB), R13
	JMP	(R3)

#define CALLFN(NAME,MAXSIZE)		\
TEXT NAME(SB),WRAPPER,$MAXSIZE-16;	\
	NO_LOCAL_POINTERS;		\
	\
	MOVW	arg+4(FP), R1;		\
	MOVW	n+8(FP), R3;		\
	ADDU	$4, R29, R2;		\
	ADDU	R1, R3;			\
	BEQ	R3, R1, 6(PC);		\
	MOVB	(R1), R4;		\
	MOVB	R4, (R2);		\
	ADDU	$1, R1;			\
	ADDU	$1, R2;			\
	JMP	-5(PC);			\
		\
	MOVW	f+0(FP), R7;		\
	MOVW	(R7), R1;		\
	PCDATA	$PCDATA_StackMapIndex, $0;	\
	JAL	(R1);			\
	\
	MOVW	retoffset+12(FP), R4;	\
	ADDU	$4, R29, R1;		\
	MOVW	n+8(FP), R3;		\
	ADDU	R1, R3;			\
	ADDU	R4, R1;			\
	MOVW	arg+4(FP), R2;		\
	ADDU	R4, R2;			\
	BEQ	R1, R3, 6(PC);		\
	MOVBU	0(R1), R4;		\
	MOVB	R4, 0(R2);		\
	ADDU	$1, R1;			\
	ADDU	$1, R2;			\
	JMP	-5(PC);			\
	RET

CALLFN(·call16, 16)
CALLFN(·call32, 32)
CALLFN(·call64, 64)
CALLFN(·call128, 128)
CALLFN(·call256, 256)
CALLFN(·call512, 512)
CALLFN(·call1024, 1024)
CALLFN(·call2048, 2048)
CALLFN(·call4096, 4096)
CALLFN(·call8192, 8192)
CALLFN(·call16384, 16384)
CALLFN(·call32768, 32768)
CALLFN(·call65536, 65536)
CALLFN(·call131072, 131072)
CALLFN(·call262144, 262144)
CALLFN(·call524288, 524288)
CALLFN(·call1048576, 1048576)
CALLFN(·call2097152, 2097152)
CALLFN(·call4194304, 4194304)
CALLFN(·call8388608, 8388608)
CALLFN(·call16777216, 16777216)
CALLFN(·call33554432, 33554432)
CALLFN(·call67108864, 67108864)
CALLFN(·call134217728, 134217728)
CALLFN(·call268435456, 268435456)
CALLFN(·call536870912, 536870912)
CALLFN(·call1073741824, 1073741824)

TEXT runtime·jmpdefer(SB),NOSPLIT,$-4-8
	MOVW	0(R29), R31
	MOVW	0(FP), R7
	ADDU	$-8, R31
	MOVW	4(FP), R29
	MOVW	0(R7), R1
	ADDU	$-4, R29
	JMP	(R1)

TEXT gosave<>(SB),NOSPLIT,$-4
	MOVW	R31, (g_sched+gobuf_pc)(g)
	MOVW	R29, (g_sched+gobuf_sp)(g)
	MOVW	R0, (g_sched+gobuf_lr)(g)
	MOVW	R0, (g_sched+gobuf_ret)(g)
	MOVW	R0, (g_sched+gobuf_ctxt)(g)
	RET

TEXT ·asmcgocall(SB),NOSPLIT,$0-8
	MOVW	fn+0(FP), R1
	MOVW	arg+4(FP), R2
	JAL	asmcgocall<>(SB)
	RET

TEXT ·asmcgocall_errno(SB),NOSPLIT,$0-12
	MOVW	fn+0(FP), R1
	MOVW	arg+4(FP), R2
	JAL	asmcgocall<>(SB)
	MOVW	R1, ret+8(FP)
	RET

TEXT asmcgocall<>(SB),NOSPLIT,$0-0
	MOVW	R0, 21(R0)

TEXT runtime·cgocallback(SB),NOSPLIT,$12-12
	MOVW	R0, 22(R0)

TEXT ·cgocallback_gofunc(SB),NOSPLIT,$8-12
	MOVW	R0, 23(R0)

TEXT runtime·setg(SB),NOSPLIT,$-4-4
	MOVW	0(FP), R1
	JMP	setg<>(SB)

TEXT setg<>(SB),NOSPLIT,$-4-0
	MOVW	R0, 24(R0)

TEXT setg_gcc<>(SB),NOSPLIT,$0
	MOVW	R0, 25(R0)

TEXT runtime·getcallerpc(SB),NOSPLIT,$-4-8
	MOVW	0(R29), R1
	MOVW	R1, ret+4(FP)
	RET

TEXT runtime·gogetcallerpc(SB),NOSPLIT,$-4-8
	MOVW	0(R29), R1
	MOVW	R1, ret+4(FP)
	RET

TEXT runtime·setcallerpc(SB),NOSPLIT,$-4-8
	MOVW	pc+4(FP), R1
	MOVW	R1, 0(R29)
	RET

TEXT runtime·getcallersp(SB),NOSPLIT,$-4-8
	MOVW	argp+0(FP), R1
	ADDU	$-4, R1
	MOVW	R1, ret+4(FP)
	RET

TEXT runtime·gogetcallersp(SB),NOSPLIT,$-4-8
	MOVW	0(FP), R1
	ADDU	$-4, R1
	MOVW	R1, ret+4(FP)
	RET

TEXT runtime·abort(SB),NOSPLIT,$-4-0
	MOVW	(R0), R0
	UNDEF

TEXT runtime·cas(SB),NOSPLIT,$0-13
	MOVW	val+0(FP), R2
	MOVW	old+4(FP), R3
	MOVW	new+8(FP), R4
	MOVW	R0, R1
retry:
	LL	0(R2), R5
	BNE	R5, R3, stop
	MOVW	R4, R1
	SC	R1, 0(R2)
	BEQ	R1, R0, retry
stop:
	MOVB	R1, ret+12(FP)
	RET

TEXT runtime·casp(SB),NOSPLIT,$0-13
	JMP	runtime·cas(SB)

TEXT runtime·casuintptr(SB),NOSPLIT,$0-13
	JMP	runtime·cas(SB)

TEXT runtime·atomicloaduintptr(SB),NOSPLIT,$0-8
	JMP	runtime·atomicload(SB)

TEXT runtime·atomicloaduint(SB),NOSPLIT,$0-8
	JMP	runtime·atomicload(SB)

TEXT runtime·atomicstoreuintptr(SB),NOSPLIT,$0-8
	JMP	runtime·atomicload(SB)

TEXT runtime·aeshash(SB),NOSPLIT,$-4-0
	MOVW	(R0), R1
TEXT runtime·aeshash32(SB),NOSPLIT,$-4-0
	MOVW	(R0), R1
TEXT runtime·aeshash64(SB),NOSPLIT,$-4-0
	MOVW	(R0), R1
TEXT runtime·aeshashstr(SB),NOSPLIT,$-4-0
	MOVW	(R0), R1

TEXT memeq<>(SB),NOSPLIT,$-4-0
	ADDU	R2, R1
loop:
	BNE	R2, R1, test
	MOVW	$1, R5
	MOVB	R5, 0(R4)
	RET
test:
	MOVBU	0(R2), R5
	MOVBU	0(R3), R6
	ADDU	$1, R2
	ADDU	$1, R3
	BEQ	R5, R6, loop
	MOVB	R0, 0(R4)
	RET

TEXT runtime·memeq(SB),NOSPLIT,$-4-13
	MOVW	size+8(FP), R1
	MOVW	a+0(FP), R2
	MOVW	b+4(FP), R3
	MOVW	$ret+12(FP), R4
	JMP	memeq<>(SB)

// see runtime_test.go:eqstring_generic for equivalent Go code
TEXT runtime·eqstring(SB),NOSPLIT,$-4-17
	MOVW	slen+4(FP), R1
	MOVW	tlen+12(FP), R2
	BNE	R1, R2, noteq
	MOVW	sptr+0(FP), R2
	MOVW	tptr+8(FP), R3
	MOVW	$ret+16(FP), R4
	JMP	memeq<>(SB)
noteq:
	MOVB	R0, ret+16(FP)
	RET

TEXT bytes·Equal(SB),NOSPLIT,$0
	MOVW	slen+4(FP), R1
	MOVW	tlen+16(FP), R2
	BNE	R1, R2, noteq
	MOVW	sptr+0(FP), R2
	MOVW	tptr+12(FP), R3
	MOVW	$ret+24(FP), R4
	JMP	memeq<>(SB)
noteq:
	MOVB	R0, ret+24(FP)
	RET

TEXT indexbyte<>(SB),NOSPLIT,$-4
	MOVW	R2, R5
	ADDU	R2, R1
loop:
	BEQ	R1, R2, notfound
	MOVBU	0(R2), R6
	ADDU	$1, R2
	BNE	R6, R3, loop

	ADDU	$-1, R2
	SUBU	R5, R2
	MOVW	R2, 0(R4)
	RET

notfound:
	ADDU	$-1, R0, R1
	MOVW	R1, 0(R4)
	RET

TEXT bytes·IndexByte(SB),NOSPLIT,$0
	MOVW	len+4(FP), R1
	MOVW	ptr+0(FP), R2
	MOVBU	c+12(FP), R3
	MOVW	$ret+16(FP), R4
	JMP	indexbyte<>(SB)

TEXT strings·IndexByte(SB),NOSPLIT,$0
	MOVW	len+4(FP), R1
	MOVW	ptr+0(FP), R2
	MOVBU	c+8(FP), R3
	MOVW	$ret+12(FP), R4
	JMP	indexbyte<>(SB)

TEXT runtime·duffzero(SB),NOSPLIT,$0-0
	MOVW	R0, 0(R1)
	MOVW	R0, 4(R1)
	MOVW	R0, 8(R1)
	MOVW	R0, 12(R1)
	MOVW	R0, 16(R1)
	MOVW	R0, 20(R1)
	MOVW	R0, 24(R1)
	MOVW	R0, 28(R1)
	MOVW	R0, 32(R1)
	MOVW	R0, 36(R1)
	MOVW	R0, 40(R1)
	MOVW	R0, 44(R1)
	MOVW	R0, 48(R1)
	MOVW	R0, 52(R1)
	MOVW	R0, 56(R1)
	MOVW	R0, 60(R1)
	MOVW	R0, 64(R1)
	MOVW	R0, 68(R1)
	MOVW	R0, 72(R1)
	MOVW	R0, 76(R1)
	MOVW	R0, 80(R1)
	MOVW	R0, 84(R1)
	MOVW	R0, 88(R1)
	MOVW	R0, 92(R1)
	MOVW	R0, 96(R1)
	MOVW	R0, 100(R1)
	MOVW	R0, 104(R1)
	MOVW	R0, 108(R1)
	MOVW	R0, 112(R1)
	MOVW	R0, 116(R1)
	MOVW	R0, 120(R1)
	MOVW	R0, 124(R1)
	MOVW	R0, 128(R1)
	MOVW	R0, 132(R1)
	MOVW	R0, 136(R1)
	MOVW	R0, 140(R1)
	MOVW	R0, 144(R1)
	MOVW	R0, 148(R1)
	MOVW	R0, 152(R1)
	MOVW	R0, 156(R1)
	MOVW	R0, 160(R1)
	MOVW	R0, 164(R1)
	MOVW	R0, 168(R1)
	MOVW	R0, 172(R1)
	MOVW	R0, 176(R1)
	MOVW	R0, 180(R1)
	MOVW	R0, 184(R1)
	MOVW	R0, 188(R1)
	MOVW	R0, 192(R1)
	MOVW	R0, 196(R1)
	MOVW	R0, 200(R1)
	MOVW	R0, 204(R1)
	MOVW	R0, 208(R1)
	MOVW	R0, 212(R1)
	MOVW	R0, 216(R1)
	MOVW	R0, 220(R1)
	MOVW	R0, 224(R1)
	MOVW	R0, 228(R1)
	MOVW	R0, 232(R1)
	MOVW	R0, 236(R1)
	MOVW	R0, 240(R1)
	MOVW	R0, 244(R1)
	MOVW	R0, 248(R1)
	MOVW	R0, 252(R1)
	MOVW	R0, 256(R1)
	MOVW	R0, 260(R1)
	MOVW	R0, 264(R1)
	MOVW	R0, 268(R1)
	MOVW	R0, 272(R1)
	MOVW	R0, 276(R1)
	MOVW	R0, 280(R1)
	MOVW	R0, 284(R1)
	MOVW	R0, 288(R1)
	MOVW	R0, 292(R1)
	MOVW	R0, 296(R1)
	MOVW	R0, 300(R1)
	MOVW	R0, 304(R1)
	MOVW	R0, 308(R1)
	MOVW	R0, 312(R1)
	MOVW	R0, 316(R1)
	MOVW	R0, 320(R1)
	MOVW	R0, 324(R1)
	MOVW	R0, 328(R1)
	MOVW	R0, 332(R1)
	MOVW	R0, 336(R1)
	MOVW	R0, 340(R1)
	MOVW	R0, 344(R1)
	MOVW	R0, 348(R1)
	MOVW	R0, 352(R1)
	MOVW	R0, 356(R1)
	MOVW	R0, 360(R1)
	MOVW	R0, 364(R1)
	MOVW	R0, 368(R1)
	MOVW	R0, 372(R1)
	MOVW	R0, 376(R1)
	MOVW	R0, 380(R1)
	MOVW	R0, 384(R1)
	MOVW	R0, 388(R1)
	MOVW	R0, 392(R1)
	MOVW	R0, 396(R1)
	MOVW	R0, 400(R1)
	MOVW	R0, 404(R1)
	MOVW	R0, 408(R1)
	MOVW	R0, 412(R1)
	MOVW	R0, 416(R1)
	MOVW	R0, 420(R1)
	MOVW	R0, 424(R1)
	MOVW	R0, 428(R1)
	MOVW	R0, 432(R1)
	MOVW	R0, 436(R1)
	MOVW	R0, 440(R1)
	MOVW	R0, 444(R1)
	MOVW	R0, 448(R1)
	MOVW	R0, 452(R1)
	MOVW	R0, 456(R1)
	MOVW	R0, 460(R1)
	MOVW	R0, 464(R1)
	MOVW	R0, 468(R1)
	MOVW	R0, 472(R1)
	MOVW	R0, 476(R1)
	MOVW	R0, 480(R1)
	MOVW	R0, 484(R1)
	MOVW	R0, 488(R1)
	MOVW	R0, 492(R1)
	MOVW	R0, 496(R1)
	MOVW	R0, 500(R1)
	MOVW	R0, 504(R1)
	MOVW	R0, 508(R1)
	RET
	
TEXT runtime·duffcopy(SB),NOSPLIT,$0-0
	MOVW	0(R5), R1
	MOVW	4(R5), R2
	MOVW	8(R5), R3
	MOVW	12(R5), R4
	MOVW	R1, 0(R6)
	MOVW	R2, 4(R6)
	MOVW	R3, 8(R6)
	MOVW	R4, 12(R6)
	MOVW	16(R5), R1
	MOVW	20(R5), R2
	MOVW	24(R5), R3
	MOVW	28(R5), R4
	MOVW	R1, 16(R6)
	MOVW	R2, 20(R6)
	MOVW	R3, 24(R6)
	MOVW	R4, 28(R6)
	MOVW	32(R5), R1
	MOVW	36(R5), R2
	MOVW	40(R5), R3
	MOVW	44(R5), R4
	MOVW	R1, 32(R6)
	MOVW	R2, 36(R6)
	MOVW	R3, 40(R6)
	MOVW	R4, 44(R6)
	MOVW	48(R5), R1
	MOVW	52(R5), R2
	MOVW	56(R5), R3
	MOVW	60(R5), R4
	MOVW	R1, 48(R6)
	MOVW	R2, 52(R6)
	MOVW	R3, 56(R6)
	MOVW	R4, 60(R6)
	MOVW	64(R5), R1
	MOVW	68(R5), R2
	MOVW	72(R5), R3
	MOVW	76(R5), R4
	MOVW	R1, 64(R6)
	MOVW	R2, 68(R6)
	MOVW	R3, 72(R6)
	MOVW	R4, 76(R6)
	MOVW	80(R5), R1
	MOVW	84(R5), R2
	MOVW	88(R5), R3
	MOVW	92(R5), R4
	MOVW	R1, 80(R6)
	MOVW	R2, 84(R6)
	MOVW	R3, 88(R6)
	MOVW	R4, 92(R6)
	MOVW	96(R5), R1
	MOVW	100(R5), R2
	MOVW	104(R5), R3
	MOVW	108(R5), R4
	MOVW	R1, 96(R6)
	MOVW	R2, 100(R6)
	MOVW	R3, 104(R6)
	MOVW	R4, 108(R6)
	MOVW	112(R5), R1
	MOVW	116(R5), R2
	MOVW	120(R5), R3
	MOVW	124(R5), R4
	MOVW	R1, 112(R6)
	MOVW	R2, 116(R6)
	MOVW	R3, 120(R6)
	MOVW	R4, 124(R6)
	MOVW	128(R5), R1
	MOVW	132(R5), R2
	MOVW	136(R5), R3
	MOVW	140(R5), R4
	MOVW	R1, 128(R6)
	MOVW	R2, 132(R6)
	MOVW	R3, 136(R6)
	MOVW	R4, 140(R6)
	MOVW	144(R5), R1
	MOVW	148(R5), R2
	MOVW	152(R5), R3
	MOVW	156(R5), R4
	MOVW	R1, 144(R6)
	MOVW	R2, 148(R6)
	MOVW	R3, 152(R6)
	MOVW	R4, 156(R6)
	MOVW	160(R5), R1
	MOVW	164(R5), R2
	MOVW	168(R5), R3
	MOVW	172(R5), R4
	MOVW	R1, 160(R6)
	MOVW	R2, 164(R6)
	MOVW	R3, 168(R6)
	MOVW	R4, 172(R6)
	MOVW	176(R5), R1
	MOVW	180(R5), R2
	MOVW	184(R5), R3
	MOVW	188(R5), R4
	MOVW	R1, 176(R6)
	MOVW	R2, 180(R6)
	MOVW	R3, 184(R6)
	MOVW	R4, 188(R6)
	MOVW	192(R5), R1
	MOVW	196(R5), R2
	MOVW	200(R5), R3
	MOVW	204(R5), R4
	MOVW	R1, 192(R6)
	MOVW	R2, 196(R6)
	MOVW	R3, 200(R6)
	MOVW	R4, 204(R6)
	MOVW	208(R5), R1
	MOVW	212(R5), R2
	MOVW	216(R5), R3
	MOVW	220(R5), R4
	MOVW	R1, 208(R6)
	MOVW	R2, 212(R6)
	MOVW	R3, 216(R6)
	MOVW	R4, 220(R6)
	MOVW	224(R5), R1
	MOVW	228(R5), R2
	MOVW	232(R5), R3
	MOVW	236(R5), R4
	MOVW	R1, 224(R6)
	MOVW	R2, 228(R6)
	MOVW	R3, 232(R6)
	MOVW	R4, 236(R6)
	MOVW	240(R5), R1
	MOVW	244(R5), R2
	MOVW	248(R5), R3
	MOVW	252(R5), R4
	MOVW	R1, 240(R6)
	MOVW	R2, 244(R6)
	MOVW	R3, 248(R6)
	MOVW	R4, 252(R6)
	MOVW	256(R5), R1
	MOVW	260(R5), R2
	MOVW	264(R5), R3
	MOVW	268(R5), R4
	MOVW	R1, 256(R6)
	MOVW	R2, 260(R6)
	MOVW	R3, 264(R6)
	MOVW	R4, 268(R6)
	MOVW	272(R5), R1
	MOVW	276(R5), R2
	MOVW	280(R5), R3
	MOVW	284(R5), R4
	MOVW	R1, 272(R6)
	MOVW	R2, 276(R6)
	MOVW	R3, 280(R6)
	MOVW	R4, 284(R6)
	MOVW	288(R5), R1
	MOVW	292(R5), R2
	MOVW	296(R5), R3
	MOVW	300(R5), R4
	MOVW	R1, 288(R6)
	MOVW	R2, 292(R6)
	MOVW	R3, 296(R6)
	MOVW	R4, 300(R6)
	MOVW	304(R5), R1
	MOVW	308(R5), R2
	MOVW	312(R5), R3
	MOVW	316(R5), R4
	MOVW	R1, 304(R6)
	MOVW	R2, 308(R6)
	MOVW	R3, 312(R6)
	MOVW	R4, 316(R6)
	MOVW	320(R5), R1
	MOVW	324(R5), R2
	MOVW	328(R5), R3
	MOVW	332(R5), R4
	MOVW	R1, 320(R6)
	MOVW	R2, 324(R6)
	MOVW	R3, 328(R6)
	MOVW	R4, 332(R6)
	MOVW	336(R5), R1
	MOVW	340(R5), R2
	MOVW	344(R5), R3
	MOVW	348(R5), R4
	MOVW	R1, 336(R6)
	MOVW	R2, 340(R6)
	MOVW	R3, 344(R6)
	MOVW	R4, 348(R6)
	MOVW	352(R5), R1
	MOVW	356(R5), R2
	MOVW	360(R5), R3
	MOVW	364(R5), R4
	MOVW	R1, 352(R6)
	MOVW	R2, 356(R6)
	MOVW	R3, 360(R6)
	MOVW	R4, 364(R6)
	MOVW	368(R5), R1
	MOVW	372(R5), R2
	MOVW	376(R5), R3
	MOVW	380(R5), R4
	MOVW	R1, 368(R6)
	MOVW	R2, 372(R6)
	MOVW	R3, 376(R6)
	MOVW	R4, 380(R6)
	MOVW	384(R5), R1
	MOVW	388(R5), R2
	MOVW	392(R5), R3
	MOVW	396(R5), R4
	MOVW	R1, 384(R6)
	MOVW	R2, 388(R6)
	MOVW	R3, 392(R6)
	MOVW	R4, 396(R6)
	MOVW	400(R5), R1
	MOVW	404(R5), R2
	MOVW	408(R5), R3
	MOVW	412(R5), R4
	MOVW	R1, 400(R6)
	MOVW	R2, 404(R6)
	MOVW	R3, 408(R6)
	MOVW	R4, 412(R6)
	MOVW	416(R5), R1
	MOVW	420(R5), R2
	MOVW	424(R5), R3
	MOVW	428(R5), R4
	MOVW	R1, 416(R6)
	MOVW	R2, 420(R6)
	MOVW	R3, 424(R6)
	MOVW	R4, 428(R6)
	MOVW	432(R5), R1
	MOVW	436(R5), R2
	MOVW	440(R5), R3
	MOVW	444(R5), R4
	MOVW	R1, 432(R6)
	MOVW	R2, 436(R6)
	MOVW	R3, 440(R6)
	MOVW	R4, 444(R6)
	MOVW	448(R5), R1
	MOVW	452(R5), R2
	MOVW	456(R5), R3
	MOVW	460(R5), R4
	MOVW	R1, 448(R6)
	MOVW	R2, 452(R6)
	MOVW	R3, 456(R6)
	MOVW	R4, 460(R6)
	MOVW	464(R5), R1
	MOVW	468(R5), R2
	MOVW	472(R5), R3
	MOVW	476(R5), R4
	MOVW	R1, 464(R6)
	MOVW	R2, 468(R6)
	MOVW	R3, 472(R6)
	MOVW	R4, 476(R6)
	MOVW	480(R5), R1
	MOVW	484(R5), R2
	MOVW	488(R5), R3
	MOVW	492(R5), R4
	MOVW	R1, 480(R6)
	MOVW	R2, 484(R6)
	MOVW	R3, 488(R6)
	MOVW	R4, 492(R6)
	MOVW	496(R5), R1
	MOVW	500(R5), R2
	MOVW	504(R5), R3
	MOVW	508(R5), R4
	MOVW	R1, 496(R6)
	MOVW	R2, 500(R6)
	MOVW	R3, 504(R6)
	MOVW	R4, 508(R6)
	RET

TEXT runtime·fastrand1(SB),NOSPLIT,$-4-4
	MOVW	g_m(g), R1
	MOVW	m_fastrand(R1), R2
	ADDU	R2, R2
	SGT	R0, R2, R3
	BEQ	R3, R0, 2(PC)
	XOR	$0x88888eef, R2
	MOVW	R2, m_fastrand(R1)
	MOVW	R2, ret+0(FP)
	RET

TEXT runtime·return0(SB),NOSPLIT,$0
	MOVW	R0, R1
	RET

TEXT runtime·procyield(SB),NOSPLIT,$0
	MOVW	tick+0(FP), R1
	ADDU	$1, R1
	ADDU	$-1, R1
	BNE	R1, R0, -1(PC)
	RET

TEXT _cgo_topofstack(SB),NOSPLIT,$8
	MOVW	R0, 27(R0)

TEXT runtime·goexit(SB),NOSPLIT,$-4-0
	SLL	$0, R0, R0
	SLL	$0, R0, R0
	JAL	runtime·goexit1(SB)
