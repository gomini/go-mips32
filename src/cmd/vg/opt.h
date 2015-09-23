
#include	"../gc/popt.h"

#define	Z	N

#define	BLOAD(r)	band(bnot(r->refbehind), r->refahead)
#define	BSTORE(r)	band(bnot(r->calbehind), r->calahead)
#define	LOAD(r)		(~r->refbehind.b[z] & r->refahead.b[z])
#define	STORE(r)	(~r->calbehind.b[z] & r->calahead.b[z])

#define	CLOAD	5
#define	CREF	5
#define	CINF	1000
#define	LOOP	3

typedef	struct	Reg	Reg;
typedef	struct	Rgn	Rgn;


struct	Reg
{
	Flow	f;

	Bits	set;
	Bits	use1;
	Bits	use2;

	Bits	refbehind;
	Bits	refahead;
	Bits	calbehind;
	Bits	calahead;
	Bits	regdiff;
	Bits	act;

	int32	regu;
	Reg*	log5;
};
#define	R	((Reg*)0)

#define	NRGN	600
struct	Rgn
{
	Reg*	enter;
	short	cost;
	short	varno;
	short	regno;
};

EXTERN	int32	exregoffset;
EXTERN	int32	exfregoffset;

EXTERN	Rgn	region[NRGN];
EXTERN	Rgn*	rgp;
EXTERN	int	nregion;
EXTERN	int	nvar;

EXTERN	Bits	externs;
EXTERN	Bits	params;
EXTERN	Bits	consts;
EXTERN	Bits	addrs;

EXTERN	int32	regbits;

EXTERN	int	change;

EXTERN	Reg	zreg;
EXTERN	Reg*	freer;
EXTERN	int32*	idom;
EXTERN	Reg**	rpo2r;
EXTERN	int32	maxnr;

EXTERN	Bits	ivar;
EXTERN	Bits	ovar;


void	excise(Flow*);
void	regopt(Prog*);
void	prop(Reg*, Bits, Bits);
int32	RtoB(int);
int32	FtoB(int);
int	BtoR(int32);
int	BtoF(int32);

void	dumpit(char *str, Flow *r0, int);

void	peep(Prog*);

typedef struct ProgInfo ProgInfo;
struct ProgInfo
{
	uint32 flags;
};

enum
{
	Pseudo = 1<<1,
	
	OK = 1<<2,

	SizeB = 1<<3,
	SizeW = 1<<4,
	SizeL = 1<<5,
	SizeQ = 1<<6,
	SizeF = 1<<7,
	SizeD = 1<<8,

	LeftAddr = 1<<9,
	LeftRead = 1<<10,
	LeftWrite = 1<<11,
	
	RegRead = 1<<12,
	CanRegRead = 1<<13,
	
	RightAddr = 1<<14,
	RightRead = 1<<15,
	RightWrite = 1<<16,

	Move = 1<<17,
	Conv = 1<<18,
	Cjmp = 1<<19,
	Break = 1<<20,
	Call = 1<<21,
	Jump = 1<<22,
	Skip = 1<<23,
};

void proginfo(ProgInfo*, Prog*);

enum
{
	ACALL = AJAL,
};
