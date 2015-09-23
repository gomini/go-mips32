
#ifndef	EXTERN
#define	EXTERN	extern
#endif

#include "../gc/go.h"
#include "../vl/v.out.h"

#define TEXTFLAG reg

EXTERN	int	isbigendian;
EXTERN	int32	dynloc;
EXTERN	uchar	reg[NREG+NREG];
EXTERN	int32	pcloc;
EXTERN	Strlit	emptystring;
EXTERN	Prog	zprog;
EXTERN	Node*	newproc;
EXTERN	Node*	deferproc;
EXTERN	Node*	deferreturn;
EXTERN	Node*	panicindex;
EXTERN	Node*	panicslice;
EXTERN	Node*	panicdiv;
EXTERN	Node*	throwreturn;
extern	long	unmappedzero;

void	cgen_callinter(Node*, Node*, int);
void	cgen_callret(Node*, Node*);
void	cgen_aret(Node*, Node*);
void	cgen_div(int, Node*, Node*, Node*);
void	cgen_hmul(Node*, Node*, Node*);
void	cgen_shift(int, int, Node*, Node*, Node*);
void	ginscall(Node*, int);

int	needconvert(Type*, Type*);
void	genconv(Type*, Type*);
void	allocparams(void);

void	agen(Node*, Node*);
void	igen(Node*, Node*, Node*);
vlong	fieldoffset(Type*, Node*);
void	sgen(Node*, Node*, int64);
int	componentgen(Node*, Node*);

void	cmp64(Node*, Node*, int, int, Prog*);
void	cgen64(Node*, Node*);

void	clearp(Prog*);
Prog*	tbranch(int as, Node*, Node*, int);
Prog*	zbranch(int, Node*, int);
Prog*	gjmp(Prog*);
Plist*	newplist(void);
void	gtrack(Sym*);
void	afunclit(Addr*, Node*);
void	ginit(void);
void	gclean(void);
void	regalloc(Node*, Type*, Node*);
void	regfree(Node*);
void	nodreg(Node*, Type*, int);
void	nodindreg(Node*, Type*, int);
Node*	nodarg(Type*, int);
Node*	ncon(uint32);
Node*	nscon(int32);
int	split64(Node*, Node*, Node*);
void	splitclean(void);
void	gmove(Node*, Node*);
Prog*	gins(int, Node*, Node*);
int	samaddr(Node*, Node*);
Prog*	gins3(int, Node*, Node*, Node*);
void	raddr(Node *n, Prog *p);
void	naddr(Node*, Addr*, int);
void	gencmpcode(int, Node*, Node*, int, Prog*);
int	optoas(int, Type*);
int	dotaddable(Node*, Node*);
void	sudoclean(void);
int	sudoaddable(int, Node*, Addr*);

void	datagostring(Strlit*, Addr*);
void	datastring(char*, int, Addr*);
