
// +build linux
// +build mips32 mips32le


#include "zasm_GOOS_GOARCH.h"
#include "textflag.h"

#define SYS_BASE 4000

#define SYS_exit (SYS_BASE + 1)
#define SYS_read (SYS_BASE + 3)
#define SYS_write (SYS_BASE + 4)
#define SYS_open (SYS_BASE + 5)
#define SYS_close (SYS_BASE + 6)
#define SYS_gettimeofday (SYS_BASE + 78)
#define SYS_clone (SYS_BASE + 120)
#define SYS_rt_sigreturn (SYS_BASE + 193)
#define SYS_rt_sigaction (SYS_BASE + 194)
#define SYS_rt_sigprocmask (SYS_BASE + 195)
#define SYS_sigaltstack (SYS_BASE + 206)
#define SYS_mmap2 (SYS_BASE + 210)
#define SYS_futex (SYS_BASE + 238)
#define SYS_exit_group (SYS_BASE + 246)
#define SYS_munmap (SYS_BASE + 91)
#define SYS_madvise (SYS_BASE + 218)
#define SYS_setitimer (SYS_BASE + 104)
#define SYS_mincore (SYS_BASE + 217)
#define SYS_gettid (SYS_BASE + 222)
#define SYS_tkill (SYS_BASE + 236)
#define SYS_sched_yield (SYS_BASE + 162)
#define SYS_select (SYS_BASE + 142) // newselect
#define SYS_getrlimit	(SYS_BASE+76)
#define SYS_sched_getaffinity (SYS_BASE + 240)
#define SYS_clock_gettime (SYS_BASE + 263)
#define SYS_epoll_create (SYS_BASE + 248)
#define SYS_epoll_ctl (SYS_BASE + 249)
#define SYS_epoll_wait (SYS_BASE + 250)
#define SYS_epoll_create1 (SYS_BASE + 326)
#define SYS_fcntl (SYS_BASE + 55)


TEXT scall<>(SB),NOSPLIT,$-4
	MOVW	R29, R17
	ADDU	$-32, R29
	AND	$-8, R29

	MOVW	R8, 16(R29)
	MOVW	R9, 20(R29)
	MOVW	R10, 24(R29)
	MOVW	R2, 28(R29)

	NOSCHED
	MOVW	28(R29), R2
	SYSCALL
	SCHED

	MOVW	R17, R29
	BEQ	R16, R0, end

	MOVW	R2, 0(R16)
end:
	RET

TEXT runtime·open(SB),NOSPLIT,$0
	MOVW	name+0(FP), R4
	MOVW	mode+4(FP), R5
	MOVW	perm+8(FP), R6
	MOVW	$ret+12(FP), R16
	MOVW	$SYS_open, R2
	JMP	scall<>(SB)

TEXT runtime·close(SB),NOSPLIT,$0
	MOVW	fd+0(FP), R4
	MOVW	$SYS_close, R2
	MOVW	$ret+4(FP), R16
	JMP	scall<>(SB)

TEXT runtime·write(SB),NOSPLIT,$0
	MOVW	fd+0(FP), R4
	MOVW	p+4(FP), R5
	MOVW	n+8(FP), R6
	MOVW	$SYS_write, R2
	MOVW	$ret+12(FP), R16
	JMP	scall<>(SB)

TEXT runtime·read(SB),NOSPLIT,$0
	MOVW	fd+0(FP), R4
	MOVW	p+4(FP), R5
	MOVW	n+8(FP), R6
	MOVW	$ret+12(FP), R16
	MOVW	$SYS_read, R2
	JMP	scall<>(SB)

TEXT runtime·getrlimit(SB),NOSPLIT,$0
	MOVW	kind+0(FP), R4
	MOVW	limit+4(FP), R5
	MOVW	$ret+8(FP), R16
	MOVW	$SYS_getrlimit, R2
	JMP	scall<>(SB)

TEXT runtime·exit(SB),NOSPLIT,$-4
	MOVW	code+0(FP), R4
	MOVW	$SYS_exit_group, R2
	MOVW	$1000, R16
	JMP	scall<>(SB)

TEXT runtime·exit1(SB),NOSPLIT,$-4
	MOVW	code+0(FP), R4
	MOVW	$SYS_exit, R2
	MOVW	$1001, R16
	JMP	scall<>(SB)

TEXT runtime·raise(SB),NOSPLIT,$-4
	MOVW	$SYS_gettid, R2
	MOVW	R0, R16
	JAL	scall<>(SB)
	MOVW	R2, R4
	MOVW	sig+0(FP), R5
	MOVW	$SYS_tkill, R2
	JMP	scall<>(SB)

TEXT runtime·mmap(SB),NOSPLIT,$0
	MOVW	addr+0(FP), R4
	MOVW	n+4(FP), R5
	MOVW	protect+8(FP), R6
	MOVW	flags+12(FP), R7
	MOVW	fd+16(FP), R8
	MOVW	off+20(FP), R9
	SRL	$12, R9
	MOVW	$SYS_mmap2, R2
	MOVW	$ret+24(FP), R16
	JMP	scall<>(SB)

TEXT runtime·munmap(SB),NOSPLIT,$0
	MOVW	addr+0(FP), R4
	MOVW	n+4(FP), R5
	MOVW	$SYS_munmap, R2
	MOVW	R0, R16
	JAL	scall<>(SB)
	BEQ	R7, R0, ok
	MOVW	$1001, R1
	MOVW	R1, 0(R0)
ok:
	RET

TEXT runtime·madvise(SB),NOSPLIT,$0
	MOVW	addr+0(FP), R4
	MOVW	n+4(FP), R5
	MOVW	flags+8(FP), R6
	MOVW	$SYS_madvise, R2
	MOVW	R0, R16
	JMP	scall<>(SB)

TEXT runtime·setitimer(SB),NOSPLIT,$0
	MOVW	which+0(FP), R4
	MOVW	new+4(FP), R5
	MOVW	old+8(FP), R6
	MOVW	$SYS_setitimer, R2
	MOVW	R0, R16
	JMP	scall<>(SB)

TEXT runtime·mincore(SB),NOSPLIT,$0
	MOVW	addr+0(FP), R4
	MOVW	n+4(FP), R5
	MOVW	dst+8(FP), R6
	MOVW	$SYS_mincore, R2
	MOVW	R0, R16
	JAL	scall<>(SB)
	MOVW	R0, ret+12(FP)
	RET

TEXT time·now(SB),NOSPLIT,$32
	MOVW	R0, R4
	MOVW	$8(R29), R5
	MOVW	$SYS_clock_gettime, R2
	MOVW	R0, R16
	JAL	scall<>(SB)
	MOVW	8(R29), R1
	MOVW	12(R29), R2


#ifdef GOARCH_mips32le
	MOVW	R1, 0(FP)
	MOVW	R0, 4(FP)
#else
	MOVW	R1, 4(FP)
	MOVW	R0, 0(FP)
#endif
	MOVW	R2, nsec+8(FP)
	RET

TEXT runtime·nanotime(SB),NOSPLIT,$32
	MOVW	$1, R4
	MOVW	$8(R29), R5
	MOVW	$SYS_clock_gettime, R2
	MOVW	R0, R16
	JAL	scall<>(SB)

	MOVW	8(R29), R1
	MOVW	12(R29), R2

	MOVW	$1000000000, R3
	MULU	R3, R1

	MOVW	LO, R1
	MOVW	HI, R4

	ADDU	R2, R1, R3
	SGTU	R1, R3, R2
	ADDU	R2, R4


#ifdef GOARCH_mips32le
	MOVW	R3, 0(FP)
	MOVW	R4, 4(FP)
#else
	MOVW	R3, 4(FP)
	MOVW	R4, 0(FP)
#endif
	RET

TEXT runtime·futex(SB),NOSPLIT,$0
	MOVW	uaddr+0(FP), R4
	MOVW	op+4(FP), R5
	MOVW	val+8(FP), R6
	MOVW	timeout+12(FP), R7
	MOVW	uaddr2+16(FP), R8
	MOVW	val2+20(FP), R9
	MOVW	$SYS_futex, R2
	MOVW	$ret+24(FP), R16
	JMP	scall<>(SB)

TEXT runtime·clone(SB),NOSPLIT,$0
	MOVW	flags+0(FP), R4
	MOVW	stack+4(FP), R5
	MOVW	R0, R6
	MOVW	R0, R7
	MOVW	R0, R8
	MOVW	R0, R9
	MOVW	R0, R10

	ADDU	$-16, R5

	MOVW	mp+8(FP), R11
	MOVW	gp+12(FP), R12
	MOVW	fn+16(FP), R13
	MOVW	$0xabba, R14
	MOVW	R11, 0(R5)
	MOVW	R12, 4(R5)
	MOVW	R13, 8(R5)
	MOVW	R14, 12(R5)

	MOVW	R29, R17
	ADDU	$-32, R29
	AND	$-8, R29

	NOSCHED
	MOVW	$SYS_clone, R2
	SYSCALL
	SCHED

	BEQ	R2, R0, child

	MOVW	R17, R29
	MOVW	R2, ret+20(FP)
	RET

child:
	MOVW	12(R29), R1
	MOVW	$0xabba, R2
	BEQ	R1, R2, 2(PC)
	JAL	runtime·abort(SB)

	MOVW	4(R29), g
	MOVW	0(R29), R1
	MOVW	R1, g_m(g)

	MOVW	0(R1), R2
	MOVW	0(g), R2

	JAL	runtime·emptyfunc(SB)

	MOVW	$SYS_gettid, R2
	MOVW	R0, R16
	JAL	scall<>(SB)
	MOVW	g_m(g), R1
	MOVW	R2, m_procid(R1)

	MOVW	8(R29), R1
	ADDU	$16, R29
	JAL	(R1)

	MOVW	R0, 4(R29)
	JAL	runtime·exit(SB)

	UNDEF

TEXT runtime·sigaltstack(SB),NOSPLIT,$0
	MOVW	0(FP), R4
	MOVW	4(FP), R5
	MOVW	$SYS_sigaltstack, R2
	MOVW	R0, R16
	JAL	scall<>(SB)
	BEQ	R7, R0, ok
	MOVW	$1002, R1
	MOVW	R0, 0(R1)
ok:
	RET

TEXT runtime·sigtramp(SB),NOSPLIT,$24
	MOVW	R4, 4(R29)

	BNE	g, R0, hasg
	MOVW	$runtime·badsignal(SB), R1
	JAL	(R1)
	RET
hasg:
	MOVW	g, R1
	MOVW	g, 20(R29)

	MOVW	g_m(g), R2
	MOVW	m_gsignal(R2), g

	MOVW	R5, 8(R29)
	MOVW	R6, 12(R29)
	MOVW	R1, 16(R29)
	JAL	runtime·sighandler(SB)
	MOVW	20(R29), g
	RET


TEXT runtime·rtsigprocmask(SB),NOSPLIT,$0
	MOVW	0(FP), R4
	MOVW	4(FP), R5
	MOVW	8(FP), R6
	MOVW	12(FP), R7
	MOVW	$SYS_rt_sigprocmask, R2
	MOVW	R0, R16
	JMP	scall<>(SB)

TEXT runtime·rt_sigaction(SB),NOSPLIT,$-4
	MOVW	0(FP), R4
	MOVW	4(FP), R5
	MOVW	8(FP), R6
	MOVW	12(FP), R7
	MOVW	$SYS_rt_sigaction, R2
	MOVW	$ret+16(FP), R16
	JMP	scall<>(SB)

TEXT runtime·usleep(SB),NOSPLIT,$12
	MOVW	usec+0(FP), R1
	MOVW	$1000000, R2
	DIVU	R2, R1
	MOVW	LO, R3
	MOVW	HI, R4
	MOVW	R3, 4(SP)
	MOVW	R4, 8(SP)
	MOVW	R0, R4
	MOVW	R0, R5
	MOVW	R0, R6
	MOVW	R0, R7
	MOVW	$4(SP), R8
	MOVW	$SYS_select, R2
	MOVW	R0, R16
	JAL	scall<>(SB)
	RET

TEXT runtime·osyield(SB),NOSPLIT,$0
	MOVW	$SYS_sched_yield, R2
	MOVW	R0, R16
	JMP	scall<>(SB)

TEXT runtime·sched_getaffinity(SB),NOSPLIT,$0
	MOVW	0(FP), R4
	MOVW	4(FP), R5
	MOVW	8(FP), R6
	MOVW	$SYS_sched_getaffinity, R2
	MOVW	$ret+12(FP), R16
	JMP	scall<>(SB)

TEXT runtime·epollcreate(SB),NOSPLIT,$0
	MOVW	size+0(FP), R4
	MOVW	$SYS_epoll_create, R2
	MOVW	$ret+4(FP), R16
	JMP	scall<>(SB)

TEXT runtime·epollcreate1(SB),NOSPLIT,$0
	MOVW	flags+0(FP), R4
	MOVW	$SYS_epoll_create1, R2
	MOVW	$ret+4(FP), R16
	JMP	scall<>(SB)

TEXT runtime·epollctl(SB),NOSPLIT,$0
	MOVW	epfd+0(FP), R4
	MOVW	op+4(FP), R5
	MOVW	fd+8(FP), R6
	MOVW	ev+12(FP), R7
	MOVW	$SYS_epoll_ctl, R2
	MOVW	$ret+16(FP), R16
	JMP	scall<>(SB)

TEXT runtime·epollwait(SB),NOSPLIT,$0
	MOVW	epfd+0(FP), R4
	MOVW	ev+4(FP), R5
	MOVW	nev+8(FP), R6
	MOVW	timeout+12(FP), R7
	MOVW	$SYS_epoll_wait, R2
	MOVW	$ret+16(FP), R16
	JMP	scall<>(SB)

TEXT runtime·closeonexec(SB),NOSPLIT,$0
	MOVW	fd+0(FP), R4
	MOVW	$2, R5
	MOVW	$1, R6
	MOVW	$SYS_fcntl, R2
	MOVW	R0, R16
	JMP	scall<>(SB)
