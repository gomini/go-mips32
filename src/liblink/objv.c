#include <u.h>
#include <libc.h>
#include <bio.h>
#include <link.h>
#include "../cmd/vl/v.out.h"
#include "../runtime/stack.h"

enum {
	simfp = 1,
	schedinst = 1,
};

static int	isfpinst(Prog *p, int32 nopok);

static Prog zprg = {
	.as	= AGOK,
	.reg	= NREG,
	.from	= {
		.name	= D_NONE,
		.type	= D_NONE,
		.reg	= NREG,
	},
	.to	= {
		.name	= D_NONE,
		.type	= D_NONE,
		.reg	= NREG,
	},
};

static int
symtype(Addr *a)
{
	return a->name;
}

static int
isdata(Prog *p)
{
	return p->as == ADATA || p->as == AGLOBL;
}

static int
iscall(Prog *p)
{
	return p->as == AJAL;
}

static int
datasize(Prog *p)
{
	return p->reg;
}

static int
textflag(Prog *p)
{
	return p->reg;
}

static void
settextflag(Prog *p, int f)
{
	p->reg = f;
}

static void
progedit(Link *ctxt, Prog *p)
{
	char literal[64];
	LSym *s;

	switch(p->as) {
	case AJMP:
	case AJAL:
	case ADUFFCOPY:
	case ADUFFZERO:
		if(p->to.type == D_OREG && (p->to.name == D_EXTERN || p->to.name == D_STATIC) && p->to.sym != nil)
			p->to.type = D_BRANCH;
		break;
	}

	switch(p->as) {
	case AMOVF:
		if(p->from.type == D_FCONST) {
			int32 i32;
			float32 f32;
			f32 = p->from.u.dval;
			memmove(&i32, &f32, 4);
			sprint(literal, "$f32.%08ux", (uint32)i32);
			s = linklookup(ctxt, literal, 0);
			s->size = 4;
			if(s->type == 0) {
				s->type = SRODATA;
				adduint32(ctxt, s, i32);
				s->reachable = 0;
			}
			p->from.type = D_OREG;
			p->from.sym = s;
			p->from.name = D_EXTERN;
			p->from.offset = 0;
		}
		break;
	case AMOVD:
		if(p->from.type == D_FCONST) {
			int64 i64;
			memmove(&i64, &p->from.u.dval, 8);
			sprint(literal, "$f64.%016llux", (uvlong)i64);
			s = linklookup(ctxt, literal, 0);
			s->size = 8;
			if(s->type == 0) {
				s->type = SRODATA;
				adduint64(ctxt, s, i64);
				s->reachable = 0;
			}
			p->from.type = D_OREG;
			p->from.sym = s;
			p->from.name = D_EXTERN;
			p->from.offset = 0;
		}
		break;
	}

	switch(p->as) {
	case ASUB:
		if(p->from.type == D_CONST) {
			p->from.offset = -p->from.offset;
			p->as = AADD;
		}
		break;
	case ASUBU:
		if(p->from.type == D_CONST) {
			p->from.offset = -p->from.offset;
			p->as = AADDU;
		}
		break;
	}
}

static Prog*
prg(void)
{
	Prog *p;
	p = emallocz(sizeof(*p));
	*p = zprg;
	return p;
}

static	Prog*	stacksplit(Link*, LSym*, Prog*, int32, int);
static	void	softfloat(Link*, LSym*);
static	void	xfol(Link*, Prog*, Prog**);

void
addnopv(Prog *p)
{
	Prog *q;

	q = prg();
	q->as = ASLL;
	q->lineno = p->lineno;
	q->from.type = D_CONST;
	q->from.offset = 0;
	q->to.type = D_REG;
	q->to.reg = 0;
	q->reg = 0;

	q->link = p->link;
	p->link = q;
}

static void
nocache(Prog *p)
{
	p->optab = 0;
	p->from.class = 0;
	p->to.class = 0;
}
static void
wrapper(Link *ctxt, Prog *p, int autosize)
{
	Prog *p1, *p2;

	p = appendp(ctxt, p);
	p->as = AMOVW;
	p->from.type = D_OREG;
	p->from.reg = REGG;
	p->from.offset = 4 * ctxt->arch->ptrsize;
	p->to.type = D_REG;
	p->to.reg = 1;

	p = appendp(ctxt, p);
	p->as = ABEQ;
	p->from.type = D_REG;
	p->from.reg = 1;
	p->reg = 0;
	p->to.type = D_BRANCH;
	p->mark |= BRANCH;
	p1 = p;

	p = appendp(ctxt, p);
	p->as = AMOVW;
	p->from.type = D_OREG;
	p->from.reg = 1;
	p->from.offset = 0;
	p->to.type = D_REG;
	p->to.reg = 2;

	p = appendp(ctxt, p);
	p->as = AADDU;
	p->from.type = D_CONST;
	p->from.offset = autosize + 4;
	p->reg = REGSP;
	p->to.type = D_REG;
	p->to.reg = 3;

	p = appendp(ctxt, p);
	p->as = ABNE;
	p->from.type = D_REG;
	p->from.reg = 2;
	p->reg = 3;
	p->to.type = D_BRANCH;
	p->mark |= BRANCH;
	p2 = p;

	p = appendp(ctxt, p);
	p->as = AADDU;
	p->from.type = D_CONST;
	p->from.offset = 4;
	p->reg = REGSP;
	p->to.type = D_REG;
	p->to.reg = 4;

	p = appendp(ctxt, p);
	p->as = AMOVW;
	p->from.type = D_REG;
	p->from.reg = 4;
	p->to.type = D_OREG;
	p->to.reg = 1;
	p->to.offset = 0;

	
	p1->pcond = p->link;
	p2->pcond = p->link;
	p1->link->mark |= LABEL;
	p2->link->mark |= LABEL;
	p->link->mark |= LABEL;
}

static void
fixpcond(Prog *s, Prog *n)
{
	if(s == nil || n == nil || n->link == nil)
		return;
	for(; s != n; s = s->link) {
		if(s->pcond == n->link)
			s->pcond = n;
	}
}
static int
isspadj(Prog *p)
{
	if(p->as == AADDU || p->as == AADD)
	if(p->to.type == D_REG && p->to.reg == REGSP && p->from.type == D_CONST)
		return 1;
	if(p->as == AMOVW && p->to.type == D_REG && p->to.reg == REGSP)
	if(p->from.type == D_CONST && p->from.reg == REGSP)
		return 1;
	return 0;
}

static void
addstacksplit(Link *ctxt, LSym *cursym)
{
	Prog *p, *q, *p1, *q1;
	int c;
	int32 autoffset, autosize;

	prepareoprange(ctxt);

	autosize = 0;

	if(ctxt->symmorestack[0] == nil) {
		ctxt->symmorestack[0] = linklookup(ctxt, "runtime.morestack", 0);
		ctxt->symmorestack[1] = linklookup(ctxt, "runtime.morestack_noctxt", 0);
	}

	ctxt->cursym = cursym;
	if(cursym->text == nil || cursym->text->link == nil)
		return;

	p = cursym->text;
	autoffset = p->to.offset;
	if(autoffset < 0)
		autoffset = 0;
	cursym->locals = autoffset;
	cursym->args = p->to.offset2;


	q = nil;
	for(p = cursym->text; p != nil; p = p->link) {
		switch(p->as) {
		case ATEXT:
			q = p;
			p->mark |= LABEL | LEAF | SYNC;
			if(p->link)
				p->link->mark |= LABEL;
			break;
		case AMOVW:
			if(p->to.type == D_FCREG || p->to.type == D_MREG) {
				p->mark |= LABEL | SYNC;
				break;
			}
			if(p->from.type == D_FCREG || p->from.type == D_MREG) {
				p->mark |= LABEL | SYNC;
				addnopv(p);
				addnopv(p);
				break;
			}
			break;
		case ASYSCALL:
		case AWORD:
		case ATLBWR:
		case ATLBWI:
		case ATLBP:
		case ATLBR:

		case APREF:
		case ALL:
		case ASC:
		case AUNDEF:
			p->mark |= LABEL | SYNC;
			break;
		case ASLL:
			if(isnopv(p))
				p->mark |= LABEL | SYNC;
			break;
		case ARET:
			if(p->link != nil)
				p->link->mark |= LABEL;
			break;
		case ANOP:
			q1 = p->link;
			q->link = q1;
			q1->mark |= p->mark;
			continue;
		case ABGEZAL:
		case ABLTZAL:
		case AJAL:
		case ADUFFCOPY:
		case ADUFFZERO:
			cursym->text->mark &= ~LEAF;
		case AJMP:
		case ABEQ:
		case ABGEZ:
		case ABGTZ:
		case ABLEZ:
		case ABLTZ:
		case ABNE:
		case ABFPT:
		case ABFPF:
			p->mark |= BRANCH;
			q1 = p->pcond;
			if(q1 != nil) {
				while(q1->as == ANOP) {
					q1 = q1->link;
					p->pcond = q1;
				}
				if(!(q1->mark & LEAF))
					q1->mark |= LABEL;
			} else
				p->mark |= LABEL;
			q1 = p->link;
			if(q1 != nil)
				q1->mark |= LABEL;
			break;
		}
		if(simfp && isfpinst(p, 0))
			cursym->text->mark &= ~LEAF;
		q = p;
	}

	for(p = cursym->text; p != nil; p = p->link) {
		switch(p->as) {
		case ATEXT:
			autosize = p->to.offset + 4;
			if(autosize <= 4)
			if(cursym->text->mark & LEAF) {
				p->to.offset = -4;
				autosize = 0;
			}

			if(!(p->reg & NOSPLIT))
				p = stacksplit(ctxt, cursym, p, autosize, !(cursym->text->reg & NEEDCTXT));

			if(autosize) {
				p = appendp(ctxt, p);
				p->as = AADDU;
				p->from.type = D_CONST;
				p->from.offset = -autosize;
				p->to.type = D_REG;
				p->to.reg = REGSP;

				fixpcond(cursym->text->link, p);
			} else if(!(cursym->text->mark & LEAF)) {
				if(ctxt->debugvlog) {
					Bprint(ctxt->bso, "save suppressed in: %s\n", cursym->name);
					Bflush(ctxt->bso);
				}
				cursym->text->mark |= LEAF;
			}

			if(cursym->text->mark & LEAF) {
				cursym->leaf = 1;
				break;
			}

			p = appendp(ctxt, p);
			p->as = AMOVW;
			p->from.type = D_REG;
			p->from.reg = REGLINK;
			p->to.type = D_OREG;
			p->to.offset = 0;
			p->to.reg = REGSP;



			if(cursym->text->reg & WRAPPER) {
				wrapper(ctxt, p, autosize);
			}
			break;
		case ARET:
			nocache(p);
			if(p->from.type == D_CONST) {
				ctxt->diag("using BECOME (%P) is not supported", p);
				break;
			}
			q = p;

			if(!(cursym->text->mark & LEAF)) {
				p->as = AMOVW;
				p->from.type = D_OREG;
				p->from.offset = 0;
				p->from.reg = REGSP;
				p->to.type = D_REG;
				p->to.reg = REGLINK;
				p->mark |= NOSCHED;
				p = appendp(ctxt, p);
			}

			if(autosize >= BIG) {
				p->as = AADDU;
				p->from.type = D_CONST;
				p->from.offset = autosize;
				p->reg = REGSP;
				p->to.type = D_REG;
				p->to.reg = REGSP;
				p->mark |= NOSCHED;
				p = appendp(ctxt, p);
			}

			p->as = AJMP;
			p->isret = 1;
			p->from = zprg.from;
			if(q->to.sym) {
				p->to.type = D_BRANCH;
				if(p != q) {
					p->to.sym = q->to.sym;
					q->to.sym = nil;
				}
			} else {
				p->to.type = D_OREG;
				p->to.offset = 0;
				p->to.reg = REGLINK;
			}
			p->mark |= BRANCH | NOSCHED;

			p = appendp(ctxt, p);
			if(autosize && autosize < BIG) {
				p->as = AADDU;
				p->from.type = D_CONST;
				p->from.offset = autosize;
				p->reg = REGSP;
				p->to.type = D_REG;
				p->to.reg = REGSP;
				p->mark |= NOSCHED;
			} else {
				p->as = ASLL;
				p->from.type = D_CONST;
				p->from.offset = 0;
				p->reg = 0;
				p->to.type = D_REG;
				p->to.reg = 0;
				p->mark |= NOSCHED;
			}

			break;
		}
	}


	if(schedinst)
		goto resched;

	for(p=cursym->text->link; p; p=p1) {
		p1 = p->link;
		if(p->mark & NOSCHED)
			continue;
		if(p->mark & BRANCH)
			addnopv(p);
	}
	goto spadj;
resched:
	q = nil;
	q1 = cursym->text->link;
	c = 0;
	for(p = cursym->text->link; p != nil; p = p1) {
		p1 = p->link;
		c++;
		if(p->mark & NOSCHED) {
			if(q1 != p) {
				schedv(ctxt, q1, q);
			}
			for(; p != nil; p = p->link) {
				if(!(p->mark & NOSCHED))
					break;
				q = p;
			}
			p1 = p;
			q1 = p;
			c = 0;
			continue;
		}
		if(p->mark & (LABEL |SYNC)) {
			if(q1 != p)
				schedv(ctxt, q1, q);
			q1 = p;
			c = 1;
		}
		if(p->mark & (BRANCH | SYNC)) {
			schedv(ctxt, q1, p);
			q1 = p1;
			c = 0;
		}
		if(c >= NSCHED) {
			schedv(ctxt, q1, p);
			q1 = p1;
			c = 0;
		}
		q = p;
	}

spadj:
	autosize = cursym->text->to.offset + 4;
	if(autosize == 0)
		goto last;
	for(p = cursym->text->link; p != nil; p = p->link) {
		switch(p->as) {
		case AJMP:
			if(!p->isret)
				break;
			q = p->link;
			if(q->link == nil)
				goto last;
			if(isspadj(q)) {
			} else {
				q->spadj = +autosize;
			}
			p = q;
			break;
		case APCDATA:
			if(p->link->as != AJAL) {
				if(p->link->as != ASLL)
					sysfatal("PCDATA got moved %P; %P", p, p->link);
				if(p->link->link == nil || p->link->link->as != AJAL)
					sysfatal("PCDATA got moved %P; %P", p, p->link);
			}
		default:
			if(isspadj(p))
				p->spadj = -p->from.offset;
			break;
		}
	}
last:
	if(simfp)
		softfloat(ctxt, cursym);
}

static int
isfpinst(Prog *p, int32 nopok)
{
	if(nopok && p->as == ASLL)
	if(p->to.type == D_REG && p->to.reg == 0)
		return 1;

	switch(p->as) {
	case AMOVW:
		if(p->to.type == D_FREG || p->from.type == D_FREG)
			return 1;
		if(p->to.type == D_FCREG || p->from.type == D_FCREG)
			return 1;
		return 0;
	case ABFPF:
	case ABFPT:

	case AMOVWD:
	case AMOVWF:
	case AMOVDW:
	case AMOVFW:
	case AMOVFD:
	case AMOVDF:
	case AMOVF:
	case AMOVD:

	case ACMPEQD:
	case ACMPEQF:
	case ACMPGED:
	case ACMPGEF:
	case ACMPGTD:
	case ACMPGTF:

	case AADDF:
	case AADDD:
	case ASUBF:
	case ASUBD:
	case AMULF:
	case AMULD:
	case ADIVF:
	case ADIVD:
	case AABSF:
	case AABSD:
	case ANEGD:
	case ANEGF:
		return 1;
	}
	return 0;
}

static void
softfloat(Link *ctxt, LSym *cursym)
{
	Prog *p, *next, *p1, tp;
	LSym *symsfloat;
	int wasfloat;

	symsfloat = linklookup(ctxt, "_sfloat", 0);
	wasfloat = 0;
	for(p = cursym->text; p != nil; p = p->link)
		p->mark = 0;
	for(p = cursym->text; p != nil; p = p->link)
		if(p->pcond != nil)
			p->pcond->mark |= LABEL;

	for(p = cursym->text; p != nil; p = p->link) {
		switch(p->as) {
		case ABFPF:
		case ABFPT:
			if(p->link == nil)
				sysfatal("no inst in the delay slot %P", p);
			if(isfpinst(p->link, 1))
				goto soft;

			p1 = prg();
			p1->as = ASLL;
			p1->from.type = D_CONST;
			p1->from.offset = 0;
			p1->to.type = D_REG;
			p1->to.reg = 0;
			p1->reg = 0;
			p1->lineno = p->lineno;

			next = p->link;
			p1->link = next->link;

			tp = *next;
			*next = *p;
			*p = tp;

			p->link = next;
			next->link = p1;
			goto notsoft;

		case ABEQ:
		case ABNE:
		case ABGEZ:
		case ABGEZAL:
		case ABGTZ:
		case ABLEZ:
		case ABLTZ:
		case ABLTZAL:
		case AJMP:
		case AJAL:
			if(p->link == nil)
				sysfatal("no inst in the delay slot %P", p);
			if(!isfpinst(p->link, 0))
				goto notsoft;
			p1 = prg();
			*p1 = zprg;
			p1->as = ASLL;
			p1->from.type = D_CONST;
			p1->from.offset = 0;
			p1->to.type = D_REG;
			p1->to.reg = 0;
			p1->reg = 0;

			next = p->link;
			p1->link = next->link;

			tp = *next;
			*next = *p;
			*p = tp;

			p->link = next;
			next->link = p1;
			goto soft;

		default:
			if(isfpinst(p, wasfloat))
				goto soft;
			goto notsoft;
		}
	soft:
		if(!wasfloat || (p->mark & LABEL)) {

			next = prg();
			*next = *p;

			p1 = prg();
			p1->as = ASLL;
			p1->from.type = D_REG;
			p1->from.reg = 0;
			p1->to.type = D_REG;
			p1->to.reg = 0;
			p1->reg = 0;
			p1->lineno = p->lineno;
			p1->link = next;

			*p = zprg;
			p->as = AJAL;
			p->to.type = D_BRANCH;
			p->to.sym = symsfloat;
			p->to.reg = NREG;
			p->lineno = p1->lineno;
			p->link = p1;

			p = next;
			wasfloat = 1;
		}
		continue;
	notsoft:
		wasfloat = 0;
	}
}

static Prog*
stacksplit(Link *ctxt, LSym *cursym, Prog *p, int32 framesize, int noctxt)
{
	Prog *q, *q1;

	q1 = nil;


	p = appendp(ctxt, p);
	p->as = AMOVW;
	p->from.type = D_OREG;
	p->from.reg = REGG;
	p->from.offset = 2*ctxt->arch->ptrsize;
	if(ctxt->cursym->cfunc)
		p->from.offset = 3*ctxt->arch->ptrsize;
	p->to.type = D_REG;
	p->to.reg = 2;

	p = appendp(ctxt, p);
	p->as = AMOVW;
	p->from.type = D_REG;
	p->from.reg = REGLINK;
	p->to.type = D_REG;
	p->to.reg = 4;

	p = appendp(ctxt, p);
	p->as = AMOVW;
	p->to.type = D_REG;
	p->to.reg = 5;
	p->from.type = D_CONST;
	p->from.name = D_EXTERN;
	p->from.sym = cursym;




	if(framesize <= StackSmall) {
		p = appendp(ctxt, p);
		p->as = ASGTU;
		p->from.type = D_REG;
		p->from.reg = 2;
		p->reg = REGSP;
		p->to.type = D_REG;
		p->to.reg = 8;
	} else if(framesize <= StackBig) {
		p = appendp(ctxt, p);
		p->as = AADDU;
		p->from.type = D_CONST;
		p->from.offset = -framesize;
		p->reg = REGSP;
		p->to.type = D_REG;
		p->to.reg = 8;

		p = appendp(ctxt, p);
		p->as = ASGTU;
		p->from.type = D_REG;
		p->from.reg = 2;
		p->reg = 8;
		p->to.type = D_REG;
		p->to.reg = 8;
	} else {
		p = appendp(ctxt, p);
		p->as = AMOVW;
		p->from.type = D_CONST;
		p->from.offset = StackPreempt;
		p->to.type = D_REG;
		p->to.reg = 8;

		p = appendp(ctxt, p);
		p->as = ABEQ;
		p->from.type = D_REG;
		p->from.reg = 2;
		p->reg = 8;
		p->to.type = D_BRANCH;

		p->mark |= BRANCH;
		q1 = p;

		p = appendp(ctxt, p);
		p->as = AADDU;
		p->from.type = D_CONST;
		p->from.offset = StackGuard;
		p->reg = REGSP;
		p->to.type = D_REG;
		p->to.reg = 8;

		p->mark |= LABEL;

		p = appendp(ctxt, p);
		p->as = ASUBU;
		p->from.type = D_REG;
		p->from.reg = 2;
		p->reg = 8;
		p->to.type = D_REG;
		p->to.reg = 8;

		p = appendp(ctxt, p);
		p->as = ASGTU;
		p->from.type = D_CONST;
		p->from.offset = framesize + StackGuard - StackSmall;
		p->reg = 8;
		p->to.type = D_REG;
		p->to.reg = 8;
	}

	p = appendp(ctxt, p);
	p->as = ABEQ;
	p->from.type = D_REG;
	p->from.reg = 8;
	p->reg = 0;
	p->to.type = D_BRANCH;
	p->mark |= BRANCH;

	q = p;

	p = appendp(ctxt, p);
	p->as = AJAL;
	p->to.type = D_BRANCH;
	p->mark |= BRANCH | LABEL;
	if(ctxt->cursym->cfunc)
		p->to.sym = linklookup(ctxt, "runtime.morestackc", 0);
	else
		p->to.sym = ctxt->symmorestack[noctxt];

	if(q1)
		q1->pcond = p;


	q->pcond = p->link;
	if(!(p->link->mark & LEAF))
		p->link->mark |= LABEL;

	return p;
}

static void
follow(Link *ctxt, LSym *s)
{
	Prog *firstp, *lastp;

	ctxt->cursym = s;

	firstp = prg();
	lastp = firstp;
	xfol(ctxt, s->text, &lastp);
	lastp->link = nil;
	s->text = firstp->link;
}

static void
xfol(Link *ctxt, Prog *p, Prog **lastp)
{
	Prog *q, *r;
	int a, i;

loop:
	if(p == nil)
		return;
	a = p->as;
	if(a == AJMP) {
		q = p->pcond;
		if((p->mark & NOSCHED) || q && (q->mark & NOSCHED)) {
			p->mark |= FOLL;
			(*lastp)->link = p;
			*lastp = p;
			p = p->link;
			xfol(ctxt, p, lastp);
			p = q;
			if(p && !(p->mark & FOLL))
				goto loop;
			return;
		}
		if(q != nil && q->as != ATEXT) {
			p->mark |= FOLL;
			p = q;
			if(!(p->mark & FOLL))
				goto loop;
		}
	}
	if(p->mark & FOLL) {
		for(i=0, q = p; i<4; i++, q = q->link) {
			if(q == *lastp || (q->mark & NOSCHED))
				break;
			a = q->as;
			if(a == ANOP) {
				i--;
				continue;
			}
			if(a == AJMP || a == ARET || a == AERET || a == AUNDEF)
				goto copy;
			if(q->pcond == nil || (q->pcond->mark & FOLL))
				continue;
			if(a != ABEQ && a != ABNE)
				continue;
		copy:
			for(;;) {
				r = prg();
				*r = *p;
				if(!(r->mark & FOLL))
					print("can't happen 1\n");
				r->mark |= FOLL;
				if(p != q) {
					p = p->link;
					(*lastp)->link = r;
					*lastp = r;
					continue;
				}
				(*lastp)->link = r;
				*lastp = r;
				if(a == AJMP || a == ARET || a == AERET || a == AUNDEF)
					return;
				r->as = ABNE;
				if(a == ABNE)
					r->as = ABEQ;
				r->pcond = p->link;
				r->link = p->pcond;
				if(!(r->link->mark & FOLL))
					xfol(ctxt, r->link, lastp);
				if(!(r->pcond->mark & FOLL))
					print("can't happen 2\n");
				return;
			}
		}
		a = AJMP;
		q = prg();
		q->as = a;
		q->lineno = p->lineno;
		q->to.type = D_BRANCH;
		q->to.offset = p->pc;
		q->pcond = p;
		p = q;
	}
	p->mark |= FOLL;
	(*lastp)->link = p;
	*lastp = p;
	if(a == AJMP || a == ARET || a == AERET || a == AUNDEF) {
		if(p->mark & NOSCHED) {
			p = p->link;
			goto loop;
		}
		return;
	}
	if(p->pcond != nil)
	if(a != AJAL && p->link != nil) {
		xfol(ctxt, p->link, lastp);
		p = p->pcond;
		if(p == nil || (p->mark & FOLL))
			return;
		goto loop;
	}
	p = p->link;
	goto loop;
}

LinkArch linkmips32 = {
	.name		= "mips32",
	.thechar	= 'v',
	.endian		= BigEndian,

	.addstacksplit	= addstacksplit,
	.assemble	= spanv,
	.datasize	= datasize,
	.follow		= follow,
	.iscall		= iscall,
	.isdata		= isdata,
	.prg		= prg,
	.progedit	= progedit,
	.settextflag	= settextflag,
	.symtype	= symtype,
	.textflag	= textflag,

	.minlc		= 4,
	.ptrsize	= 4,
	.regsize	= 4,

	.D_ADDR		= D_ADDR,
	.D_AUTO		= D_AUTO,
	.D_BRANCH	= D_BRANCH,
	.D_CONST	= D_CONST,
	.D_EXTERN	= D_EXTERN,
	.D_FCONST	= D_FCONST,
	.D_NONE		= D_NONE,
	.D_PARAM	= D_PARAM,
	.D_SCONST	= D_SCONST,
	.D_STATIC	= D_STATIC,
	.D_OREG		= D_OREG,

	.ACALL		= AJAL,
	.ADATA		= ADATA,
	.AEND		= AEND,
	.AFUNCDATA	= AFUNCDATA,
	.AGLOBL		= AGLOBL,
	.AJMP		= AJMP,
	.ANOP		= ANOP,
	.APCDATA	= APCDATA,
	.ARET		= ARET,
	.ATEXT		= ATEXT,
	.ATYPE		= ATYPE,
	.AUSEFIELD	= AUSEFIELD,
};

LinkArch linkmips32le = {
	.name		= "mips32le",
	.thechar	= 'v',
	.endian		= LittleEndian,

	.addstacksplit	= addstacksplit,
	.assemble	= spanv,
	.datasize	= datasize,
	.follow		= follow,
	.iscall		= iscall,
	.isdata		= isdata,
	.prg		= prg,
	.progedit	= progedit,
	.settextflag	= settextflag,
	.symtype	= symtype,
	.textflag	= textflag,

	.minlc		= 4,
	.ptrsize	= 4,
	.regsize	= 4,

	.D_ADDR		= D_ADDR,
	.D_AUTO		= D_AUTO,
	.D_BRANCH	= D_BRANCH,
	.D_CONST	= D_CONST,
	.D_EXTERN	= D_EXTERN,
	.D_FCONST	= D_FCONST,
	.D_NONE		= D_NONE,
	.D_PARAM	= D_PARAM,
	.D_SCONST	= D_SCONST,
	.D_STATIC	= D_STATIC,
	.D_OREG		= D_OREG,

	.ACALL		= AJAL,
	.ADATA		= ADATA,
	.AEND		= AEND,
	.AFUNCDATA	= AFUNCDATA,
	.AGLOBL		= AGLOBL,
	.AJMP		= AJMP,
	.ANOP		= ANOP,
	.APCDATA	= APCDATA,
	.ARET		= ARET,
	.ATEXT		= ATEXT,
	.ATYPE		= ATYPE,
	.AUSEFIELD	= AUSEFIELD,
};

