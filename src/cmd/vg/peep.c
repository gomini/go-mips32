#include <u.h>
#include <libc.h>
#include "gg.h"
#include "opt.h"

static int	subprop(Flow*);
static int	copyprop(Graph*, Flow*);
static int	copy1(Addr*, Addr*, Flow*, int);
static int	copyas(Addr*, Addr*);
static int	copyau(Addr*, Addr*);
static int	copysub(Addr*, Addr*, Addr*, int);
static int	copysub1(Prog*, Addr*, Addr*, int);

static int	copyu(Prog*, Addr*, Addr*);
static int	copyau1(Prog*, Addr*);

static int	regzer(Addr*);
static int	a2type(Prog*);

static uint32	gactive;

void
peep(Prog* firstp)
{
	Flow *r, *r1;
	Graph *g;
	Prog *p, *p1;
	int t;

	g = flowstart(firstp, sizeof(Flow));
	if(g == nil)
		return;
	gactive = 0;

loop1:
	if(debug['P'] && debug['v'])
		dumpit("loop1", g->start, 0);

	t = 0;
	for(r=g->start; r!=nil; r=r->link) {
		p = r->prog;
		if(p->as == AMOVW || p->as == AMOVF || p->as == AMOVD)
		if(regtyp(&p->to)) {
			if(regtyp(&p->from))
			if(p->from.type == p->to.type) {
				if(copyprop(g, r)) {
					excise(r);
					t++;
				} else
				if(subprop(r) && copyprop(g, r)) {
					excise(r);
					t++;
				}
			}
			if(regzer(&p->from))
			if(p->to.type == D_REG) {
				p->from.type = D_REG;
				p->from.reg = 0;
				if(copyprop(g, r)) {
					excise(r);
					t++;
				} else
				if(subprop(r) && copyprop(g, r)) {
					excise(r);
					t++;
				}
			}
		}
	}
	if(t)
		goto loop1;
	for(r=g->start; r!=nil; r=r->link) {
		p = r->prog;
		switch(p->as) {
		default:
			continue;
		case AMOVH:
		case AMOVHU:
		case AMOVB:
		case AMOVBU:
			if(p->to.type != D_REG)
				continue;
			break;
		}
		r1 = r->link;
		if(r1 == nil)
			continue;
		p1 = r1->prog;
		if(p1->as != p->as)
			continue;
		if(p1->from.type != D_REG || p1->from.reg != p->to.reg)
			continue;
		if(p1->to.type != D_REG || p1->to.reg != p->to.reg)
			continue;
		excise(r1);
	}

	flowend(g);
}

static int
regzer(Addr *a)
{

	if(a->type == D_CONST) {
		if(a->name == D_NONE && a->reg == NREG && a->offset == 0)
			return 1;
		if(a->name == D_EXTERN || a->name == D_STATIC)
		if(a->sym != nil && a->sym->type == SCONST)
		if(a->sym->value + a->offset == 0)
			return 1;
	}
	if(a->type == D_REG && a->reg == 0)
		return 1;
	return 0;
}

int
regtyp(Addr *a)
{

	if(a->type == D_REG) {
		if(a->reg != 0)
			return 1;
		return 0;
	}
	if(a->type == D_FREG)
		return 1;
	return 0;
}

static int
subprop(Flow *r0)
{
	Prog *p;
	Addr *v1, *v2;
	Flow *r;
	int t;

	p = r0->prog;
	v1 = &p->from;
	if(!regtyp(v1))
		return 0;
	v2 = &p->to;
	if(!regtyp(v2))
		return 0;
	for(r=uniqp(r0); r!=nil; r=uniqp(r)) {
		if(uniqs(r) == nil)
			break;
		p = r->prog;
		switch(p->as) {
		case AJAL:
			return 0;

		case ASGT:
		case ASGTU:

		case AADD:
		case AADDU:
		case ASUB:
		case ASUBU:
		case ASLL:
		case ASRL:
		case ASRA:
		case AOR:
		case AAND:
		case AXOR:
		case AMUL:
		case AMULU:
		case AMUL32:
		case ADIV:
		case ADIVU:

		case AADDD:
		case AADDF:
		case ASUBD:
		case ASUBF:
		case AMULD:
		case AMULF:
		case ADIVD:
		case ADIVF:
			if(p->to.type == v1->type)
			if(p->to.reg == v1->reg) {
				if(p->reg == NREG)
					p->reg = p->to.reg;
				goto gotit;
			}
			break;

		case AMOVF:
		case AMOVD:
		case AMOVW:
			if(p->to.type == v1->type)
			if(p->to.reg == v1->reg)
				goto gotit;
			break;
		}
		if(copyau(&p->from, v2) ||
		   copyau1(p, v2) ||
		   copyau(&p->to, v2))
			break;
		if(copysub(&p->from, v1, v2, 0) ||
		   copysub1(p, v1, v2, 0) ||
		   copysub(&p->to, v1, v2, 0))
			break;
	}
	return 0;

gotit:
	copysub(&p->to, v1, v2, 1);
	if(debug['P']) {
		print("gotit: %D->%D\n%P", v1, v2, r->prog);
		if(p->from.type == v2->type)
			print(" excise");
		print("\n");
	}
	for(r=uniqs(r); r!=r0; r=uniqs(r)) {
		p = r->prog;
		copysub(&p->from, v1, v2, 1);
		copysub1(p, v1, v2, 1);
		copysub(&p->to, v1, v2, 1);
		if(debug['P'])
			print("%P\n", r->prog);
	}
	t = v1->reg;
	v1->reg = v2->reg;
	v2->reg = t;
	if(debug['P'])
		print("%P last\n", r->prog);
	return 1;
}

static int
copyprop(Graph *g, Flow *r0)
{
	Prog *p;
	Addr *v1, *v2;

	USED(g);
	p = r0->prog;
	v1 = &p->from;
	v2 = &p->to;
	if(copyas(v1, v2))
		return 1;
	gactive++;
	return copy1(v1, v2, r0->s1, 0);
}

static int
copy1(Addr *v1, Addr *v2, Flow *r, int f)
{
	int t;
	Prog *p;

	if(r->active == gactive) {
		if(debug['P'])
			print("act set; return 1\n");
		return 1;
	}
	r->active = gactive;
	if(debug['P'])
		print("copy %D->%D f=%d\n", v1, v2, f);
	for(; r != nil; r = r->s1) {
		p = r->prog;
		if(debug['P'])
			print("%P", p);
		if(!f && uniqp(r) == nil) {
			f = 1;
			if(debug['P'])
				print("; merge; f=%d", f);
		}
		t = copyu(p, v2, nil);
		switch(t) {
		case 2:
			if(debug['P'])
				print("; %Drar; return 0\n", v2);
			return 0;

		case 3:
			if(debug['P'])
				print("; %Dset; return 1\n", v2);
			return 1;

		case 1:
		case 4:
			if(f) {
				if(!debug['P'])
					return 0;
				if(t == 4)
					print("; %Dused+set and f=%d; return 0\n", v2, f);
				else
					print("; %Dused and f=%d; return 0\n", v2, f);
				return 0;
			}
			if(copyu(p, v2, v1)) {
				if(debug['P'])
					print("; sub fail; return 0\n");
				return 0;
			}
			if(debug['P'])
				print("; sub%D/%D", v2, v1);
			if(t == 4) {
				if(debug['P'])
					print("; %Dused+set; return 1\n", v2);
				return 1;
			}
			break;
		}
		if(!f) {
			t = copyu(p, v1, nil);
			if(!f && (t == 2 || t == 3 || t == 4)) {
				f = 1;
				if(debug['P'])
					print("; %Dset and !f; f=%d", v1, f);
			}
		}
		if(debug['P'])
			print("\n");
		if(r->s2)
			if(!copy1(v1, v2, r->s2, f))
				return 0;
	}
	return 1;
}

static int
copyu(Prog *p, Addr *v, Addr *s)
{

	switch(p->as) {

	default:
		if(debug['P'])
			print(" (?)");
		return 2;


	case ANOP:
	case AMOVW:
	case AMOVF:
	case AMOVD:
	case AMOVH:
	case AMOVHU:
	case AMOVB:
	case AMOVBU:
	case AMOVFW:
	case AMOVWF:
	case AMOVDW:
	case AMOVWD:
	case AMOVFD:
	case AMOVDF:
		if(s != nil) {
			if(copysub(&p->from, v, s, 1))
				return 1;
			if(!copyas(&p->to, v))
				if(copysub(&p->to, v, s, 1))
					return 1;
			return 0;
		}
		if(copyas(&p->to, v)) {
			if(copyau(&p->from, v))
				return 4;
			return 3;
		}
		if(copyau(&p->from, v))
			return 1;
		if(copyau(&p->to, v))
			return 1;
		return 0;

	case ASGT:
	case ASGTU:

	case AADD:
	case AADDU:
	case ASUB:
	case ASUBU:
	case ASLL:
	case ASRL:
	case ASRA:
	case AOR:
	case ANOR:
	case AAND:
	case AXOR:
	case AMUL:
	case AMULU:
	case AMUL32:
	case ADIV:
	case ADIVU:

	case AADDF:
	case AADDD:
	case ASUBF:
	case ASUBD:
	case AMULF:
	case AMULD:
	case ADIVF:
	case ADIVD:
	case ACHECKNIL:
		if(s != nil) {
			if(copysub(&p->from, v, s, 1))
				return 1;
			if(copysub1(p, v, s, 1))
				return 1;
			if(!copyas(&p->to, v))
				if(copysub(&p->to, v, s, 1))
					return 1;
			return 0;
		}
		if(copyas(&p->to, v)) {
			if(p->reg == NREG)
				p->reg = p->to.reg;
			if(copyau(&p->from, v))
				return 4;
			if(copyau1(p, v))
				return 4;
			return 3;
		}
		if(copyau(&p->from, v))
			return 1;
		if(copyau1(p, v))
			return 1;
		if(copyau(&p->to, v))
			return 1;
		return 0;

	case ABEQ:
	case ABNE:
	case ABGTZ:
	case ABGEZ:
	case ABLTZ:
	case ABLEZ:

	case ACMPEQD:
	case ACMPEQF:
	case ACMPGED:
	case ACMPGEF:
	case ACMPGTD:
	case ACMPGTF:
	case ABFPF:
	case ABFPT:
		if(s != nil) {
			if(copysub(&p->from, v, s, 1))
				return 1;
			return copysub1(p, v, s, 1);
		}
		if(copyau(&p->from, v))
			return 1;
		if(copyau1(p, v))
			return 1;
		return 0;

	case AJMP:
		if(s != nil) {
			if(copysub(&p->to, v, s, 1))
				return 1;
			return 0;
		}
		if(copyau(&p->to, v))
			return 1;
		return 0;

	case ARET:
		if(v->type == D_REG)
		if(v->reg == REGRET)
			return 2;
		if(v->type == D_FREG)
		if(v->reg == FREGRET)
			return 2;

	case AJAL:
		if(v->type == D_REG) {
			if(v->reg <= REGEXT && v->reg > exregoffset)
				return 2;
			if(REGARG > 0 && v->reg == REGARG)
				return 2;
		}
		if(v->type == D_FREG)
			if(v->reg <= FREGEXT && v->reg > exfregoffset)
				return 2;
		if(p->from.type == D_REG && v->type == D_REG && p->from.reg == v->reg)
			return 2;

		if(s != nil) {
			if(copysub(&p->to, v, s, 1))
				return 1;
			return 0;
		}
		if(copyau(&p->to, v))
			return 4;
		return 3;

	case ADUFFZERO:
		if(v->type == D_REG) {
			if(v->reg == REGRT1)
				return 2;
		}
		return 0;

	case ADUFFCOPY:
		if(v->type == D_REG) {
			if(v->reg >= REGRT1 && v->reg < REGRT1+4)
				return 3;
			if(v->reg == REGRT1+4 || v->reg == REGRT1+5)
				return 2;
		}
		return 0;

	case ATEXT:
		if(v->type == D_REG)
			if(v->reg == REGARG)
				return 3;
		return 0;

	case APCDATA:
	case AFUNCDATA:
	case AVARDEF:
	case AVARKILL:
		return 0;
	}
}

static int
a2type(Prog *p)
{

	switch(p->as) {
	case ABEQ:
	case ABNE:
	case ABGTZ:
	case ABGEZ:
	case ABLTZ:
	case ABLEZ:

	case ASGT:
	case ASGTU:

	case AADD:
	case AADDU:
	case ASUB:
	case ASUBU:
	case ASLL:
	case ASRL:
	case ASRA:
	case AOR:
	case AAND:
	case AXOR:
	case AMUL:
	case AMULU:
	case AMUL32:
	case ADIV:
	case ADIVU:
		return D_REG;

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
		return D_FREG;
	}
	return D_NONE;
}

static int
copyas(Addr *a, Addr *v)
{

	if(regtyp(v)) {
		if(a->type == v->type)
		if(a->reg == v->reg)
			return 1;
	} else if(v->type == D_CONST) {
		if(a->type == v->type)
		if(a->name == v->name)
		if(a->sym == v->sym)
		if(a->reg == v->reg)
		if(a->offset == v->offset)
			return 1;
	}
	return 0;
}

int
sameaddr(Addr *a, Addr *v)
{
	if(a->type != v->type)
		return 0;
	if(regtyp(v) && a->reg == v->reg)
		return 1;
	if(v->type == D_AUTO || v->type == D_PARAM) {
		if(v->offset == a->offset)
			return 1;
	}
	return 0;
}

static int
copyau(Addr *a, Addr *v)
{

	if(copyas(a, v))
		return 1;
	if(v->type == D_REG) {
		if(a->type == D_CONST && a->reg != NREG) {
			if(a->reg == v->reg)
				return 1;
		} else if(a->type == D_OREG) {
			if(v->reg == a->reg)
				return 1;
		}
	}
	return 0;
}

static int
copyau1(Prog *p, Addr *v)
{

	if(regtyp(v))
		if(p->from.type == v->type || p->to.type == v->type)
		if(p->reg == v->reg) {
			if(a2type(p) != v->type)
				print("botch a2type %P\n", p);
			return 1;
		}
	return 0;
}

static int
copysub(Addr *a, Addr *v, Addr *s, int f)
{

	if(f)
	if(copyau(a, v))
		a->reg = s->reg;
	return 0;
}

static int
copysub1(Prog *p1, Addr *v, Addr *s, int f)
{

	if(f)
	if(copyau1(p1, v))
		p1->reg = s->reg;
	return 0;
}

int
stackaddr(Addr *a)
{
	return regtyp(a) && a->reg == REGSP;
}

int
smallindir(Addr *a, Addr *reg)
{
	if(reg->type == D_REG)
	if(a->type == D_OREG)
	if(a->reg == reg->reg)
	if(0 <= a->offset && a->offset < 4096)
		return 1;
	return 0;
}
