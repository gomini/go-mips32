

enum {
	EINTR	= 0x4,
	EAGAIN	= 0xb,
	ENOMEM	= 0xc,

	PROT_NONE	= 0x0,
	PROT_READ	= 0x1,
	PROT_WRITE	= 0x2,
	PROT_EXEC	= 0x4,

	MAP_ANON	= 0x800,
	MAP_PRIVATE	= 0x2,
	MAP_FIXED	= 0x10,

	MADV_DONTNEED	= 0x4,

	SA_RESTART	= 0x10000000,
	SA_ONSTACK	= 0x8000000,
	SA_RESTORER	= 0,
	SA_SIGINFO	= 0x8,

	SIGHUP		= 0x1,
	SIGINT		= 0x2,
	SIGQUIT		= 0x3,
	SIGILL		= 0x4,
	SIGTRAP		= 0x5,
	SIGABRT		= 0x6,
	SIGBUS		= 0xa,
	SIGFPE		= 0x8,
	SIGKILL		= 0x9,
	SIGUSR1		= 0x10,
	SIGSEGV		= 0xb,
	SIGUSR2		= 0x11,
	SIGPIPE		= 0xd,
	SIGALRM		= 0xe,
	SIGSTKFLT	= 0,
	SIGCHLD		= 0x12,
	SIGCONT		= 0x19,
	SIGSTOP		= 0x17,
	SIGTSTP		= 0x18,
	SIGTTIN		= 0x1a,
	SIGTTOU		= 0x1b,
	SIGURG		= 0x15,
	SIGXCPU		= 0x1e,
	SIGXFSZ		= 0x1f,
	SIGVTALRM	= 0x1c,
	SIGPROF		= 0x1d,
	SIGWINCH	= 0x14,
	SIGIO		= 0x16,
	SIGPWR		= 0x13,
	SIGSYS		= 0xc,

	FPE_INTDIV	= 0x1,
	FPE_INTOVF	= 0x2,
	FPE_FLTDIV	= 0x3,
	FPE_FLTOVF	= 0x4,
	FPE_FLTUND	= 0x5,
	FPE_FLTRES	= 0x6,
	FPE_FLTINV	= 0x7,
	FPE_FLTSUB	= 0x8,

	BUS_ADRALN	= 0x1,
	BUS_ADRERR	= 0x2,
	BUS_OBJERR	= 0x3,

	SEGV_MAPERR	= 0x1,
	SEGV_ACCERR	= 0x2,

	ITIMER_REAL	= 0x0,
	ITIMER_VIRTUAL	= 0x1,
	ITIMER_PROF	= 0x2,

	O_RDONLY	= 0x0,
	O_CLOEXEC	= 0x80000,

	EPOLLIN		= 0x1,
	EPOLLOUT	= 0x4,
	EPOLLERR	= 0x8,
	EPOLLHUP	= 0x10,
	EPOLLRDHUP	= 0x2000,
	EPOLLET		= -0x80000000,
	EPOLL_CLOEXEC	= 0x80000,
	EPOLL_CTL_ADD	= 0x1,
	EPOLL_CTL_DEL	= 0x2,
	EPOLL_CTL_MOD	= 0x3,
};

typedef struct Timespec Timespec;
typedef struct Timeval Timeval;
typedef struct SigsetT SigsetT;
typedef struct Siginfo Siginfo;
typedef struct SigactionT SigactionT;
typedef struct SigaltstackT SigaltstackT;
typedef struct Sigcontext Sigcontext;
typedef struct Ucontext Ucontext;
typedef struct Itimerval Itimerval;
typedef struct EpollEvent EpollEvent;

#pragma pack on

struct Timespec {
	int32	tv_sec;
	int32	tv_nsec;
};
struct Timeval {
	int32	tv_sec;
	int32	tv_usec;
};
struct SigsetT {
	uint32	sig[4];
};
struct Siginfo {
	int32	si_signo;
	int32	si_code;
	int32	si_errno;
	byte	_sifields[116];
};
struct SigactionT {
	uint32	sa_flags;
	void	*sa_handler;
	SigsetT	sa_mask;
};
struct SigaltstackT {
	byte	*ss_sp;
	uint32	ss_size;
	int32	ss_flags;
};
struct Sigcontext {
	uint32	sc_regmask;
	uint32	sc_status;
	uint64	sc_pc;
	uint64	sc_regs[32];
	uint64	sc_fpregs[32];
	uint32	sc_acx;
	uint32	sc_fpc_csr;
	uint32	sc_fpc_eir;
	uint32	sc_used_math;
	uint32	sc_dsp;
	byte	Pad_cgo_0[4];
	uint64	sc_mdhi;
	uint64	sc_mdlo;
	uint32	sc_hi1;
	uint32	sc_lo1;
	uint32	sc_hi2;
	uint32	sc_lo2;
	uint32	sc_hi3;
	uint32	sc_lo3;
};
struct Ucontext {
	uint32	uc_flags;
	Ucontext	*uc_link;
	SigaltstackT	uc_stack;
	byte	Pad_cgo_0[4];
	Sigcontext	uc_mcontext;
	SigsetT	uc_sigmask;
};
struct Itimerval {
	Timeval	it_interval;
	Timeval	it_value;
};
struct EpollEvent {
	uint32	events;
	byte	Pad_cgo_0[4];
	uint64	data;
};


#pragma pack off
