#include	<u.h>
#include	<libc.h>
#include	<bio.h>
#include	<link.h>
#include	"v.out.h"

#ifndef	EXTERN
#define	EXTERN	extern
#endif

enum
{
	thechar = 'v',
	PtrSize = 4,
	IntSize = 4,
	RegSize = 4,
	MaxAlign = 8,
	FuncAlign = 4,
};

#define	P		((Prog*)0)
#define	S		((LSym*)0)

enum
{
	MINLC		= 4,
};

EXTERN	int32	autosize;
EXTERN	LSym*	datap;
EXTERN	int	debug[128];
EXTERN	Prog*	lastp;
EXTERN	int32	lcsize;
EXTERN	char	literal[32];
EXTERN	int	nerrors;
EXTERN	int32	instoffset;
EXTERN	char*	rpath;
EXTERN	int32	symsize;

void	asmb(void);
void	adddynlib(char *lib);
void	adddynrel(LSym *s, Reloc *r);
void	adddynsym(Link *ctxt, LSym *s);
int	archreloc(Reloc *r, LSym *s, vlong *val);
void	listinit(void);
int32	rnd(int32, int32);

#define	LPUT(a)	(ctxt->arch->endian == BigEndian ? lputb(a):lputl(a))
#define	WPUT(a)	(ctxt->arch->endian == BigEndian ? wputb(a):wputl(a))
#define	VPUT(a)	(ctxt->arch->endian == BigEndian ? vputb(a):vputl(a))

enum
{
	DWARFREGSP = 29
};
