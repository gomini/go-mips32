#include <u.h>
#include <libc.h>
#include <bio.h>
#include <link.h>
#include "../cmd/vl/v.out.h"

enum
{
	E_HILO	= 1<<0,
	E_FCR	= 1<<1,
	E_MCR	= 1<<2,
	E_MEM	= 1<<3,
	E_MEMSP	= 1<<4,
	E_MEMSB	= 1<<5,
	ANYMEM	= E_MEM|E_MEMSP|E_MEMSB,
	DELAY	= BRANCH|LOAD|FCMP,
};

typedef struct 	Count	Count;
typedef struct	Nop	Nop;
typedef	struct	Sch	Sch;
typedef	struct	Dep	Dep;

struct	Count
{
	int32	count;
	int32	outof;
};

struct	Nop
{
	Count	branch;
	Count	fcmp;
	Count	load;
	Count	mfrom;
	Count	jump;
};

struct	Dep
{
	uint32	ireg;
	uint32	freg;
	uint32	cc;
};
struct	Sch
{
	Prog	p;
	Dep	set;
	Dep	used;
	int32	soffset;
	int8	size;
	int8	nop;
	int8	comp;
	int8	mfhi;
};

static void	regsused(Link*, Sch*);
static int	depend(Link*, Sch*, Sch*);
static int	conflict(Sch*, Sch*);
static int	offoverlap(Sch*, Sch*);
static void	dumpbits(Link*, Sch*, Dep*);

static Nop	nopv;

void
schedv(Link *ctxt, Prog *p0, Prog *pe)
{
	Prog *p, *q;
	Sch sch[NSCHED], *s, *t, *u, *se, stmp;

	s = sch;
	for(p=p0;; p=p->link) {
		memset(s, 0, sizeof(*s));
		s->p = *p;
		regsused(ctxt, s);
		if(ctxt->debugvlog) {
			Bprint(ctxt->bso, "%P\t\tset", &s->p);
			dumpbits(ctxt, s, &s->set);
			Bprint(ctxt->bso, "; used");
			dumpbits(ctxt, s, &s->used);
			if(s->comp)
				Bprint(ctxt->bso, "; compound");
			if(s->p.mark & LOAD)
				Bprint(ctxt->bso, "; load");
			if(s->p.mark & BRANCH)
				Bprint(ctxt->bso, "; branch");
			if(s->p.mark & FCMP)
				Bprint(ctxt->bso, "; fcmp");
			Bprint(ctxt->bso, "\n");
		}
		if(p == pe)
			break;
		s++;
	}
	se = s;

	for(s=sch; s<=se; s++) {
		if(!(s->p.mark & LOAD))
			continue;
		for(t=s+1; t<=se; t++) {
			if(!(t->p.mark & LOAD))
				continue;
			if(t->p.mark & BRANCH)
				break;
			if(conflict(s, t))
				break;
			for(u=t-1; u>s; u--)
				if(depend(ctxt, u, t))
					goto no11;
			u = s+1;
			stmp = *t;
			memmove(s+2, u, (uint8*)t - (uint8*)u);
			*u = stmp;
			break;
		}
	no11:

		for(t=s+1; t<=se; t++) {
			if(t->p.mark & BRANCH)
				break;
			if(t->p.as == APCDATA)
				break;
			if(s > sch && conflict(s-1, t))
				continue;
			for(u=t-1; u>=s; u--)
				if(depend(ctxt, t, u))
					goto no1;
			stmp = *t;
			memmove(s+1, s, (uint8*)t - (uint8*)s);
			*s = stmp;
			if(!(s->p.mark & LOAD))
				break;
		no1:;
		}
	}

	for(s=se; s>=sch; s--) {
		if(!(s->p.mark & DELAY))
			continue;
		if(s < se)
			if(!conflict(s, s+1))
				goto out3;
		for(t=s-1; t>=sch; t--) {
			if(t->p.as == APCDATA)
				goto no2;
			if(t->comp)
				if(s->p.mark & BRANCH)
					goto no2;
			if(t->p.mark & DELAY)
				if(s >= se || conflict(t, s+1))
					goto no2;
			for(u=t+1; u<=s; u++)
				if(depend(ctxt, u, t))
					goto no2;
			goto out2;
		no2:;
		}
		s->nop = 1;
		if(ctxt->debugvlog) {
			if(s->p.mark & LOAD) {
				nopv.load.count++;
				nopv.load.outof++;
			}
			if(s->p.mark & BRANCH) {
				nopv.branch.count++;
				nopv.branch.outof++;
			}
			if(s->p.mark & FCMP) {
				nopv.fcmp.count++;
				nopv.fcmp.outof++;
			}
		}
		continue;

	out2:
		stmp = *t;
		memmove(t, t+1, (uint8*)s - (uint8*)t);
		*s = stmp;
		s--;

	out3:
		if(ctxt->debugvlog) {
			if(s->p.mark & LOAD)
				nopv.load.outof++;
			if(s->p.mark & BRANCH)
				nopv.branch.outof++;
			if(s->p.mark & FCMP)
				nopv.fcmp.outof++;
		}
	}

	t = sch+1;
	for(s=sch; s<se-1; s++, t++) {
		if((s->used.cc & E_HILO) == 0)
			continue;
		if(t->set.cc & E_HILO) {
			s->nop = 2;
			s->mfhi = 1;
		}
	}

	for(s=sch, p=p0; s<=se; s++, p=q) {
		q = p->link;
		if(q != s->p.link) {
			*p = s->p;
			p->link = q;
		}
		if(s->mfhi || (p->mark & BRANCH)) {
			while(s->nop--)
				addnopv(p);
		}
	}
}

static void
regsused(Link *ctxt, Sch *s)
{
	int c, ar, ad, ld, sz;
	uint32 m;
	Prog *p;

	p = &s->p;
	s->comp = compoundv(ctxt, p);
	s->nop = 0;
	if(s->comp) {
		s->set.ireg |= 1<<REGTMP;
		s->used.ireg |= 1<<REGTMP;
	}

	ar = 0;
	ad = 0;
	ld = 0;
	sz = 20;

	switch(p->as) {
	case ATEXT:
		ad = 1;
		break;
	case AJAL:
		c = p->reg;
		if(c == NREG)
			c = REGLINK;
		s->set.ireg |= 1<<c;
		ar = 1;
		ad = 1;
		break;
	case ADUFFZERO:
		sz = (128 - p->to.offset/4)*4;
		s->used.ireg |= 1<<REGRT1;
		s->set.cc |= ANYMEM;
		s->size = sz;
		s->soffset = p->to.offset;
		ar = 1;
		ad = 1;
		break;
	case ADUFFCOPY:
		sz = (128/4 - p->to.offset/32)*4;
		s->set.cc |= ANYMEM;
		s->set.ireg |= (1<<REGRT1) | (1<<(REGRT1+1)) | (1<<(REGRT1+2)) | (1<<(REGRT1+3));
		s->used.ireg |= s->set.ireg | (1<<(REGRT1+4)) | (1<<(REGRT1+5));
		s->used.cc |= ANYMEM;
		s->size = sz;
		s->soffset = 16*(128/4-sz/4);
		ar = 1;
		ad = 1;
		break;
	case ABGEZAL:
	case ABLTZAL:
		s->set.ireg |= 1<<REGLINK;
	case ABEQ:
	case ABGEZ:
	case ABGTZ:
	case ABLEZ:
	case ABLTZ:
	case ABNE:
		ar = 1;
		ad = 1;
		break;
	case ABFPT:
	case ABFPF:
		ad = 1;
		s->used.cc |= E_FCR;
		break;
	case ACMPEQD:
	case ACMPEQF:
	case ACMPGED:
	case ACMPGEF:
	case ACMPGTD:
	case ACMPGTF:
		ar = 1;
		s->set.cc |= E_FCR;
		p->mark |= FCMP;
		break;
	case AJMP:
		ar = 1;
		ad = 1;
		break;
	case AMOVB:
	case AMOVBU:
		sz = 1;
		ld = 1;
		break;
	case AMOVH:
	case AMOVHU:
		sz = 2;
		ld = 1;
		break;
	case AMOVF:
	case AMOVW:
	case AMOVWL:
	case AMOVWR:
		sz = 4;
		ld = 1;
		break;
	case AMOVD:
		sz = 8;
		ld = 1;
		break;
	case ADIV:
	case ADIVU:
	case AMUL:
	case AMULU:
	case AREM:
	case AREMU:

	case AMAD:
	case AMADU:
	case AMUL32:
	case AMSUB:
	case AMSUBU:
		s->set.cc = E_HILO;
	case AADD:
	case AADDU:
	case AAND:
	case ANOR:
	case AOR:
	case ASGT:
	case ASGTU:
	case ASLL:
	case ASRA:
	case ASRL:
	case ASUB:
	case ASUBU:
	case AXOR:

	case AADDD:
	case AADDF:
	case ASUBD:
	case ASUBF:
	case AMULF:
	case AMULD:
	case ADIVF:
	case ADIVD:
	
	case ASEB:
	case ASEH:
	case AWSBH:
		if(p->reg == NREG) {
			if(p->to.type == D_REG || p->to.type == D_FREG)
				p->reg = p->to.reg;
			if(p->reg == NREG)
				ctxt->diag("botch %P\n", p);
		}
		break;

	case AMOVZ:
	case AMOVN:
	case ACLZ:
	case ACLO:
		break;
	}

	
	c = p->to.class;
	if(c == 0) {
		c = aclassv(ctxt, &p->to) + 1;
		p->to.class = c;
	}
	c--;
	switch(c) {
	default:
		ctxt->diag("unknown class %^ %D\n", c, &p->to);

	case C_ZCON:
	case C_SCON:
	case C_ADD0CON:
	case C_AND0CON:
	case C_ADDCON:
	case C_ANDCON:
	case C_UCON:
	case C_LCON:
	case C_NONE:
	case C_SBRA:
	case C_LBRA:
		break;

	case C_HI:
	case C_LO:
		s->set.cc |= E_HILO;
		break;
	case C_FCREG:
		s->set.cc |= E_FCR;
		break;
	case C_MREG:
		s->set.cc |= E_MCR;
		break;
	case C_ZOREG:
	case C_SOREG:
	case C_LOREG:
		c = p->to.reg;
		s->used.ireg |= 1<<c;
		if(ad)
			break;
		s->size = sz;
		s->soffset = regoffv(ctxt, &p->to);

		m = ANYMEM;
		if(c == REGSP)
			m = E_MEMSP;

		if(ar)
			s->used.cc |= m;
		else
			s->set.cc |= m;
		break;
	case C_SACON:
	case C_LACON:
		if(p->to.reg == NREG)
			s->used.ireg |= 1<<REGSP;
		else
			s->used.ireg |= 1<<p->to.reg;
		break;
	case C_SECON:
	case C_LECON:
		break;
	case C_REG:
		if(ar)
			s->used.ireg |= 1<<p->to.reg;
		else
			s->set.ireg |= 1<<p->to.reg;
		break;
	case C_FREG:
		if(ar) {
			s->used.freg |= 1<<p->to.reg;
			s->used.freg |= 1<<(p->to.reg|1);
		} else {
			s->set.freg |= 1<<p->to.reg;
			s->set.freg |= 1<<(p->to.reg|1);
		}
		if(ld && p->from.type == D_REG)
			p->mark |= LOAD;
		break;
	case C_SAUTO:
	case C_LAUTO:
		s->used.ireg |= 1<<REGSP;
		if(ad)
			break;
		s->size = sz;
		s->soffset = regoffv(ctxt, &p->to);

		if(ar)
			s->used.cc |= E_MEMSP;
		else
			s->set.cc |= E_MEMSP;
		break;
	case C_SEXT:
	case C_LEXT:
		if(ad)
			break;
		s->size = sz;
		s->soffset = regoffv(ctxt, &p->to);

		if(ar)
			s->used.cc |= E_MEMSB;
		else
			s->set.cc |= E_MEMSB;
		break;
	}

	
	c = p->from.class;
	if(c == 0) {
		c = aclassv(ctxt, &p->from) + 1;
		p->from.class = c;
	}
	c--;
	switch(c) {
	default:
		ctxt->diag("unknown class %^ %D\n", c, &p->from);

	case C_ZCON:
	case C_SCON:
	case C_LCON:
	case C_ADD0CON:
	case C_AND0CON:
	case C_ADDCON:
	case C_ANDCON:
	case C_UCON:
	case C_NONE:
	case C_SBRA:
	case C_LBRA:
		break;
	case C_HI:
	case C_LO:
		s->used.cc |= E_HILO;
		break;
	case C_FCREG:
		s->used.cc |= E_FCR;
		break;
	case C_MREG:
		s->used.cc |= E_MCR;
		break;
	case C_ZOREG:
	case C_SOREG:
	case C_LOREG:
		c = p->from.reg;
		s->used.ireg |= 1<<c;
		if(ld)
			p->mark |= LOAD;
		s->size = sz;
		s->soffset = regoffv(ctxt, &p->from);

		m = ANYMEM;
		if(c == REGSP)
			m = E_MEMSP;

		s->used.cc |= m;
		break;
	case C_SACON:
	case C_LACON:
		if(p->from.reg == NREG)
			s->used.ireg |= 1 << REGSP;
		else
			s->used.ireg |= 1 << p->from.reg;
		break;
	case C_SECON:
	case C_LECON:
		break;
	case C_REG:
		s->used.ireg |= 1<<p->from.reg;
		break;
	case C_FREG:
		s->used.freg |= 1<<p->from.reg;
		s->used.freg |= 1<<(p->from.reg|1);
		if(ld && p->to.type == D_REG)
			p->mark |= LOAD;
		break;
	case C_SAUTO:
	case C_LAUTO:
		s->used.ireg |= 1<<REGSP;
		if(ld)
			p->mark |= LOAD;
		if(ad)
			break;
		s->size = sz;
		s->soffset = regoffv(ctxt, &p->from);

		s->used.cc |= E_MEMSP;
		break;
	case C_SEXT:
	case C_LEXT:
		if(ld)
			p->mark |= LOAD;
		if(ad)
			break;
		s->size = sz;
		s->soffset = regoffv(ctxt, &p->from);

		s->used.cc |= E_MEMSB;
		break;
	}

	c = p->reg;
	if(c != NREG) {
		if(p->from.type == D_FREG || p->to.type == D_FREG) {
			s->used.freg |= 1<<c;
			s->used.freg |= 1<<(c|1);
		} else
			s->used.ireg |= 1<<c;
	}
	s->set.ireg &= ~(1<<REGZERO);
}

static int
depend(Link *ctxt, Sch *sa, Sch *sb)
{
	uint32 x;

	if(sa->set.ireg & (sb->set.ireg|sb->used.ireg))
		return 1;
	if(sb->set.ireg & sa->used.ireg)
		return 1;

	if(sa->set.freg & (sb->set.freg|sb->used.freg))
		return 1;
	if(sb->set.freg & sa->used.freg)
		return 1;

	if(sa->used.cc & sb->used.cc & E_MEM)
		if(sa->p.reg == sb->p.reg)
		if(regoffv(ctxt, &sa->p.from) == regoffv(ctxt, &sb->p.from))
			return 1;

	x = (sa->set.cc & (sb->set.cc|sb->used.cc)) |
		(sb->set.cc & sa->used.cc);
	if(x) {
		if(x != E_MEMSP && x != E_MEMSB)
			return 1;
		x = sa->set.cc | sb->set.cc |
			sa->used.cc | sb->used.cc;
		if(x & E_MEM)
			return 1;
		if(offoverlap(sa, sb))
			return 1;
	}

	return 0; 
}

static int
offoverlap(Sch *sa, Sch *sb)
{

	if(sa->soffset < sb->soffset) {
		if(sa->soffset+sa->size > sb->soffset)
			return 1;
		return 0;
	}
	if(sb->soffset+sb->size > sa->soffset)
		return 1;
	return 0;
}

static int
conflict(Sch *sa, Sch *sb)
{

	if(sa->set.ireg & sb->used.ireg)
		return 1;
	if(sa->set.freg & sb->used.freg)
		return 1;
	if(sa->set.cc & sb->used.cc)
		return 1;

	return 0;
}

static void
dumpbits(Link *ctxt, Sch *s, Dep *d)
{
	int i;

	for(i=0; i<32; i++)
		if(d->ireg & (1<<i))
			Bprint(ctxt->bso, " R%d", i);
	for(i=0; i<32; i++)
		if(d->freg & (1<<i))
			Bprint(ctxt->bso, " F%d", i);
	for(i=0; i<32; i++)
		switch(d->cc & (1<<i)) {
		default:
			break;
		case E_HILO:
			Bprint(ctxt->bso, " HILO");
			break;
		case E_FCR:
			Bprint(ctxt->bso, " FCR");
			break;
		case E_MCR:
			Bprint(ctxt->bso, " MCR");
			break;
		case E_MEM:
			Bprint(ctxt->bso, " MEM%d", s->size);
			break;
		case E_MEMSB:
			Bprint(ctxt->bso, " SB%d", s->size);
			break;
		case E_MEMSP:
			Bprint(ctxt->bso, " SP%d", s->size);
			break;
		}
}
