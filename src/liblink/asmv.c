#include <u.h>
#include <libc.h>
#include <bio.h>
#include <link.h>
#include "../cmd/vl/v.out.h"
#include "../runtime/stack.h"

enum {
	FuncAlign = 8,
};

typedef	struct	Optab	Optab;
typedef struct	Oprang	Oprang;
typedef uchar	Opcross[32][2][32];

struct	Optab
{
	char	as;
	char	a1;
	char	a2;
	char	a3;
	char	type;
	char	size;
	char	param;
};
struct Oprang
{
	Optab*	start;
	Optab*	stop;
};

Optab	optab[] =
{

	{ ATEXT,	C_LEXT,	C_NONE,	C_LCON, 	 0, 0, 0 },
	{ ATEXT,	C_LEXT,	C_REG,	C_LCON, 	 0, 0, 0 },

	{ AMOVW,	C_REG,	C_NONE,	C_REG,		 1, 4, 0 },
	{ AMOVB,	C_REG,	C_NONE,	C_REG,		12, 4, 0 },
	{ AMOVBU,	C_REG,	C_NONE,	C_REG,		13, 4, 0 },

	{ ASUB,		C_REG,	C_REG,	C_REG,		 2, 4, 0 },
	{ AADD,		C_REG,	C_REG,	C_REG,		 2, 4, 0 },
	{ AAND,		C_REG,	C_REG,	C_REG,		 2, 4, 0 },
	{ ASUB,		C_REG,	C_NONE,	C_REG,		 2, 4, 0 },
	{ AADD,		C_REG,	C_NONE,	C_REG,		 2, 4, 0 },
	{ AAND,		C_REG,	C_NONE,	C_REG,		 2, 4, 0 },

	{ ASLL,		C_REG,	C_NONE,	C_REG,		 9, 4, 0 },
	{ ASLL,		C_REG,	C_REG,	C_REG,		 9, 4, 0 },

	{ AADDF,	C_FREG,	C_NONE,	C_FREG,		32, 4, 0 },
	{ AADDF,	C_FREG,	C_REG,	C_FREG,		32, 4, 0 },
	{ ACMPEQF,	C_FREG,	C_REG,	C_NONE,		32, 4, 0 },
	{ AABSF,	C_FREG,	C_NONE,	C_FREG,		33, 4, 0 },
	{ AMOVF,	C_FREG,	C_NONE,	C_FREG,		33, 4, 0 },
	{ AMOVD,	C_FREG,	C_NONE,	C_FREG,		33, 4, 0 },

	{ AMOVW,	C_REG,	C_NONE,	C_SEXT,		 7, 4, 0},
	{ AMOVB,	C_REG,	C_NONE,	C_SEXT,		 7, 4, 0},
	{ AMOVBU,	C_REG,	C_NONE,	C_SEXT,		 7, 4, 0},
	{ AMOVWL,	C_REG,	C_NONE,	C_SEXT,		 7, 4, 0},
	{ AMOVW,	C_REG,	C_NONE,	C_SAUTO,	 7, 4, REGSP },
	{ AMOVB,	C_REG,	C_NONE,	C_SAUTO,	 7, 4, REGSP },
	{ AMOVBU,	C_REG,	C_NONE,	C_SAUTO,	 7, 4, REGSP },
	{ AMOVWL,	C_REG,	C_NONE,	C_SAUTO,	 7, 4, REGSP },
	{ AMOVW,	C_REG,	C_NONE,	C_SOREG,	 7, 4, REGZERO },
	{ AMOVB,	C_REG,	C_NONE,	C_SOREG,	 7, 4, REGZERO },
	{ AMOVBU,	C_REG,	C_NONE,	C_SOREG,	 7, 4, REGZERO },
	{ AMOVWL,	C_REG,	C_NONE,	C_SOREG,	 7, 4, REGZERO },

	{ AMOVW,	C_SEXT,	C_NONE,	C_REG,		 8, 4, 0},
	{ AMOVB,	C_SEXT,	C_NONE,	C_REG,		 8, 4, 0},
	{ AMOVBU,	C_SEXT,	C_NONE,	C_REG,		 8, 4, 0},
	{ AMOVWL,	C_SEXT,	C_NONE,	C_REG,		 8, 4, 0},
	{ AMOVW,	C_SAUTO,C_NONE,	C_REG,		 8, 4, REGSP },
	{ AMOVB,	C_SAUTO,C_NONE,	C_REG,		 8, 4, REGSP },
	{ AMOVBU,	C_SAUTO,C_NONE,	C_REG,		 8, 4, REGSP },
	{ AMOVWL,	C_SAUTO,C_NONE,	C_REG,		 8, 4, REGSP },
	{ AMOVW,	C_SOREG,C_NONE,	C_REG,		 8, 4, REGZERO },
	{ AMOVB,	C_SOREG,C_NONE,	C_REG,		 8, 4, REGZERO },
	{ AMOVBU,	C_SOREG,C_NONE,	C_REG,		 8, 4, REGZERO },
	{ AMOVWL,	C_SOREG,C_NONE,	C_REG,		 8, 4, REGZERO },

	{ AMOVW,	C_REG,	C_NONE,	C_LEXT,		35, 12, 0},
	{ AMOVB,	C_REG,	C_NONE,	C_LEXT,		35, 12, 0},
	{ AMOVBU,	C_REG,	C_NONE,	C_LEXT,		35, 12, 0},
	{ AMOVW,	C_REG,	C_NONE,	C_LAUTO,	35, 16, REGSP },
	{ AMOVB,	C_REG,	C_NONE,	C_LAUTO,	35, 16, REGSP },
	{ AMOVBU,	C_REG,	C_NONE,	C_LAUTO,	35, 16, REGSP },
	{ AMOVW,	C_REG,	C_NONE,	C_LOREG,	35, 16, REGZERO },
	{ AMOVB,	C_REG,	C_NONE,	C_LOREG,	35, 16, REGZERO },
	{ AMOVBU,	C_REG,	C_NONE,	C_LOREG,	35, 16, REGZERO },

	{ AMOVW,	C_LEXT,	C_NONE,	C_REG,		36, 12, 0},
	{ AMOVB,	C_LEXT,	C_NONE,	C_REG,		36, 12, 0},
	{ AMOVBU,	C_LEXT,	C_NONE,	C_REG,		36, 12, 0},
	{ AMOVW,	C_LAUTO,C_NONE,	C_REG,		36, 16, REGSP },
	{ AMOVB,	C_LAUTO,C_NONE,	C_REG,		36, 16, REGSP },
	{ AMOVBU,	C_LAUTO,C_NONE,	C_REG,		36, 16, REGSP },
	{ AMOVW,	C_LOREG,C_NONE,	C_REG,		36, 16, REGZERO },
	{ AMOVB,	C_LOREG,C_NONE,	C_REG,		36, 16, REGZERO },
	{ AMOVBU,	C_LOREG,C_NONE,	C_REG,		36, 16, REGZERO },

	{ AMOVW,	C_SECON,C_NONE,	C_REG,		 3, 4, 0},
	{ AMOVW,	C_SACON,C_NONE,	C_REG,		 3, 4, REGSP },
	{ AMOVW,	C_LECON,C_NONE,	C_REG,		26, 12, 0},
	{ AMOVW,	C_LACON,C_NONE,	C_REG,		26, 12, REGSP },
	{ AMOVW,	C_ADDCON,C_NONE,C_REG,		 3, 4, REGZERO },
	{ AMOVW,	C_ANDCON,C_NONE,C_REG,		 3, 4, REGZERO },

	{ AMOVW,	C_UCON, C_NONE, C_REG,		24, 4, 0 },
	{ AMOVW,	C_LCON,	C_NONE,	C_REG,		19, 8, 0 },

	{ AMOVW,	C_HI,	C_NONE,	C_REG,		20, 4, 0 },
	{ AMOVW,	C_LO,	C_NONE,	C_REG,		20, 4, 0 },
	{ AMOVW,	C_REG,	C_NONE,	C_HI,		21, 4, 0 },
	{ AMOVW,	C_REG,	C_NONE,	C_LO,		21, 4, 0 },

	{ AMUL,		C_REG,	C_REG,	C_NONE,		22, 4, 0 },

	{ AADD,		C_ADD0CON,C_REG,C_REG,		 4, 4, 0 },
	{ AADD,		C_ADD0CON,C_NONE,C_REG,		 4, 4, 0 },
	{ AADD,		C_ANDCON,C_REG,	C_REG,		10, 8, 0 },
	{ AADD,		C_ANDCON,C_NONE,C_REG,		10, 8, 0 },

	{ AAND,		C_AND0CON,C_REG,C_REG,		 4, 4, 0 },
	{ AAND,		C_AND0CON,C_NONE,C_REG,		 4, 4, 0 },
	{ AAND,		C_ADDCON,C_REG,	C_REG,		10, 8, 0 },
	{ AAND,		C_ADDCON,C_NONE,C_REG,		10, 8, 0 },

	{ AADD,		C_UCON,	C_REG,	C_REG,		25, 8, 0 },
	{ AADD,		C_UCON,	C_NONE,	C_REG,		25, 8, 0 },
	{ AAND,		C_UCON,	C_REG,	C_REG,		25, 8, 0 },
	{ AAND,		C_UCON,	C_NONE,	C_REG,		25, 8, 0 },

	{ AADD,		C_LCON,	C_NONE,	C_REG,		23, 12, 0 },
	{ AAND,		C_LCON,	C_NONE,	C_REG,		23, 12, 0 },
	{ AADD,		C_LCON,	C_REG,	C_REG,		23, 12, 0 },
	{ AAND,		C_LCON,	C_REG,	C_REG,		23, 12, 0 },

	{ ASLL,		C_SCON,	C_REG,	C_REG,		16, 4, 0 },
	{ ASLL,		C_SCON,	C_NONE,	C_REG,		16, 4, 0 },
	{ ASYSCALL,	C_NONE,	C_NONE,	C_NONE,		 5, 4, 0 },

	{ ABEQ,		C_REG,	C_REG,	C_SBRA,		 6, 4, 0 },
	{ ABEQ,		C_REG,	C_NONE,	C_SBRA,		 6, 4, 0 },
	{ ABLEZ,	C_REG,	C_NONE,	C_SBRA,		 6, 4, 0 },
	{ ABFPT,	C_NONE,	C_NONE,	C_SBRA,		 6, 4, 0 },

	{ AJMP,		C_NONE,	C_NONE,	C_LBRA,		11, 4, 0 },
	{ AJAL,		C_NONE,	C_NONE,	C_LBRA,		11, 4, 0 },
	{ AJAL,		C_REG,	C_NONE,	C_LBRA,		11, 4, 0 },

	{ AJMP,		C_NONE,	C_NONE,	C_ZOREG,	18, 4, 0 },
	{ AJAL,		C_NONE,	C_NONE,	C_ZOREG,	18, 4, REGLINK },
	{ AJAL,		C_REG,	C_NONE, C_ZOREG,	18, 4, REGLINK },

	{ AMOVW,	C_SEXT,	C_NONE,	C_FREG,		27, 4, 0},
	{ AMOVF,	C_SEXT,	C_NONE,	C_FREG,		27, 4, 0},
	{ AMOVD,	C_SEXT,	C_NONE,	C_FREG,		27, 8, 0},
	{ AMOVW,	C_SAUTO,C_NONE,	C_FREG,		27, 4, REGSP },
	{ AMOVF,	C_SAUTO,C_NONE,	C_FREG,		27, 4, REGSP },
	{ AMOVD,	C_SAUTO,C_NONE,	C_FREG,		27, 8, REGSP },
	{ AMOVW,	C_SOREG,C_NONE,	C_FREG,		27, 4, REGZERO },
	{ AMOVF,	C_SOREG,C_NONE,	C_FREG,		27, 4, REGZERO },
	{ AMOVD,	C_SOREG,C_NONE,	C_FREG,		27, 8, REGZERO },

	{ AMOVW,	C_LEXT,	C_NONE,	C_FREG,		27, 12, 0},
	{ AMOVF,	C_LEXT,	C_NONE,	C_FREG,		27, 12, 0},
	{ AMOVD,	C_LEXT,	C_NONE,	C_FREG,		27, 16, 0},
	{ AMOVW,	C_LAUTO,C_NONE,	C_FREG,		27, 16, REGSP },
	{ AMOVF,	C_LAUTO,C_NONE,	C_FREG,		27, 16, REGSP },
	{ AMOVD,	C_LAUTO,C_NONE,	C_FREG,		27, 20, REGSP },
	{ AMOVW,	C_LOREG,C_NONE,	C_FREG,		27, 16, REGZERO },
	{ AMOVF,	C_LOREG,C_NONE,	C_FREG,		27, 16, REGZERO },
	{ AMOVD,	C_LOREG,C_NONE,	C_FREG,		27, 20, REGZERO },

	{ AMOVW,	C_FREG,	C_NONE,	C_SEXT,		28, 4, 0},
	{ AMOVF,	C_FREG,	C_NONE,	C_SEXT,		28, 4, 0},
	{ AMOVD,	C_FREG,	C_NONE,	C_SEXT,		28, 8, 0},
	{ AMOVW,	C_FREG,	C_NONE,	C_SAUTO,	28, 4, REGSP },
	{ AMOVF,	C_FREG,	C_NONE,	C_SAUTO,	28, 4, REGSP },
	{ AMOVD,	C_FREG,	C_NONE,	C_SAUTO,	28, 8, REGSP },
	{ AMOVW,	C_FREG,	C_NONE,	C_SOREG,	28, 4, REGZERO },
	{ AMOVF,	C_FREG,	C_NONE,	C_SOREG,	28, 4, REGZERO },
	{ AMOVD,	C_FREG,	C_NONE,	C_SOREG,	28, 8, REGZERO },

	{ AMOVW,	C_FREG,	C_NONE,	C_LEXT,		28, 12, 0},
	{ AMOVF,	C_FREG,	C_NONE,	C_LEXT,		28, 12, 0},
	{ AMOVD,	C_FREG,	C_NONE,	C_LEXT,		28, 16, 0},
	{ AMOVW,	C_FREG,	C_NONE,	C_LAUTO,	28, 16, REGSP },
	{ AMOVF,	C_FREG,	C_NONE,	C_LAUTO,	28, 16, REGSP },
	{ AMOVD,	C_FREG,	C_NONE,	C_LAUTO,	28, 20, REGSP },
	{ AMOVW,	C_FREG,	C_NONE,	C_LOREG,	28, 16, REGZERO },
	{ AMOVF,	C_FREG,	C_NONE,	C_LOREG,	28, 16, REGZERO },
	{ AMOVD,	C_FREG,	C_NONE,	C_LOREG,	28, 20, REGZERO },

	{ AMOVW,	C_REG,	C_NONE,	C_FREG,		30, 4, 0 },
	{ AMOVW,	C_FREG,	C_NONE,	C_REG,		31, 4, 0 },

	{ AMOVW,	C_ADDCON,C_NONE,C_FREG,		34, 8, 0 },
	{ AMOVW,	C_ANDCON,C_NONE,C_FREG,		34, 8, 0 },
	{ AMOVW,	C_UCON, C_NONE, C_FREG,		35, 8, 0 },
	{ AMOVW,	C_LCON,	C_NONE,	C_FREG,		36, 12, 0 },

	{ AMOVW,	C_REG,	C_NONE,	C_MREG,		37, 4, 0 },
	{ AMOVW,	C_MREG,	C_NONE,	C_REG,		38, 4, 0 },

	{ AWORD,	C_NONE,	C_NONE,	C_LCON,		40, 4, 0 },

	{ AMOVW,	C_REG,	C_NONE,	C_FCREG,	41, 8, 0 },
	{ AMOVW,	C_FCREG,C_NONE,	C_REG,		42, 4, 0 },



	{ AERET,	C_NONE, C_NONE, C_NONE,		5,  4, 0 },
	{ AROTR,	C_REG,	C_NONE,	C_REG,		 9, 4, 0 },
	{ AROTR,	C_REG,	C_REG,	C_REG,		 9, 4, 0 },
	{ AROTR,	C_SCON, C_REG,	C_REG,		16, 4, 0 },
	{ AROTR,	C_SCON, C_REG,	C_REG,		16, 4, 0 },

	{ AMAD,		C_REG,	C_REG,	C_NONE,		22, 4, 0 },
	{ AMADU,	C_REG,	C_REG,	C_NONE,		22, 4, 0 },
	{ AMSUB,	C_REG,	C_REG,	C_NONE,		22, 4, 0 },
	{ AMSUBU,	C_REG,	C_REG,	C_NONE,		22, 4, 0 },

	{ AMUL32,	C_REG,	C_REG,	C_REG,		47, 4, 0 },
	{ AMOVZ,	C_REG,	C_REG,	C_REG,		48, 4, 0 },
	{ AMOVN,	C_REG,	C_REG,	C_REG,		48, 4, 0 },
	{ ACLZ,		C_REG,	C_NONE,	C_REG,		49, 4, 0 },
	{ ACLO,		C_REG,	C_NONE,	C_REG,		49, 4, 0 },
	{ ASEB,		C_REG,	C_NONE,	C_REG,		50, 4, 0 },
	{ ASEH,		C_REG,	C_NONE, C_REG,		50, 4, 0 },
	{ AWSBH,	C_REG,	C_NONE,	C_REG,		50, 4, 0 },

	{ APREF,	C_SCON,	C_NONE,	C_SAUTO,	51, 4, REGSP },
	{ APREF,	C_SCON,	C_NONE,	C_SOREG,	51, 4, REGZERO },
	{ APREF,	C_SCON,	C_NONE,	C_ZOREG,	51, 4, REGZERO },
	{ APREF,	C_SCON, C_NONE, C_LEXT,		52, 8, 0},
	{ APREF,	C_SCON, C_NONE, C_LAUTO,	53, 16, REGSP },
	{ APREF,	C_SCON, C_NONE,	C_LOREG,	53, 16, REGZERO },

	{ ALL,		C_SAUTO,C_NONE, C_REG,		54, 4, REGSP },
	{ ALL,		C_SOREG,C_NONE,	C_REG,		54, 4, REGZERO },
	{ ALL,		C_ZOREG,C_NONE,	C_REG,		54, 4, REGZERO },
	{ ALL,		C_LEXT, C_NONE, C_REG,		55, 8, 0},
	{ ALL,		C_LAUTO,C_NONE, C_REG,		56, 16, REGSP },
	{ ALL,		C_LOREG,C_NONE, C_REG,		56, 16, REGZERO },

	{ ASC,		C_REG,	C_NONE,	C_SAUTO,	57, 4, REGSP },
	{ ASC,		C_REG,	C_NONE,	C_SOREG,	57, 4, REGZERO },
	{ ASC,		C_REG,	C_NONE,	C_ZOREG,	57, 4, REGZERO },
	{ ASC,		C_REG,	C_NONE,	C_LEXT,		58, 8, 0},
	{ ASC,		C_REG,	C_NONE,	C_LAUTO,	59, 16, REGSP },
	{ ASC,		C_REG,	C_NONE,	C_LOREG,	59, 16, REGZERO },

	{ ABREAK,	C_LCON, C_NONE, C_NONE,		70, 4, 0},

	{ AUNDEF,	C_NONE, C_NONE, C_NONE,		80, 4, 0 },
	{ AUSEFIELD,	C_LEXT, C_NONE, C_NONE,		0, 0, 0 },
	{ APCDATA,	C_LCON, C_NONE, C_LCON,		0, 0, 0 },
	{ AFUNCDATA,	C_LCON, C_NONE, C_LEXT,		0, 0, 0 },
	{ ANOP,		C_NONE,	C_NONE, C_NONE,		0, 0, 0 },

	{ ADUFFZERO,	C_NONE, C_NONE, C_LBRA,		11, 4, 0 },
	{ ADUFFCOPY,	C_NONE, C_NONE, C_LBRA,		11, 4, 0 },

	{ AXXX,		C_NONE,	C_NONE,	C_NONE,		 0, 4, 0 },
};

static int	cmp(int, int);
static int	ocmp(const void*, const void*);
static void	buildop(Link*);
static void	buildrep(Link*, int, int);
static void	prasm(Prog*);
static Optab*	oplook(Link*, Prog*);
static void	asmout(Link*, Prog*, Optab*, int32*);
static int32	oprrr(Link*, int);
static int32	opirr(Link*, int);
static void	addaddrreloc(Link*, LSym*, int*, int*);

static Oprang	oprange[ALAST];
static Opcross	opcross[10];
static uchar	repop[ALAST];
static uchar	xcmp[C_NCLASS][C_NCLASS];

void
prepareoprange(Link *ctxt)
{
	if(oprange[AAND].start == nil)
		buildop(ctxt);
}

void
spanv(Link *ctxt, LSym *cursym)
{
	Prog *p, *q;
	Optab *o;
	int m, bflag, i, j;
	int32 out[4], c, otxt;
	uchar *bp, *cast;

	p = cursym->text;
	if(p == nil || p->link == nil)
		return;
	ctxt->cursym = cursym;
	ctxt->autosize = p->to.offset + 4;

	prepareoprange(ctxt);

	for(c = 0, p = p->link; p != nil; p = p->link) {
		ctxt->curp = p;
		p->pc = c;
		o = oplook(ctxt, p);
		m = o->size;
		if(m == 0) {
			if(p->as != ANOP && p->as != AFUNCDATA && p->as != APCDATA)
				ctxt->diag("zero-width instruction\n%P", p);
			continue;
		}
		c += m;
	}
	cursym->size = c;

	bflag = 1;
	while(bflag) {
		if(ctxt->debugvlog)
			Bprint(ctxt->bso, "%5.2f span1\n", cputime());
		bflag = 0;
		c = 0;
		cursym->text->pc = 0;
		for(p = cursym->text->link; p != nil; p = p->link) {
			p->pc = c;
			o = oplook(ctxt, p);
			if(o->type == 6 && p->pcond) {
				otxt = p->pcond->pc - c;
				if(otxt < 0)
					otxt = - otxt;
				if(otxt >= (1L<<17) - 10) {
					q = ctxt->arch->prg();
					q->link = p->link;
					p->link = q;
					q->as = AJMP;
					q->to.type = D_BRANCH;
					q->pcond = p->pcond;
					p->pcond = q;
					q = ctxt->arch->prg();
					q->link = p->link;
					p->link = q;
					q->as = AJMP;
					q->to.type = D_BRANCH;
					q->pcond = q->link->link;
					addnopv(p->link);
					addnopv(p);
					bflag = 1;
				}
			}

			m = o->size;
			if(m == 0) {
				if(p->as != ANOP && p->as != AFUNCDATA && p->as != APCDATA)
					ctxt->diag("zero-width instruction\n%P", p);
				continue;
			}
			c += m;
		}
		cursym->size = c;
	}

	c += -c&(FuncAlign-1);
	cursym->size = c;

	if(ctxt->tlsg == nil)
		ctxt->tlsg = linklookup(ctxt, "runtime.tlsg", 0);

	symgrow(ctxt, cursym, cursym->size);

	bp = cursym->p;
	for(p = cursym->text->link; p != nil; p = p->link) {
		ctxt->pc = p->pc;
		ctxt->curp = p;
		o = oplook(ctxt, p);
		if(o->size > 4*nelem(out))
			sysfatal("out array in spanv is too small, need at least %d for %P", o->size/4, p);
		asmout(ctxt, p, o, out);
		for(i=0; i<o->size/4; i++) {
			cast = (uchar*)&out[i];
			for(j=0; j<4; j++)
				*bp++ = cast[inuxi4[j]];
		}
	}
}

int
aclassv(Link *ctxt, Addr *a)
{
	switch(a->type) {
	case D_NONE:
		return C_NONE;
	case D_REG:
		return C_REG;
	case D_FREG:
		return C_FREG;
	case D_FCREG:
		return C_FCREG;
	case D_MREG:
		return C_MREG;
	case D_OREG:
		switch(a->name) {
		case D_EXTERN:
		case D_STATIC:
			if(a->sym == nil || a->sym->name == 0) {
				ctxt->diag("null sym external\n");
				return C_GOK;
			}
			ctxt->instoffset = a->offset;
			return C_LEXT;
		case D_AUTO:
			ctxt->instoffset = ctxt->autosize + a->offset;
			if(ctxt->instoffset >= -BIG && ctxt->instoffset < BIG)
				return C_SAUTO;
			return C_LAUTO;
		case D_PARAM:
			ctxt->instoffset = ctxt->autosize + a->offset + 4L;
			if(ctxt->instoffset >= -BIG && ctxt->instoffset < BIG)
				return C_SAUTO;
			return C_LAUTO;
		case D_NONE:
			ctxt->instoffset = a->offset;
			if(a->offset == 0)
				return C_ZOREG;
			if(a->offset >= -BIG && a->offset < BIG)
				return C_SOREG;
			return C_LOREG;
		}
		return C_GOK;
	case D_HI:
		return C_HI;
	case D_LO:
		return C_LO;
	case D_OCONST:
		switch(a->name) {
		case D_EXTERN:
		case D_STATIC:
			ctxt->instoffset = 0;
			return C_LEXT;
		}
		return C_GOK;
	case D_CONST:
		switch(a->name) {
		case D_NONE:
			ctxt->instoffset = a->offset;
			if(a->reg != NREG) {
				if(ctxt->instoffset >= -BIG && ctxt->instoffset < BIG)
					return C_SACON;
				return C_LACON;
			}
		consize:
			if(ctxt->instoffset == 0)
				return C_ZCON;
			if(ctxt->instoffset > 0) {
				if(ctxt->instoffset <= 0x7fff)
					return C_SCON;
				if(ctxt->instoffset <= 0xffff)
					return C_ANDCON;
				if((ctxt->instoffset & 0xffff) == 0)
					return C_UCON;
				return C_LCON;
			}
			if(ctxt->instoffset >= -0x8000)
				return C_ADDCON;
			if((ctxt->instoffset & 0xffff) == 0)
				return C_UCON;
			return C_LCON;
		case D_EXTERN:
		case D_STATIC:
			if(a->sym == nil)
				break;
			if(a->sym->type == SCONST) {
				ctxt->instoffset = a->sym->value + a->offset;
				goto consize;
			}
			ctxt->instoffset = a->sym->value + a->offset;
			return C_LCON;
		case D_AUTO:
			ctxt->instoffset = ctxt->autosize + a->offset;
			if(ctxt->instoffset >= -BIG && ctxt->instoffset < BIG)
				return C_SACON;
			return C_LACON;
		case D_PARAM:
			ctxt->instoffset = ctxt->autosize + a->offset + 4L;
			if(ctxt->instoffset >= -BIG && ctxt->instoffset < BIG)
				return C_SACON;
			return C_LACON;
		}
		return C_GOK;
	case D_BRANCH:
		return C_SBRA;
	}
	return C_GOK;
}

static void
prasm(Prog *p)
{
	print("%P\n", p);
}

static Optab*
oplook(Link *ctxt, Prog *p)
{
	int a1, a2, a3, r, a1x;
	uchar *c1, *c3;
	Optab *o, *e;

	prepareoprange(ctxt);

	a1 = p->optab;
	if(a1)
		return optab + (a1-1);

	a1 = p->from.class;
	if(a1 == 0) {
		a1 = aclassv(ctxt, &p->from) + 1;
		p->from.class = a1;
	}
	a1--;
	a1x = a1;

	a3 = p->to.class;
	if(a3 == 0) {
		a3 = aclassv(ctxt, &p->to) + 1;
		p->to.class = a3;
	}
	a3--;

	a2 = C_NONE;
	if(p->reg != NREG)
		a2 = C_REG;

	r = p->as;
	o = oprange[r].start;
	if(o == nil) {
		a1 = opcross[repop[r]][a1][a2][a3];
		if(a1) {
			p->optab = a1 + 1;
			return optab + a1;
		}
		o = oprange[r].stop;
	}
	e = oprange[r].stop;
	c1 = xcmp[a1];
	c3 = xcmp[a3];
	for(; o < e; o++)
		if(o->a2 == a2)
		if(c1[(int)(o->a1)])
		if(c3[(int)(o->a3)]) {
			p->optab = (o - optab) + 1;
			return o;
		}
	ctxt->diag("illegal combination %P; %^ %^ %^, %d %d",
		p, a1x, a2, a3, p->from.type, p->to.type);
	ctxt->diag("from %d %d to %d %d\n",
		p->from.type, p->from.name, p->to.type, p->to.name);
	prasm(p);
	sysfatal("oplook failed\n");
	o = optab;
	return 0;
}

static int
cmp(int a, int b)
{
	if(a == b)
		return 1;
	switch(a) {
	case C_LCON:
		if(b == C_ZCON || b == C_SCON || b == C_UCON || b == C_ADDCON || b == C_ANDCON)
			return 1;
		break;
	case C_ADD0CON:
		if(b == C_ADDCON)
			return 1;
	case C_ADDCON:
		if(b == C_ZCON || b == C_SCON)
			return 1;
		break;
	case C_AND0CON:
		if(b == C_ANDCON)
			return 1;
	case C_ANDCON:
		if(b == C_ZCON || b == C_SCON)
			return 1;
		break;
	case C_UCON:
		if(b == C_ZCON)
			return 1;
		break;
	case C_SCON:
		if(b == C_ZCON)
			return 1;
		break;
	case C_LACON:
		if(b == C_SACON)
			return 1;
		break;
	case C_LBRA:
		if(b == C_SBRA)
			return 1;
		break;
	case C_LEXT:
		if(b == C_SEXT)
			return 1;
		break;
	case C_LAUTO:
		if(b == C_SAUTO)
			return 1;
		break;
	case C_REG:
		if(b == C_ZCON)
			return 1;
		break;
	case C_LOREG:
		if(b == C_ZOREG || b == C_SOREG)
			return 1;
		break;
	case C_SOREG:
		if(b == C_ZOREG)
			return 1;
		break;
	}
	return 0;
}

static int
ocmp(const void *a1, const void *a2)
{
	Optab *p1, *p2;
	int n;

	p1 = (Optab*)a1;
	p2 = (Optab*)a2;
	n = p1->as - p2->as;
	if(n)
		return n;
	n = p1->a1 - p2->a1;
	if(n)
		return n;
	n = p1->a2 - p2->a2;
	if(n)
		return n;
	n = p1->a3 - p2->a3;
	if(n)
		return n;
	return 0;
}

static void
buildop(Link *ctxt)
{
	int i, n, r;

	for(i=0; i < C_NCLASS; i++)
		for(n=0; n < C_NCLASS; n++)
			xcmp[i][n] = cmp(n, i);
	for(n=0; optab[n].as != AXXX; n++)
		;
	qsort(optab, n, sizeof(optab[0]), ocmp);
	for(i=0; i<n; i++) {
		r = optab[i].as;
		oprange[r].start = optab + i;
		while(optab[i].as == r)
			i++;
		oprange[r].stop = optab + i;
		i--;

		switch(r) {
		default:
			ctxt->diag("unknown op in build: %A", r);
			sysfatal("buildop: bad code");
		case AABSF:
			oprange[AMOVFD] = oprange[r];
			oprange[AMOVDF] = oprange[r];
			oprange[AMOVWF] = oprange[r];
			oprange[AMOVFW] = oprange[r];
			oprange[AMOVWD] = oprange[r];
			oprange[AMOVDW] = oprange[r];
			oprange[ANEGF] = oprange[r];
			oprange[ANEGD] = oprange[r];
			oprange[AABSD] = oprange[r];
			break;
		case AADD:
			buildrep(ctxt, 1, AADD);
			oprange[ASGT] = oprange[r];
			repop[ASGT] = 1;
			oprange[ASGTU] = oprange[r];
			repop[ASGTU] = 1;
			oprange[AADDU] = oprange[r];
			repop[AADDU] = 1;
			break;
		case AADDF:
			oprange[ADIVF] = oprange[r];
			oprange[ADIVD] = oprange[r];
			oprange[AMULF] = oprange[r];
			oprange[AMULD] = oprange[r];
			oprange[ASUBF] = oprange[r];
			oprange[ASUBD] = oprange[r];
			oprange[AADDD] = oprange[r];
			break;
		case AAND:
			buildrep(ctxt, 2, AAND);
			oprange[AXOR] = oprange[r];
			repop[AXOR] = 2;
			oprange[AOR] = oprange[r];
			repop[AOR] = 2;
			break;
		case ABEQ:
			oprange[ABNE] = oprange[r];
			break;
		case ABLEZ:
			oprange[ABGEZ] = oprange[r];
			oprange[ABGEZAL] = oprange[r];
			oprange[ABLTZ] = oprange[r];
			oprange[ABLTZAL] = oprange[r];
			oprange[ABGTZ] = oprange[r];
			break;
		case AMOVB:
			buildrep(ctxt, 3, AMOVB);
			oprange[AMOVH] = oprange[r];
			repop[AMOVH] = 3;
			break;
		case AMOVBU:
			buildrep(ctxt, 4, AMOVBU);
			oprange[AMOVHU] = oprange[r];
			repop[AMOVHU] = 4;
			break;
		case AMUL:
			oprange[AREM] = oprange[r];
			oprange[AREMU] = oprange[r];
			oprange[ADIVU] = oprange[r];
			oprange[AMULU] = oprange[r];
			oprange[ADIV] = oprange[r];
			break;
		case ASLL:
			oprange[ASRL] = oprange[r];
			oprange[ASRA] = oprange[r];
			break;
		case ASUB:
			oprange[ASUBU] = oprange[r];
			oprange[ANOR] = oprange[r];
			break;
		case ASYSCALL:
			oprange[ATLBP] = oprange[r];
			oprange[ATLBR] = oprange[r];
			oprange[ATLBWI] = oprange[r];
			oprange[ATLBWR] = oprange[r];
			break;
		case ACMPEQF:
			oprange[ACMPGTF] = oprange[r];
			oprange[ACMPGTD] = oprange[r];
			oprange[ACMPGEF] = oprange[r];
			oprange[ACMPGED] = oprange[r];
			oprange[ACMPEQD] = oprange[r];
			break;
		case ABFPT:
			oprange[ABFPF] = oprange[r];
			break;
		case AMOVWL:
			oprange[AMOVWR] = oprange[r];
			break;
		case AMOVW:
			buildrep(ctxt, 5, AMOVW);
			break;
		case AMOVD:
			buildrep(ctxt, 6, AMOVD);
			break;
		case AMOVF:
			buildrep(ctxt, 7, AMOVF);
			break;
		case ABREAK:
		case AJAL:
		case AJMP:
		case AWORD:

		case AERET:
		case AROTR:
		case AMAD:
		case AMADU:
		case AMSUB:
		case AMSUBU:
		case AMUL32:
		case AMOVZ:
		case AMOVN:
		case ACLZ:
		case ACLO:
		case ASEB:
		case ASEH:
		case AWSBH:
		case APREF:
		case ALL:
		case ASC:
		case ATEXT:
		case AUNDEF:
		case AUSEFIELD:
		case AFUNCDATA:
		case APCDATA:
		case ADUFFCOPY:
		case ADUFFZERO:
		case ANOP:
			break;
		}
	}
}

static void
buildrep(Link *ctxt, int x, int as)
{
	Opcross *p;
	Optab *e, *s, *o;
	int a1, a2, a3, n;

	if(C_NONE != 0 || C_REG != 1 || x >= C_NCLASS) {
		ctxt->diag("assumptions fail in buildrep");
		sysfatal("buildrep");
	}

	repop[as] = x;
	p = (opcross + x);
	s = oprange[as].start;
	e = oprange[as].stop;
	for(o = e-1; o >= s; o--) {
		n = o - optab;
		for(a2=0; a2 < 2; a2++) {
			if(a2) {
				if(o->a2 == C_NONE)
					continue;
			} else
				if(o->a2 != C_NONE)
					continue;
			for(a1=0; a1<C_NCLASS; a1++) {
				if(!xcmp[a1][(int)(o->a1)])
					continue;
				for(a3=0; a3<C_NCLASS; a3++)
					if(xcmp[a3][(int)(o->a3)])
						(*p)[a1][a2][a3] = n;
			}
		}
	}
	oprange[as].start = nil;
}

int
compoundv(Link *ctxt, Prog *p)
{
	Optab *o;

	o = oplook(ctxt, p);
	if(o->size != 4)
		return 1;
	return 0;
}

int32
regoffv(Link *ctxt, Addr *a)
{
	ctxt->instoffset = 0;
	aclassv(ctxt, a);
	return ctxt->instoffset;
}

int
isnopv(Prog *p)
{
	if(p->as == ASLL && p->to.type == D_REG && p->to.reg == 0)
		return 1;
	return 0;
}

static void
addaddrreloc(Link *ctxt, LSym *s, int *o1, int *o2)
{
	Reloc *rel;

	rel = addrel(ctxt->cursym);
	rel->off = ctxt->pc;
	rel->siz = 8;
	rel->sym = s;
	rel->add = ((uvlong)*o1<<32) | (uvlong)*o2;
	rel->type = R_ADDRMIPS;
}

static uint16
high16adj(int32 v)
{
	if(v & 0x8000)
		return (v >> 16) + 1;

	return v >> 16;
}

#define	OP_RRR(op,r1,r2,r3)\
	((op)|(((r1)&31L)<<16)|(((r2)&31L)<<21)|(((r3)&31L)<<11))
#define	OP_IRR(op,i,r2,r3)\
	((op)|((i)&0xffffL)|(((r2)&31L)<<21)|(((r3)&31L)<<16))
#define	OP_SRR(op,s,r2,r3)\
	((op)|(((s)&31L)<<6)|(((r2)&31L)<<16)|(((r3)&31L)<<11))
#define	OP_FRRR(op,r1,r2,r3)\
	((op)|(((r1)&31L)<<16)|(((r2)&31L)<<11)|(((r3)&31L)<<6))
#define	OP_JMP(op,i)\
		((op)|((i)&0x3ffffffL))

#define	OP(x,y)\
	(((x)<<3)|((y)<<0))
#define	SP(x,y)\
	(((x)<<29)|((y)<<26))
#define	BCOND(x,y)\
	(((x)<<19)|((y)<<16))
#define	MMU(x,y)\
	(SP(2,0)|(16<<21)|((x)<<3)|((y)<<0))
#define	FPF(x,y)\
	(SP(2,1)|(16<<21)|((x)<<3)|((y)<<0))
#define	FPD(x,y)\
	(SP(2,1)|(17<<21)|((x)<<3)|((y)<<0))
#define	FPW(x,y)\
	(SP(2,1)|(20<<21)|((x)<<3)|((y)<<0))

static void
asmout(Link *ctxt, Prog *p, Optab *o, int32 *out)
{
	int32 o1, o2, o3, o4, o5, v;
	int r, a;
	Reloc *rel;

	o1 = o2 = o3 = o4 = 0;
	switch(o->type) {
	default:
		ctxt->diag("unknown type %d", o->type);
		prasm(p);
		break;
	case 0:
		break;
	case 1:
		o1 = OP_RRR(oprrr(ctxt, AOR), p->from.reg, REGZERO, p->to.reg);
		break;
	case 2:
		r = p->reg;
		if(r == NREG)
			r = p->to.reg;
		o1 = OP_RRR(oprrr(ctxt, p->as), p->from.reg, r, p->to.reg);
		break;
	case 3:
		v = regoffv(ctxt, &p->from);
		r = p->from.reg;
		if(r == NREG)
			r = o->param;
		a = AADDU;
		if(o->a1 == C_ANDCON)
			a = AOR;
		o1 = OP_IRR(opirr(ctxt, a), v, r, p->to.reg);
		break;
	case 4:
		v = regoffv(ctxt, &p->from);
		r = p->reg;
		if(r == NREG)
			r = p->to.reg;
		o1 = OP_IRR(opirr(ctxt, p->as), v, r, p->to.reg);
		break;
	case 5:
		o1 = oprrr(ctxt, p->as);
		break;
	case 6:
		if(p->pcond == nil)
			v = -4 >> 2;
		else
			v = (p->pcond->pc - p->pc - 4) >> 2;
		if(((v<<16)>>16) != v)
			ctxt->diag("short branch too far: %ld\n%P", v, p);
		if(p->reg == NREG)
			p->reg = 0;
		o1 = OP_IRR(opirr(ctxt, p->as), v, p->from.reg, p->reg);
		break;
	case 7:
		r = p->to.reg;
		if(r == NREG)
			r = o->param;
		v = regoffv(ctxt, &p->to);
		o1 = OP_IRR(opirr(ctxt, p->as), v, r, p->from.reg);
		break;
	case 8:
		r = p->from.reg;
		if(r == NREG)
			r = o->param;
		v = regoffv(ctxt, &p->from);
		o1 = OP_IRR(opirr(ctxt, p->as+ALAST), v, r, p->to.reg);
		break;
	case 9:
		r = p->reg;
		if(r == NREG)
			r = p->to.reg;
		if(isnopv(p)) {
			o1 = 0;
			break;
		}
		o1 = OP_RRR(oprrr(ctxt, p->as), r, p->from.reg, p->to.reg);
		break;
	case 10:
		v = regoffv(ctxt, &p->from);
		r = AOR;
		if(v<0)
			r = AADDU;
		o1 = OP_IRR(opirr(ctxt, r), v, 0, REGTMP);
		r = p->reg;
		if(r == NREG)
			r = p->to.reg;
		o2 = OP_RRR(oprrr(ctxt, p->as), REGTMP, r, p->to.reg);
		break;
	case 11:
		v = 0;
		if(p->pcond != nil)
			v = p->pcond->pc;
		if((v & 0xf0000000ul) != 0)
			ctxt->diag("branch too far: %x\n%P", v, p);

		rel = addrel(ctxt->cursym);
		rel->off = ctxt->pc;
		rel->siz = 4;
		rel->type = R_CALLMIPS;
		if(p->to.sym == nil)
			rel->sym = ctxt->cursym;
		else {
			rel->sym = p->to.sym;
			v += p->to.offset;
		}
		o1 = OP_JMP(opirr(ctxt, p->as), v>>2);
		rel->add = o1;
		break;
	case 12:
		if(p->as == AMOVB)
			o1 = OP_SRR((31<<26)|32, 16, p->from.reg, p->to.reg);
		else
			o1 = OP_SRR((31<<26)|32, 24, p->from.reg, p->to.reg);
		break;
	case 13:
		if(p->as == AMOVBU)
			o1 = OP_IRR(opirr(ctxt, AAND), 0xffL, p->from.reg, p->to.reg);
		else
			o1 = OP_IRR(opirr(ctxt, AAND), 0xffffL, p->from.reg, p->to.reg);
		break;
	case 16:
		v = regoffv(ctxt, &p->from);
		r = p->reg;
		if(r == NREG)
			r = p->to.reg;
		if(isnopv(p)) {
			o1 = 0;
			break;
		}
		o1 = OP_SRR(opirr(ctxt, p->as), v&0x1f, r, p->to.reg);
		break;
	case 18:
		r = p->reg;
		if(r == NREG)
			r = o->param;
		o1 = OP_RRR(oprrr(ctxt, p->as), 0, p->to.reg, r);
		if(p->as == AJAL) {
			rel = addrel(ctxt->cursym);
			rel->off = ctxt->pc;
			rel->siz = 0;
			rel->type = R_CALLIND;
		}
		break;
	case 19:
		v = regoffv(ctxt, &p->from);
		o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, p->to.reg);
		o2 = OP_IRR(opirr(ctxt, AADDU), v&0xffffL, p->to.reg, p->to.reg);
		if(p->from.sym != nil)
			addaddrreloc(ctxt, p->from.sym, &o1, &o2);
		break;
	case 20:
		r = OP(2, 0);
		if(p->from.type == D_LO)
			r = OP(2, 2);
		o1 = OP_RRR(r, 0, 0, p->to.reg);
		break;
	case 21:
		r = OP(2, 1);
		if(p->to.type == D_LO)
			r = OP(2, 3);
		o1 = OP_RRR(r, 0, p->from.reg, 0);
		break;
	case 22:
		o1 = OP_RRR(oprrr(ctxt, p->as), p->from.reg, p->reg, 0);
		break;
	case 23:
		v = regoffv(ctxt, &p->from);
		if(p->to.reg == REGTMP || p->reg == REGTMP)
			ctxt->diag("can't synthesize large constant\n%P", p);
		o1 = OP_IRR(opirr(ctxt, ALAST), v>>16, 0, REGTMP);
		o2 = OP_IRR(opirr(ctxt, AOR), v, REGTMP, REGTMP);
		r = p->reg;
		if(r == NREG)
			r = p->to.reg;
		o3 = OP_RRR(oprrr(ctxt, p->as), REGTMP, r, p->to.reg);
		if(p->from.sym != nil)
			ctxt->diag("%P is not supported", p);
		break;
	case 24:
		v = regoffv(ctxt, &p->from);
		o1 = OP_IRR(opirr(ctxt, ALAST), v>>16, 0, p->to.reg);
		break;
	case 25:
		v = regoffv(ctxt, &p->from);
		o1 = OP_IRR(opirr(ctxt, ALAST), v>>16, 0, REGTMP);
		r = p->reg;
		if(r == NREG)
			r = p->to.reg;
		o2 = OP_RRR(oprrr(ctxt, p->as), REGTMP, r, p->to.reg);
		break;
	case 26:
		v = regoffv(ctxt, &p->from);
		if(p->to.reg == REGTMP)
			ctxt->diag("can't synyhesize large constant\n%P", p);
		o1 = OP_IRR(opirr(ctxt, ALAST), v>>16, 0, REGTMP);
		o2 = OP_IRR(opirr(ctxt, AOR), v, REGTMP, REGTMP);
		r = p->from.reg;
		if(r == NREG)
			r = o->param;
		o3 = OP_RRR(oprrr(ctxt, AADDU), REGTMP, r, p->to.reg);
		break;
	case 27:
		r = p->from.reg;
		if(r == NREG)
			r = o->param;
		v = regoffv(ctxt, &p->from);
		switch(o->size) {
		case 4:
			o1 = OP_IRR(opirr(ctxt, AMOVF+ALAST), v, r, p->to.reg);
			break;
		case 8:
			if(ctxt->arch->endian == BigEndian) {
				o1 = OP_IRR(opirr(ctxt, AMOVF+ALAST), v, r, p->to.reg);
				o2 = OP_IRR(opirr(ctxt, AMOVF+ALAST), v+4, r, p->to.reg+1);
			} else {
				o1 = OP_IRR(opirr(ctxt, AMOVF+ALAST), v+4, r, p->to.reg);
				o2 = OP_IRR(opirr(ctxt, AMOVF+ALAST), v, r, p->to.reg+1);
			}
			break;
		case 12:
			o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
			o2 = OP_IRR(opirr(ctxt, AADDU), v, REGTMP, REGTMP);
			o3 = OP_IRR(opirr(ctxt, AMOVF+ALAST), 0, REGTMP, p->to.reg);
			break;
		case 16:
			o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
			o2 = OP_IRR(opirr(ctxt, AADDU), v, REGTMP, REGTMP);
			if(p->as != AMOVD) {
				o3 = OP_RRR(oprrr(ctxt, AADDU), r, REGTMP, REGTMP);
				o4 = OP_IRR(opirr(ctxt, AMOVF+ALAST), 0, REGTMP, p->to.reg);
			} else {
				if(ctxt->arch->endian == BigEndian) {
					o3 = OP_IRR(opirr(ctxt, AMOVF+ALAST), 0, REGTMP, p->to.reg);
					o4 = OP_IRR(opirr(ctxt, AMOVF+ALAST), 4, REGTMP, p->to.reg+1);
				} else {
					o3 = OP_IRR(opirr(ctxt, AMOVF+ALAST), 4, REGTMP, p->to.reg);
					o4 = OP_IRR(opirr(ctxt, AMOVF+ALAST), 0, REGTMP, p->to.reg+1);
				}
			}
			break;
		case 20:
			o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
			o2 = OP_IRR(opirr(ctxt, AADDU), v, REGTMP, REGTMP);
			o3 = OP_RRR(oprrr(ctxt, AADDU), r, REGTMP, REGTMP);
			if(ctxt->arch->endian == BigEndian) {
				o4 = OP_IRR(opirr(ctxt, AMOVF+ALAST), 0, REGTMP, p->to.reg);
				o5 = OP_IRR(opirr(ctxt, AMOVF+ALAST), 4, REGTMP, p->to.reg+1);
			} else {
				o4 = OP_IRR(opirr(ctxt, AMOVF+ALAST), 4, REGTMP, p->to.reg);
				o5 = OP_IRR(opirr(ctxt, AMOVF+ALAST), 0, REGTMP, p->to.reg+1);
			}
			break;
		}
		if(p->from.sym != nil)
		if(p->from.name == D_EXTERN || p->from.name == D_STATIC) {
			if(o->size < 12)
				sysfatal("impossible: %d, %P", o->size, p);
			addaddrreloc(ctxt, p->from.sym, &o1, &o2);
		}
		break;
	case 28:
		r = p->to.reg;
		if(r == NREG)
			r = o->param;
		v = regoffv(ctxt, &p->to);
		switch(o->size) {
		case 4:
			o1 = OP_IRR(opirr(ctxt, AMOVF), v, r, p->from.reg);
			break;
		case 8:
			if(ctxt->arch->endian == BigEndian) {
				o1 = OP_IRR(opirr(ctxt, AMOVF), v, r, p->from.reg);
				o2 = OP_IRR(opirr(ctxt, AMOVF), v+4, r, p->from.reg+1);
			} else {
				o1 = OP_IRR(opirr(ctxt, AMOVF), v+4, r, p->from.reg);
				o2 = OP_IRR(opirr(ctxt, AMOVF), v, r, p->from.reg+1);
			}
			break;
		case 12:
			o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
			o2 = OP_IRR(opirr(ctxt, AADDU), v, REGTMP, REGTMP);
			o3 = OP_IRR(opirr(ctxt, AMOVF), 0, REGTMP, p->from.reg);
			break;
		case 16:
			o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
			o2 = OP_IRR(opirr(ctxt, AADDU), v, REGTMP, REGTMP);
			if(p->as != AMOVD) {
				o3 = OP_RRR(oprrr(ctxt, AADDU), r, REGTMP, REGTMP);
				o4 = OP_IRR(opirr(ctxt, AMOVF), 0, REGTMP, p->from.reg);
			} else {
				if(ctxt->arch->endian == BigEndian) {
					o3 = OP_IRR(opirr(ctxt, AMOVF), 0, REGTMP, p->from.reg);
					o4 = OP_IRR(opirr(ctxt, AMOVF), 4, REGTMP, p->from.reg+1);
				} else {
					o3 = OP_IRR(opirr(ctxt, AMOVF), 4, REGTMP, p->from.reg);
					o4 = OP_IRR(opirr(ctxt, AMOVF), 0, REGTMP, p->from.reg+1);
				}
			}
			break;
		case 20:
			o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
			o2 = OP_IRR(opirr(ctxt, AADDU), v, REGTMP, REGTMP);
			o3 = OP_RRR(oprrr(ctxt, AADDU), r, REGTMP, REGTMP);
			if(ctxt->arch->endian == BigEndian) {
				o4 = OP_IRR(opirr(ctxt, AMOVF), 0, REGTMP, p->from.reg);
				o5 = OP_IRR(opirr(ctxt, AMOVF), 4, REGTMP, p->from.reg+1);
			} else {
				o4 = OP_IRR(opirr(ctxt, AMOVF), 4, REGTMP, p->from.reg);
				o5 = OP_IRR(opirr(ctxt, AMOVF), 0, REGTMP, p->from.reg+1);
			}
			break;
		}
		if(p->to.sym != nil)
		if(p->to.name == D_EXTERN || p->to.name == D_STATIC) {
			if(o->size < 12)
				sysfatal("impossible: %d, %P", o->size, p);
			addaddrreloc(ctxt, p->to.sym, &o1, &o2);
		}
		break;
	case 30:
		r = SP(2,1)|(4<<21);
		o1 = OP_RRR(r, p->from.reg, 0, p->to.reg);
		break;
	case 31:
		r = SP(2,1)|(0<<21);
		o1 = OP_RRR(r, p->to.reg, 0, p->from.reg);
		break;
	case 32:
		r = p->reg;
		if(r == NREG)
			o1 = OP_FRRR(oprrr(ctxt, p->as), p->from.reg, p->to.reg, p->to.reg);
		else
			o1 = OP_FRRR(oprrr(ctxt, p->as), p->from.reg, r, p->to.reg);
		break;
	case 33:
		o1 = OP_FRRR(oprrr(ctxt, p->as), 0, p->from.reg, p->to.reg);
		break;
	case 34:
		v = regoffv(ctxt, &p->from);
		r = AADDU;
		if(o->a1 == C_ANDCON)
			r = AOR;
		o1 = OP_IRR(opirr(ctxt, r), v, 0, REGTMP);
		o2 = OP_RRR(SP(2,1)|(4<<21), REGTMP, 0, p->to.reg);
		break;
	case 35:
		v = regoffv(ctxt, &p->to);
		r = p->to.reg;
		if(r == NREG)
			r = o->param;
		if(r == REGTMP)
			ctxt->diag("can't synthesize large constant\n%P", p);
		o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
		o2 = OP_IRR(opirr(ctxt, AADDU), v&0xffffL, REGTMP, REGTMP);

		if(p->to.sym != nil)
		if(p->to.name == D_EXTERN || p->to.name == D_STATIC)
			addaddrreloc(ctxt, p->to.sym, &o1, &o2);
		
		o3 = OP_RRR(oprrr(ctxt, AADDU), r, REGTMP, REGTMP);
		o4 = OP_IRR(opirr(ctxt, p->as), 0, REGTMP, p->from.reg);
		if(r == 0) {
			o3 = o4;
			o4 = 0;
		}
		break;
	case 36:
		v = regoffv(ctxt, &p->from);
		r = p->from.reg;
		if(r == NREG)
			r = o->param;
		if(r == REGTMP)
			ctxt->diag("can't synthesize large constant\n%P", p);
		o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
		o2 = OP_IRR(opirr(ctxt, AADDU), v&0xffffL, REGTMP, REGTMP);

		if(p->from.sym != nil)
		if(p->from.name == D_EXTERN || p->from.name == D_STATIC)
			addaddrreloc(ctxt, p->from.sym, &o1, &o2);

		o3 = OP_RRR(oprrr(ctxt, AADDU), r, REGTMP, REGTMP);
		o4 = OP_IRR(opirr(ctxt, p->as+ALAST), 0, REGTMP, p->to.reg);
		if(r == 0) {
			o3 = o4;
			o4 = 0;
		}
		break;
	case 37:
		r = SP(2,0) | (4<<21);
		o1 = OP_RRR(r, p->from.reg, 0, p->to.reg);
		break;
	case 38:
		r = SP(2,0) | (0<<21);
		o1 = OP_RRR(r, p->to.reg, 0, p->from.reg);
		break;
	case 40:
		o1 = regoffv(ctxt, &p->to);
		if(p->to.sym != nil) {
			rel = addrel(ctxt->cursym);
			rel->off = ctxt->pc;
			rel->siz = 4;
			rel->sym = p->to.sym;
			rel->add = p->from.offset;
			rel->type = R_ADDR;
			o1 = 0;
		}
		break;
	case 41:
		o1 = OP_RRR(SP(2,1)|(2<<21), 0, 0, p->to.reg);
		o2 = OP_RRR(SP(2,1)|(6<<21), p->from.reg, 0, p->to.reg);
		break;
	case 42:
		o1 = OP_RRR(SP(2,1)|(2<<21), p->to.reg, 0, p->from.reg);
		break;
	case 47:
		r = p->reg;
		if(r == NREG)
			r = p->to.reg;
		o1 = OP_RRR(SP(3,4)|2, r, p->from.reg, p->to.reg);
		break;
	case 48:
		r = p->reg;
		if(r == NREG)
			r = p->from.reg;
		o1 = OP_RRR(oprrr(ctxt, p->as), r, p->from.reg, p->to.reg);
		break;
	case 49:
		o1 = OP_RRR(oprrr(ctxt, p->as), p->from.reg, 0, p->to.reg);
		break;
	case 50:
		o1 = OP_RRR(oprrr(ctxt, p->as), p->from.reg, 0, p->to.reg);
		break;
	case 51:
		r = p->to.reg;
		if(r == NREG)
			r = o->param;
		v = regoffv(ctxt, &p->to);
		o1 = OP_IRR(SP(6,3), v, r, regoffv(ctxt, &p->from));
		break;
	case 52:
		v = regoffv(ctxt, &p->to);
		o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
		o2 = OP_IRR(SP(6,3), v, REGTMP, regoffv(ctxt, &p->from));

		if(p->to.sym != nil)
		if(p->to.name == D_EXTERN || p->to.name == D_STATIC)
			addaddrreloc(ctxt, p->to.sym, &o1, &o2);

		break;
	case 53:
		r = p->to.reg;
		if(r == NREG)
			r = o->param;
		if(r == REGTMP || p->from.reg == REGTMP)
			ctxt->diag("can't synthesize large constant\n%P", p);
		v = regoffv(ctxt, &p->to);
		o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
		o2 = OP_IRR(opirr(ctxt, AADDU), v, REGTMP, REGTMP);
		o3 = OP_RRR(oprrr(ctxt, AADDU), r, REGTMP, REGTMP);
		o4 = OP_IRR(SP(6,3), 0, REGTMP, regoffv(ctxt, &p->from));

		if(p->to.sym != nil)
		if(p->to.name == D_EXTERN || p->to.name == D_STATIC)
			addaddrreloc(ctxt, p->to.sym, &o1, &o2);
		break;
	case 54:
		r = p->from.reg;
		if(r == NREG)
			r = o->param;
		v = regoffv(ctxt, &p->from);
		o1 = OP_IRR(opirr(ctxt, ALL), v, r, p->to.reg);
		break;
	case 55:
		if(p->to.reg == REGTMP)
			ctxt->diag("can't synthesize large constant\n%P", p);
		v = regoffv(ctxt, &p->from);
		o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
		o2 = OP_IRR(opirr(ctxt, ALL), v, REGTMP, p->to.reg);

		if(p->from.sym != nil)
		if(p->from.name == D_EXTERN || p->from.name == D_STATIC)
			addaddrreloc(ctxt, p->from.sym, &o1, &o2);
		break;
	case 56:
		r = p->from.reg;
		if(r == NREG)
			r = o->param;
		if(r == REGTMP || p->to.reg == REGTMP)
			ctxt->diag("can't synthesize large constant\n%P", p);
		v = regoffv(ctxt, &p->from);
		o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
		o2 = OP_IRR(opirr(ctxt, AADDU), v, REGTMP, REGTMP);
		o3 = OP_RRR(oprrr(ctxt, AADDU), r, REGTMP, REGTMP);
		o4 = OP_IRR(opirr(ctxt, ALL), 0, REGTMP, p->to.reg);

		if(p->from.sym != nil)
		if(p->from.name == D_EXTERN || p->from.name == D_STATIC)
			addaddrreloc(ctxt, p->from.sym, &o1, &o2);
		break;
	case 57:
		r = p->to.reg;
		if(r == NREG)
			r = o->param;
		v = regoffv(ctxt, &p->to);
		o1 = OP_IRR(opirr(ctxt, ASC), v, r, p->from.reg);
		break;
	case 58:
		if(p->from.reg == REGTMP)
			ctxt->diag("can't synthesize large constant\n%P", p);
		v = regoffv(ctxt, &p->to);
		o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
		o2 = OP_IRR(opirr(ctxt, ASC), v, REGTMP, p->from.reg);

		if(p->to.sym != nil)
		if(p->to.name == D_EXTERN || p->to.name == D_STATIC)
			addaddrreloc(ctxt, p->to.sym, &o1, &o2);
		break;
	case 59:
		r = p->to.reg;
		if(r == NREG)
			r = o->param;
		if(r == REGTMP || p->from.reg == REGTMP)
			ctxt->diag("can't synthesize large constant\n%P", p);
		v = regoffv(ctxt, &p->to);
		o1 = OP_IRR(opirr(ctxt, ALAST), high16adj(v), 0, REGTMP);
		o2 = OP_IRR(opirr(ctxt, AADDU), v, REGTMP, REGTMP);
		o3 = OP_RRR(oprrr(ctxt, AADDU), r, REGTMP, REGTMP);
		o4 = OP_IRR(opirr(ctxt, ASC), 0, REGTMP, p->from.reg);

		if(p->to.sym != nil)
		if(p->to.name == D_EXTERN || p->to.name == D_STATIC)
			addaddrreloc(ctxt, p->to.sym, &o1, &o2);
		break;
	case 70:
		v = regoffv(ctxt, &p->from);
		o1 = OP(1,5)|((v&1023)<<16);
		break;
	case 80:
		o1 = OP(1,5)|(10<<16);
		break;
	}
	out[0] = o1;
	out[1] = o2;
	out[2] = o3;
	out[3] = o4;
	out[4] = o5;
	return;
}

static int32
oprrr(Link *ctxt, int a)
{
	switch(a) {
	case AADD:	return OP(4,0);
	case AADDU:	return OP(4,1);
	case ASGT:	return OP(5,2);
	case ASGTU:	return OP(5,3);
	case AAND:	return OP(4,4);
	case AOR:	return OP(4,5);
	case AXOR:	return OP(4,6);
	case ASUB:	return OP(4,2);
	case ASUBU:	return OP(4,3);
	case ANOR:	return OP(4,7);
	case ASLL:	return OP(0,4);
	case ASRL:	return OP(0,6);
	case ASRA:	return OP(0,7);

	case AREM:
	case ADIV:	return OP(3,2);
	case AREMU:
	case ADIVU:	return OP(3,3);
	case AMUL:	return OP(3,0);
	case AMULU:	return OP(3,1);

	case AJMP:	return OP(1,0);
	case AJAL:	return OP(1,1);

	case ASYSCALL:	return OP(1,4);
	case ATLBP:	return MMU(1,0);
	case ATLBR:	return MMU(0,1);
	case ATLBWI:	return MMU(0,2);
	case ATLBWR:	return MMU(0,6);
	case AERET:	return MMU(3,0);

	case ADIVF:	return FPF(0,3);
	case ADIVD:	return FPD(0,3);
	case AMULF:	return FPF(0,2);
	case AMULD:	return FPD(0,2);
	case ASUBF:	return FPF(0,1);
	case ASUBD:	return FPD(0,1);
	case AADDF:	return FPF(0,0);
	case AADDD:	return FPD(0,0);

	case AMOVFW:	return FPF(4,4);
	case AMOVDW:	return FPD(4,4);
	case AMOVWF:	return FPW(4,0);
	case AMOVDF:	return FPD(4,0);
	case AMOVWD:	return FPW(4,1);
	case AMOVFD:	return FPF(4,1);
	case AABSF:	return FPF(0,5);
	case AABSD:	return FPD(0,5);
	case AMOVF:	return FPF(0,6);
	case AMOVD:	return FPD(0,6);
	case ANEGF:	return FPF(0,7);
	case ANEGD:	return FPD(0,7);

	case ACMPEQF:	return FPF(6,2);
	case ACMPEQD:	return FPD(6,2);
	case ACMPGTF:	return FPF(7,4);
	case ACMPGTD:	return FPD(7,4);
	case ACMPGEF:	return FPF(7,6);
	case ACMPGED:	return FPD(7,6);


	case AMOVZ:	return 10;
	case AMOVN:	return 11;
	case ACLZ:	return SP(3,4)|OP(4,0);
	case ACLO:	return SP(3,4)|OP(4,1);
	case ASEB:	return SP(3,7)|(16<<6)|OP(4,0);
	case ASEH:	return SP(3,7)|(24<<6)|OP(4,0);
	case AWSBH:	return SP(3,7)|(2<<6)|OP(4,0);
	}
	ctxt->diag("bad rrr %d", a);
	return 0;
}

static int32
opirr(Link *ctxt, int a)
{
	switch(a) {
	case AADD:	return SP(1,0);
	case AADDU:	return SP(1,1);
	case ASGT:	return SP(1,2);
	case ASGTU:	return SP(1,3);
	case AAND:	return SP(1,4);
	case AOR:	return SP(1,5);
	case AXOR:	return SP(1,6);
	case ALAST:	return SP(1,7);
	case ASLL:	return OP(0,0);
	case ASRL:	return OP(0,2);
	case ASRA:	return OP(0,3);

	case AJMP:	return SP(0,2);
	case AJAL:	return SP(0,3);
	case ADUFFCOPY:	return SP(0,3);
	case ADUFFZERO:	return SP(0,3);
	case ABEQ:	return SP(0,4);
	case ABNE:	return SP(0,5);

	case ABGEZ:	return SP(0,1)|BCOND(0,1);
	case ABGEZAL:	return SP(0,1)|BCOND(2,1);
	case ABGTZ:	return SP(0,7);
	case ABLEZ:	return SP(0,6);
	case ABLTZ:	return SP(0,1)|BCOND(0,0);
	case ABLTZAL:	return SP(0,1)|BCOND(2,0);

	case ABFPT:	return SP(2,1)|(257<<16);
	case ABFPF:	return SP(2,1)|(256<<16);

	case AMOVB:
	case AMOVBU:	return SP(5,0);
	case AMOVH:
	case AMOVHU:	return SP(5,1);
	case AMOVW:	return SP(5,3);
	case AMOVF:	return SP(7,1);
	case AMOVD:	return SP(7,5);
	case AMOVWL:	return SP(5,2);
	case AMOVWR:	return SP(5,6);


	case AMOVWL+ALAST:	return SP(4,2);
	case AMOVWR+ALAST:	return SP(4,6);
	case AMOVB+ALAST:	return SP(4,0);
	case AMOVBU+ALAST:	return SP(4,4);
	case AMOVH+ALAST:	return SP(4,1);
	case AMOVHU+ALAST:	return SP(4,5);
	case AMOVW+ALAST:	return SP(4,3);
	case AMOVF+ALAST:	return SP(6,1);
	case AMOVD+ALAST:	return SP(6,5);



	case ALL:		return SP(6,0);
	case ASC:		return SP(7,0);
	}
	ctxt->diag("bad irr %d", a);
	return 0;
}

