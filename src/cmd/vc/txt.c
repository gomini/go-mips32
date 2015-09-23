#include "gc.h"

static void	gopcode64(int o, Node *f1, Node *f2, Node *t);


int thechar = 'v';
char *thestring = "mips32";

LinkArch	*thelinkarch = &linkmips32;

void
linkarchinit(void)
{
	isbigendian = 1;
	if(strcmp(getgoarch(), "mips32le") == 0) {
		thelinkarch = &linkmips32le;
		isbigendian = 0;
	}
}

void
ginit(void)
{
	int i;
	Type *t;

	exregoffset = REGEXT;
	exfregoffset = FREGEXT;
	listinit();
	nstring = 0;
	mnstring = 0;
	nrathole = 0;
	pc = 0;
	breakpc = -1;
	continpc = -1;
	cases = C;
	lastp = P;
	tfield = types[TLONG];

	zprog.link = P;
	zprog.as = AGOK;
	zprog.reg = NREG;
	zprog.from.type = D_NONE;
	zprog.from.name = D_NONE;
	zprog.from.reg = NREG;
	zprog.to = zprog.from;

	regnode.op = OREGISTER;
	regnode.class = CEXREG;
	regnode.reg = REGTMP;
	regnode.complex = 0;
	regnode.addable = 11;
	regnode.type = types[TLONG];

	constnode.op = OCONST;
	constnode.class = CXXX;
	constnode.complex = 0;
	constnode.addable = 20;
	constnode.type = types[TLONG];

	fconstnode.op = OCONST;
	fconstnode.class = CXXX;
	fconstnode.complex = 0;
	fconstnode.addable = 20;
	fconstnode.type = types[TDOUBLE];

	nodsafe = new(ONAME, Z, Z);
	nodsafe->sym = slookup(".safe");
	nodsafe->type = types[TINT];
	nodsafe->etype = types[TINT]->etype;
	nodsafe->class = CAUTO;
	complex(nodsafe);

	t = typ(TARRAY, types[TCHAR]);
	symrathole = slookup(".rathole");
	symrathole->class = CGLOBL;
	symrathole->type = t;

	nodrat = new(ONAME, Z, Z);
	nodrat->sym = symrathole;
	nodrat->type = types[TIND];
	nodrat->etype = TVOID;
	nodrat->class = CGLOBL;
	complex(nodrat);
	nodrat->type = t;

	nodret = new(ONAME, Z, Z);
	nodret->sym = slookup(".ret");
	nodret->type = types[TIND];
	nodret->etype = TIND;
	nodret->class = CPARAM;
	nodret = new(OIND, nodret, Z);
	complex(nodret);

	com64init();

	for(i=0; i<nelem(reg); i++) {
		reg[i] = 0;
		if(i == REGZERO ||
		  (i >= NREG && ((i-NREG)&1)))
			reg[i] = 1;
	}
}

void
gclean(void)
{
	int i;
	Sym *s;

	for(i=0; i<NREG; i++)
		if(i != REGZERO)
			if(reg[i])
				diag(Z, "reg %d left allocated", i);
	for(i=NREG; i<NREG+NREG; i+=2)
		if(reg[i])
			diag(Z, "freg %d left allocated", i-NREG);
	while(mnstring)
		outstring("", 1L);
	symstring->type->width = nstring;
	symrathole->type->width = nrathole;
	for(i=0; i<NHASH; i++)
	for(s = hash[i]; s != S; s = s->link) {
		if(s->type == T)
			continue;
		if(s->type->width == 0)
			continue;
		if(s->class != CGLOBL && s->class != CSTATIC)
			continue;
		if(s->type == types[TENUM])
			continue;
		gpseudo(AGLOBL, s, nodconst(s->type->width));
	}
	nextpc();
	p->as = AEND;
	outcode();
}

void
nextpc(void)
{
	Plist *pl;

	p = alloc(sizeof(*p));
	*p = zprog;
	p->lineno = nearln;
	p->pc = pc;
	pc++;
	if(lastp == P) {
		pl = linknewplist(ctxt);
		pl->firstpc = p;
	} else
		lastp->link = p;
	lastp = p;
}

void
gargs(Node *n, Node *tn1, Node *tn2)
{
	int32 regs;
	Node fnxargs[20], *fnxp;

	regs = cursafe;

	fnxp = fnxargs;
	garg1(n, tn1, tn2, 0, &fnxp);

	curarg = 0;
	fnxp = fnxargs;
	garg1(n, tn1, tn2, 1, &fnxp);

	cursafe = regs;
}

void
garg1(Node *n, Node *tn1, Node *tn2, int f, Node **fnxp)
{
	Node nod;

	if(n == Z)
		return;
	if(n->op == OLIST) {
		garg1(n->left, tn1, tn2, f, fnxp);
		garg1(n->right, tn1, tn2, f, fnxp);
		return;
	}
	if(f == 0) {
		if(n->complex >= FNX) {
			regsalloc(*fnxp, n);
			nod = znode;
			nod.op = OAS;
			nod.left = *fnxp;
			nod.right = n;
			nod.type = n->type;
			cgen(&nod, Z);
			(*fnxp)++;
		}
		return;
	}
	if(typesuv[n->type->etype]) {
		regaalloc(tn2, n);
		if(n->complex >= FNX) {
			sugen(*fnxp, tn2, n->type->width);
			(*fnxp)++;
		} else
			sugen(n, tn2, n->type->width);
		return;
	}
	if(REGARG>0 && curarg == 0 && typechlp[n->type->etype]) {
		regaalloc1(tn1, n);
		if(n->complex >= FNX) {
			cgen(*fnxp, tn1);
			(*fnxp)++;
		} else
			cgen(n, tn1);
		return;
	}
	if(vconst(n) == 0) {
		regaalloc(tn2, n);
		gopcode(OAS, n, Z, tn2);
		return;
	}
	regalloc(tn1, n, Z);
	if(n->complex >= FNX) {
		cgen(*fnxp, tn1);
		(*fnxp)++;
	} else
		cgen(n, tn1);
	regaalloc(tn2, n);
	gopcode(OAS, tn1, Z, tn2);
	regfree(tn1);
}

Node*
nodconst(int32 v)
{
	constnode.vconst = v;
	return &constnode;
}

Node*
nod32const(vlong v)
{
	constnode.vconst = v & MASK(32);
	return &constnode;
}

Node*
nodfconst(double d)
{
	fconstnode.fconst = d;
	return &fconstnode;
}

void
nodreg(Node *n, Node *nn, int reg)
{
	*n = regnode;
	n->reg = reg;
	n->type = nn->type;
	n->lineno = nn->lineno;
}

void
regret(Node *n, Node *nn, Type *t, int mode)
{
	int r;

	if(mode == 0 || hasdotdotdot(t) || nn->type->width == 0) {
		r = REGRET;
		if(typefd[nn->type->etype])
			r = FREGRET + NREG;
		nodreg(n, nn, r);
		reg[r]++;
		return;
	}
	if(mode == 1) {
		curarg = (curarg+3) & ~3;
		regaalloc(n, nn);
		return;
	}
	if(mode == 2) {
		if(t->etype != TFUNC)
			fatal(Z, "bad regret func %T", t);
		*n = *nn;
		n->op = ONAME;
		n->class = CPARAM;
		n->sym = slookup(".ret");
		n->complex = nodret->complex;
		n->addable = 20;
		n->xoffset = argsize(0);
		return;
	}

	fatal(Z, "bad regret");
}

int
tmpreg(void)
{
	int i;

	for(i=REGRET+1; i<NREG; i++)
		if(reg[i] == 0)
			return i;
	diag(Z, "out of fixed registers");
	return 0;
}

void
regalloc(Node *n, Node *tn, Node *o)
{
	int i, t;

	switch(t = tn->type->etype) {
	case TCHAR:
	case TUCHAR:
	case TSHORT:
	case TUSHORT:
	case TINT:
	case TUINT:
	case TLONG:
	case TULONG:
	case TIND:
		if(o != Z && o->op == OREGISTER) {
			i = o->reg;
			if(i > 0 && i < NREG)
				goto out;
		}
		for(i = REGRET + 1; i <= REGMAX; i++)
			if(reg[i] == 0)
				goto out;
		diag(tn, "out of fixed registers");
		goto err;

	case TFLOAT:
	case TDOUBLE:
		if(o != Z && o->op == OREGISTER) {
			i = o->reg;
			if(i >= NREG && i < NREG+NREG)
				goto out;
		}
		for(i = NREG; i < NREG + NREG; i += 2)
			if(reg[i] == 0)
				goto out;
		diag(tn, "out of float registers");
		goto err;
	case TVLONG:
	case TUVLONG:
		n->op = OREGPAIR;
		n->complex = 0;
		n->addable = 11;
		n->type = tn->type;
		n->lineno = tn->lineno;
		n->left = alloc(sizeof(Node));
		n->right = alloc(sizeof(Node));
		if(o != Z && o->op == OREGPAIR) {
			regalloc(n->left, &regnode, o->left);
			regalloc(n->right, &regnode, o->right);
		} else {
			regalloc(n->left, &regnode, Z);
			regalloc(n->right, &regnode, Z);
		}
		n->right->type = types[TULONG];
		if(tn->type->etype == TUVLONG)
			n->left->type = types[TULONG];
		return;
	}
	diag(tn, "unknown type in regalloc: %T", tn->type);
err:
	nodreg(n, tn, 0);
	return;
out:
	reg[i]++;
	nodreg(n, tn, i);
}

void
regialloc(Node *n, Node *tn, Node *o)
{
	Node nod;

	nod = *tn;
	nod.type = types[TIND];
	regalloc(n, &nod, o);
}

void
regfree(Node *n)
{
	int i;

	if(n->op == OREGPAIR) {
		regfree(n->left);
		regfree(n->right);
		return;
	}
	i = 0;
	if(n->op != OREGISTER && n->op != OINDREG)
		goto err;
	i = n->reg;
	if(i < 0 || i >= nelem(reg))
		goto err;
	if(reg[i] <= 0)
		goto err;
	reg[i]--;
	return;
err:
	diag(n, "error in regfree: %d", i);
}

void
regsalloc(Node *n, Node *nn)
{
	cursafe = align(cursafe, nn->type, Aaut3, nil);
	maxargsafe = maxround(maxargsafe, cursafe+curarg);
	*n = *nodsafe;
	n->xoffset = -(stkoff + cursafe);
	n->type = nn->type;
	n->etype = nn->type->etype;
	n->lineno = nn->lineno;
}

void
regaalloc1(Node *n, Node *nn)
{
	if(REGARG < 0) {
		fatal(n, "regaalloc1 and REGARG<0");
		return;
	}
	nodreg(n, nn, REGARG);
	reg[REGARG]++;
	curarg = align(curarg, nn->type, Aarg1, nil);
	curarg = align(curarg, nn->type, Aarg2, nil);
	maxargsafe = maxround(maxargsafe, cursafe+curarg);
}

void
regaalloc(Node *n, Node *nn)
{
	curarg = align(curarg, nn->type, Aarg1, nil);
	*n = *nn;
	n->op = OINDREG;
	n->reg = REGSP;
	n->xoffset = curarg + SZ_LONG;
	n->complex = 0;
	n->addable = 20;
	curarg = align(curarg, nn->type, Aarg2, nil);
	maxargsafe = maxround(maxargsafe, cursafe+curarg);
}

void
regind(Node *n, Node *nn)
{

	if(n->op != OREGISTER) {
		diag(n, "regind not OREGISTER");
		return;
	}
	n->op = OINDREG;
	n->type = nn->type;
}

void
raddr(Node *n, Prog *p)
{
	Addr a;

	naddr(n, &a);
	if(a.type == D_CONST && a.offset == 0) {
		a.type = D_REG;
		a.reg = 0;
	}
	if(a.type != D_REG && a.type != D_FREG) {
		if(n)
			diag(n, "bad in raddr: %O:%lld", n->op, n->vconst);
		else
			diag(n, "bad in raddr: <null>");
		p->reg = NREG;
	} else
		p->reg = a.reg;
}

void
naddr(Node *n, Addr *a)
{
	int32 v;

	a->type = D_NONE;
	if(n == Z)
		return;
	switch(n->op) {
	default:
	bad:
		prtree(n, "naddr");
		diag(n, "%L: !bad in naddr: %O", n->lineno, n->op);
		break;

	case OREGISTER:
		a->type = D_REG;
		a->sym = nil;
		a->reg = n->reg;
		if(a->reg >= NREG) {
			a->type = D_FREG;
			a->reg -= NREG;
		}
		break;

	case OIND:
		naddr(n->left, a);
		a->offset += n->xoffset;
		if(a->type == D_REG) {
			a->type = D_OREG;
			break;
		}
		if(a->type == D_CONST) {
			a->type = D_OREG;
			break;
		}
		goto bad;

	case OINDREG:
		a->type = D_OREG;
		a->sym = linksym(n->sym);
		a->offset = n->xoffset;
		a->reg = n->reg;
		break;

	case ONAME:
		a->etype = n->etype;
		a->type = D_OREG;
		a->name = D_STATIC;
		a->sym = linksym(n->sym);
		a->offset = n->xoffset;
		if(n->class == CSTATIC)
			break;
		if(n->class == CEXTERN || n->class == CGLOBL) {
			a->name = D_EXTERN;
			break;
		}
		if(n->class == CAUTO) {
			a->name = D_AUTO;
			break;
		}
		if(n->class == CPARAM) {
			a->name = D_PARAM;
			break;
		}
		goto bad;

	case OCONST:
		a->sym = nil;
		a->reg = NREG;
		if(typefd[n->type->etype]) {
			a->type = D_FCONST;
			a->u.dval = n->fconst;
		} else {
			a->type = D_CONST;
			a->offset = n->vconst;
		}
		break;

	case OADDR:
		naddr(n->left, a);
		if(a->type == D_OREG) {
			a->type = D_CONST;
			break;
		}
		goto bad;

	case OADD:
		if(n->left->op == OCONST) {
			naddr(n->left, a);
			v = a->offset;
			naddr(n->right, a);
		} else {
			naddr(n->right, a);
			v = a->offset;
			naddr(n->left, a);
		}
		a->offset += v;
		break;

	}
}

void
gloadmsb(Node *f, Node *t)
{
	Type *ft;

	if(f->op == OREGPAIR) {
		gmove(f->left, t);
		return;
	}
	if(f->op == OCONST) {
		gmove(nodconst(f->vconst>>32), t);
		return;
	}
	ft = f->type;
	f->type = types[TLONG];
	if(!isbigendian)
		f->xoffset += SZ_LONG;
	gmove(f, t);
	f->type = ft;
	if(!isbigendian)
		f->xoffset -= SZ_LONG;
}

void
gloadlsb(Node *f, Node *t)
{
	Type *ft;

	if(f->op == OREGPAIR) {
		gmove(f->right, t);
		return;
	}
	if(f->op == OCONST) {
		gmove(nodconst(f->vconst), t);
		return;
	}
	ft = f->type;
	f->type = types[TLONG];
	if(isbigendian)
		f->xoffset += SZ_LONG;
	gmove(f, t);
	f->type = ft;
	if(isbigendian)
		f->xoffset -= SZ_LONG;
}

void
fop(int as, int f1, int f2, Node *t)
{
	Node nod1, nod2, nod3;

	nodreg(&nod1, t, NREG+f1);
	nodreg(&nod2, t, NREG+f2);
	regalloc(&nod3, t, t);
	gopcode(as, &nod1, &nod2, &nod3);
	gmove(&nod3, t);
	regfree(&nod3);
}

#define CASE(a,b)	((a<<8)|(b<<0))
#define DBL(t)		((t) == TDOUBLE)

void
gmove(Node *f, Node *t)
{
	int ft, tt, a, i;
	Node nod, nod1, nod2;
	Prog *p1;
	double d;

	ft = f->type->etype;
	tt = t->type->etype;

	if(ft == TDOUBLE && f->op == OCONST) {
		d = f->fconst;
		if(d == 0.0) {
			a = FREGZERO;
			goto ffreg;
		}
		if(d == 0.5) {
			a = FREGHALF;
			goto ffreg;
		}
		if(d == 1.0) {
			a = FREGONE;
			goto ffreg;
		}
		if(d == 2.0) {
			a = FREGTWO;
			goto ffreg;
		}
		if(d == -.5) {
			fop(OSUB, FREGHALF, FREGZERO, t);
			return;
		}
		if(d == -1.0) {
			fop(OSUB, FREGONE, FREGZERO, t);
			return;
		}
		if(d == -2.0) {
			fop(OSUB, FREGTWO, FREGZERO, t);
			return;
		}
		if(d == 1.5) {
			fop(OADD, FREGONE, FREGHALF, t);
			return;
		}
		if(d == 2.5) {
			fop(OADD, FREGTWO, FREGHALF, t);
			return;
		}
		if(d == 3.0) {
			fop(OADD, FREGTWO, FREGONE, t);
			return;
		}
	}
	if(ft == TFLOAT && f->op == OCONST) {
		d = f->fconst;
		if(d == 0) {
			a = FREGZERO;
		ffreg:
			nodreg(&nod, f, NREG+a);
			gmove(&nod, t);
			return;
		}
	}
	if(ft == TVLONG || ft == TUVLONG)
	if(f->op == OCONST && t->op == OREGPAIR) {
			gmove(nod32const(f->vconst>>32), t->left);
			gmove(nod32const(f->vconst), t->right);
		return;
	}
	if(f->op == ONAME || f->op == OINDREG || f->op == OIND) {
		switch(ft) {
		default:
			if(typefd[tt]) {
				regalloc(&nod, t, t);
				gins(AMOVW, f, &nod);
				a = AMOVWD;
				if(tt == TFLOAT)
					a = AMOVWF;
				gins(a, &nod, &nod);
				gmove(&nod, t);
				regfree(&nod);
				return;
			}
			a = AMOVW;
			break;
		case TFLOAT:
			a = AMOVF;
			break;
		case TDOUBLE:
			a = AMOVD;
			break;
		case TCHAR:
			a = AMOVB;
			break;
		case TUCHAR:
			a = AMOVBU;
			break;
		case TSHORT:
			a = AMOVH;
			break;
		case TUSHORT:
			a = AMOVHU;
			break;
		}
		if(typev[ft]) {
			if(typev[tt]) {
				regalloc(&nod, f, t);
				if(isbigendian)
					f->xoffset += SZ_LONG;
				gins(AMOVW, f, nod.right);
				if(isbigendian)
					f->xoffset -= SZ_LONG;
				else
					f->xoffset += SZ_LONG;
				gins(AMOVW, f, nod.left);
				if(!isbigendian)
					f->xoffset -= SZ_LONG;
			} else {
				regalloc(&nod, &regnode, t);
				if(isbigendian)
					f->xoffset += SZ_LONG;
				gins(AMOVW, f, &nod);
				if(isbigendian)
					f->xoffset -= SZ_LONG;
			}
			gmove(&nod, t);
			regfree(&nod);
			return;
		}

		if(typechlp[ft] && typeilp[tt])
			regalloc(&nod, t, t);
		else
			regalloc(&nod, f, t);
		gins(a, f, &nod);
		gmove(&nod, t);
		regfree(&nod);
		return;
	}

	if(t->op == ONAME || t->op == OINDREG || t->op == OIND) {
		switch(tt) {
		default:
			a = AMOVW;
			break;
		case TUCHAR:
			a = AMOVBU;
			break;
		case TCHAR:
			a = AMOVB;
			break;
		case TUSHORT:
			a = AMOVHU;
			break;
		case TSHORT:
			a = AMOVH;
			break;
		case TFLOAT:
			a = AMOVF;
			break;
		case TDOUBLE:
			a = AMOVD;
			break;
		}
		if(!typefd[ft] && vconst(f) == 0) {
			gins(a, f, t);
			if(typev[tt]) {
				t->xoffset += SZ_LONG;
				gins(a, f, t);
				t->xoffset -= SZ_LONG;
			}
			return;
		}
		if(ft == tt)
			regalloc(&nod, t, f);
		else
			regalloc(&nod, t, Z);
		gmove(f, &nod);
		if(typev[tt]) {
			if(isbigendian)
				t->xoffset += SZ_LONG;
			gins(a, nod.right, t);
			if(isbigendian)
				t->xoffset -= SZ_LONG;
			else
				t->xoffset += SZ_LONG;
			gins(AMOVW, nod.left, t);
			if(!isbigendian)
				t->xoffset -= SZ_LONG;
		} else
			gins(a, &nod, t);
		regfree(&nod);
		return;
	}

	a = AGOK;
	switch(ft) {
	case TDOUBLE:
	case TFLOAT:
		switch(tt) {
		case TDOUBLE:
			a = AMOVD;
			if(ft == TFLOAT)
				a = AMOVFD;
			break;
		case TFLOAT:
			a = AMOVDF;
			if(ft == TFLOAT)
				a = AMOVF;
			break;
		case TINT:
		case TUINT:
		case TLONG:
		case TULONG:
		case TIND:
		case TSHORT:
		case TUSHORT:
		case TCHAR:
		case TUCHAR:
			if(fproundflg) {
				regalloc(&nod, f, Z);
				gins(AMOVDW, f, &nod);
				if(ft == TFLOAT)
					p->as = AMOVFW;
				gins(AMOVW, &nod, t);
				regfree(&nod);
				gins(AMOVW, t, t);
				return;
			}
			regalloc(&nod1, &regnode, Z);
			regalloc(&nod2, &regnode, Z);

			gins(AMOVW, Z, &nod1);
			p->from.type = D_FCREG;
			p->from.reg = 31;

			for(i=0; i < 2; i++) {
				gins(ASLL, nodconst(0), nodconst(0));
				p->to.type = D_REG;
				p->to.reg = 0;
				p->from.type = D_REG;
				p->from.reg = 0;
			}

			gins(AOR, nodconst(3), &nod2);
			p->reg = nod1.reg;

			gins(AXOR, nodconst(2), &nod2);

			gins(AMOVW, &nod2, Z);
			p->to.type = D_FCREG;
			p->to.reg = 31;

			for(i=0; i<2; i++) {
				gins(ASLL, nodconst(0), nodconst(0));
				p->to.type = D_REG;
				p->to.reg = 0;
				p->from.type = D_REG;
				p->from.reg = 0;
			}

			regalloc(&nod, f, Z);
			gins(AMOVDW, f, &nod);
			if(ft == TFLOAT)
				p->as = AMOVFW;
			gins(AMOVW, &nod, t);
			regfree(&nod);
			gins(AMOVW, t, t);

			gins(AMOVW, &nod1, Z);
			p->to.type = D_FCREG;
			p->to.reg = 31;

			for(i=0; i<2; i++) {
				gins(ASLL, nodconst(0), nodconst(0));
				p->to.type = D_REG;
				p->to.reg = 0;
				p->from.type = D_REG;
				p->from.reg = 0;
			}

			regfree(&nod1);
			regfree(&nod2);
			return;
		}
		break;
	case TINT:
	case TUINT:
	case TLONG:
	case TULONG:
	case TIND:
		switch(tt) {
		case TDOUBLE:
			gins(AMOVW, f, t);
			gins(AMOVWD, t, t);
			if(ft == TULONG || ft == TUINT) {
				regalloc(&nod, t, Z);
				gins(ACMPGED, t, Z);
				p->reg = FREGZERO;
				gins(ABFPT, Z, Z);
				p1 = p;
				gins(AMOVD, nodfconst(4294967296.), &nod);
				gins(AADDD, &nod, t);
				patch(p1, pc);
				regfree(&nod);
			}
			return;
		case TFLOAT:
			gins(AMOVW, f, t);
			gins(AMOVWF, t, t);
			if(ft == TULONG || ft == TUINT) {
				regalloc(&nod, t, Z);
				gins(ACMPGEF, t, Z);
				p->reg = FREGZERO;
				gins(ABFPT, Z, Z);
				p1 = p;
				gins(AMOVF, nodfconst(4294967296.), &nod);
				gins(AADDF, &nod, t);
				patch(p1, pc);
				regfree(&nod);
			}
			return;
		case TINT:
		case TUINT:
		case TLONG:
		case TULONG:
		case TIND:
			a = AMOVW;
			break;
		case TSHORT:
			a = AMOVH;
			break;
		case TUSHORT:
			a = AMOVHU;
			break;
		case TCHAR:
			a = AMOVB;
			break;
		case TUCHAR:
			a = AMOVBU;
			break;
		}
		break;
	case TSHORT:
		switch(tt) {
		case TDOUBLE:
		case TFLOAT:
			regalloc(&nod, f, Z);
			gins(AMOVH, f, &nod);
			gins(AMOVW, &nod, t);
			gins(AMOVWD, t, t);
			if(tt == TFLOAT)
				p->as = AMOVWF;
			regfree(&nod);
			return;
		case TINT:
		case TUINT:
		case TLONG:
		case TULONG:
		case TIND:
			a = AMOVH;
			break;
		case TUSHORT:
			a = AMOVHU;
			break;
		case TSHORT:
			a = AMOVW;
			break;
		case TCHAR:
			a = AMOVB;
			break;
		case TUCHAR:
			a = AMOVBU;
			break;
		}
		break;
	case TUSHORT:
		switch(tt) {
		case TDOUBLE:
		case TFLOAT:
			regalloc(&nod, f, Z);
			gins(AMOVHU, f, &nod);
			gins(AMOVW, &nod, t);
			gins(AMOVWD, t, t);
			if(tt == TFLOAT)
				p->as = AMOVWF;
			regfree(&nod);
			return;
		case TINT:
		case TUINT:
		case TLONG:
		case TULONG:
		case TIND:
			a = AMOVHU;
			break;
		case TSHORT:
			a = AMOVH;
			break;
		case TCHAR:
			a = AMOVB;
			break;
		case TUCHAR:
			a = AMOVBU;
			break;
		case TUSHORT:
			a = AMOVW;
			break;
		}
		break;
	case TCHAR:
		switch(tt) {
		case TDOUBLE:
		case TFLOAT:
			regalloc(&nod, f, Z);
			gins(AMOVB, f, &nod);
			gins(AMOVW, &nod, t);
			gins(AMOVWD, t, t);
			if(tt == TFLOAT)
				p->as = AMOVWF;
			regfree(&nod);
			return;
		case TINT:
		case TUINT:
		case TLONG:
		case TULONG:
		case TIND:
		case TSHORT:
		case TUSHORT:
			a = AMOVB;
			break;
		case TUCHAR:
			a = AMOVBU;
			break;
		case TCHAR:
			a = AMOVW;
			break;
		}
		break;
	case TUCHAR:
		switch(tt) {
		case TDOUBLE:
		case TFLOAT:
			regalloc(&nod, f, Z);
			gins(AMOVBU, f, &nod);
			gins(AMOVW, &nod, t);
			gins(AMOVWD, t, t);
			if(tt == TFLOAT)
				p->as = AMOVWF;
			regfree(&nod);
			return;
		case TINT:
		case TUINT:
		case TLONG:
		case TULONG:
		case TIND:
		case TSHORT:
		case TUSHORT:
			a = AMOVBU;
			break;
		case TCHAR:
			a = AMOVB;
			break;
		case TUCHAR:
			a = AMOVW;
			break;
		}
		break;
	case TVLONG:
	case TUVLONG:
		switch(tt) {
		case TVLONG:
		case TUVLONG:
			a = AMOVW;
			break;
		}
		break;
	}

	if(a == AGOK)
		diag(Z, "bad opcode in gmove %T -> %T", f->type, t->type);

	if(a == AMOVW || a == AMOVF || a == AMOVD)
	if(samaddr(f, t))
		return;
	if(typev[ft]) {
		if(f->op != OREGPAIR || t->op != OREGPAIR)
			diag(Z, "bad vlong in gmove (%O->%O)", f->op, t->op);
		gins(a, f->left, t->left);
		gins(a, f->right, t->right);
	} else
		gins(a, f, t);
}

static void
gmul(Node *f1, Node *f2, Node *lo, Node *hi, int sign)
{

	if(f1->op == OCONST && f1->vconst == 0
	|| f2->op == OCONST && f1->vconst == 0
	|| f1->op == OREGISTER && f1->reg == 0
	|| f2->op == OREGISTER && f2->reg == 0) {
		if(lo != nil) {
			nextpc();
			p->as = AMOVW;
			naddr(nodconst(0), &p->from);
			naddr(lo, &p->to);
		}
		if(hi != nil) {
			nextpc();
			p->as = AMOVW;
			naddr(nodconst(0), &p->from);
			naddr(hi, &p->to);
		}
		return;
	}
	nextpc();
	p->as = AMULU;
	if(sign)
		p->as = AMUL;
	naddr(f1, &p->from);
	raddr(f2, p);
	if(lo != nil) {
		nextpc();
		p->as = AMOVW;
		p->from.type = D_LO;
		naddr(lo, &p->to);
	}
	if(hi != nil) {
		nextpc();
		p->as = AMOVW;
		p->from.type = D_HI;
		naddr(hi, &p->to);
	}
}

void
gins(int a, Node *f, Node *t)
{

	nextpc();
	p->as = a;
	if(f != Z)
		naddr(f, &p->from);
	if(t != Z)
		naddr(t, &p->to);
	if(debug['g'])
		print("%P\n", p);
}

void
gopcode(int o, Node *f1, Node *f2, Node *t)
{
	int a, et;
	Addr ta;
	Prog *p1;

	et = TLONG;
	if(f1 != Z && f1->type != T) {
		if(f1->op == OCONST && t != Z && t->type != T)
			et = t->type->etype;
		else
			et = f1->type->etype;
	}

	if(typev[et] || t != Z && t->type != T && typev[t->type->etype])
	if(o != OFUNC) {
		gopcode64(o, f1, f2, t);
		return;
	}

	a = AGOK;
	switch(o) {
	case OAS:
		gmove(f1, t);
		return;

	case OASADD:
	case OADD:
		a = AADDU;
		if(et == TFLOAT)
			a = AADDF;
		else
		if(et == TDOUBLE)
			a = AADDD;
		break;

	case OASSUB:
	case OSUB:
		a = ASUBU;
		if(et == TFLOAT)
			a = ASUBF;
		else
		if(et == TDOUBLE)
			a = ASUBD;
		break;

	case OASOR:
	case OOR:
		a = AOR;
		break;

	case OCOM:
		a = ANOR;
		break;

	case OASAND:
	case OAND:
		a = AAND;
		break;

	case OASXOR:
	case OXOR:
		a = AXOR;
		break;

	case OASLSHR:
	case OLSHR:
		a = ASRL;
		break;

	case OASASHR:
	case OASHR:
		a = ASRA;
		break;

	case OASASHL:
	case OASHL:
		a = ASLL;
		break;

	case OFUNC:
		a = AJAL;
		break;

	case OCOND:
		a = ASGTU;
		break;

	case OCOMMA:
		a = ASGT;
		break;

	case OASMUL:
	case OMUL:
		if(et == TFLOAT) {
			a = AMULF;
			break;
		} else
		if(et == TDOUBLE) {
			a = AMULD;
			break;
		}
		a = AMUL;
		goto muldiv;

	case OASDIV:
	case ODIV:
		if(et == TFLOAT) {
			a = ADIVF;
			break;
		} else
		if(et == TDOUBLE) {
			a = ADIVD;
			break;
		}
		a = ADIV;
		goto muldiv;

	case OASMOD:
	case OMOD:
		a = ADIV;
		o = OMOD;
		goto muldiv;

	case OASLMUL:
	case OLMUL:
		a = AMULU;
		goto muldiv;

	case OASLMOD:
	case OLMOD:
		a = ADIVU;
		o = OMOD;
		goto muldiv;

	case OASLDIV:
	case OLDIV:
		a = ADIVU;
		goto muldiv;

	muldiv:
		nextpc();
		naddr(f1, &p->from);
		if(f2 == Z)
			raddr(t, p);
		else
			raddr(f2, p);
		p->as = a;
		if(debug['g'])
			print("%P\n", p);

		nextpc();
		p->as = AMOVW;
		a = D_LO;
		if(o == OMOD)
			a = D_HI;
		p->from.type = a;
		naddr(t, &p->to);
		if(debug['g'])
			print("%P\n", p);
		return;

	case OEQ:
		if(!typefd[et]) {
			a = ABEQ;
			break;
		}

	case ONE:
		if(!typefd[et]) {
			a = ABNE;
			break;
		}

	case OLT:
	case OLE:
	case OGE:
	case OGT:
		if(typefd[et]) {
			nextpc();
			if(et == TFLOAT) {
				a = ACMPGTF;
				if(o==OEQ || o==ONE)
					a = ACMPEQF;
				else if(o==OGE || o==OLE)
					a = ACMPGEF;
			} else {
				a = ACMPGTD;
				if(o==OEQ || o==ONE)
					a = ACMPEQD;
				else if(o==OGE || o==OLE)
					a = ACMPGED;
			}
			p->as = a;
			if(o == OLT || o == OLE) {
				naddr(f2, &p->from);
				raddr(f1, p);
			} else {
				naddr(f1, &p->from);
				raddr(f2, p);
			}
			if(debug['g'])
				print("%P\n", p);
			nextpc();
			a = ABFPT;
			if(o == ONE)
				a = ABFPF;
			p->as = a;
			if(debug['g'])
				print("%P\n", p);
			return;
		}
	case OLO:
	case OLS:
	case OHI:
	case OHS:
		if(sconst(f1) && f1->vconst == 0) {
			o = invrel[relindex(o)];
			f1 = f2;
			f2 = nodconst(0);
		}
		if(sconst(f2) && f2->vconst == 0) {
			switch(o) {
			case OLT:
				a = ABLTZ;
				break;
			case OLE:
				a = ABLEZ;
				break;
			case OGE:
				a = ABGEZ;
				break;
			case OGT:
				a = ABGTZ;
				break;
			case OLS:
			case OHS:
				a = ABEQ;
				break;
			case OHI:
				a = ABNE;
				break;
			case OLO:
				gbranch(OGOTO);
				p1 = p;
				gbranch(OGOTO);
				patch(p1, pc);
				return;
			}
			f2 = Z;
			break;
		}

		nextpc();
		a = ASGT;
		if(o == OLO || o == OLS || o == OHS || o == OHI)
			a = ASGTU;
		p->as = a;

		if(o == OLE || o == OGT || o == OLS || o == OHI) {
			naddr(f1, &p->from);
			raddr(f2, p);
		} else {
			naddr(f2, &p->from);
			raddr(f1, p);
		}
		naddr(&regnode, &p->to);
		p->to.reg = tmpreg();
		if(debug['g'])
			print("%P\n", p);

		nextpc();
		naddr(&regnode, &p->from);
		p->from.reg = tmpreg();
		a = ABEQ;
		if(o == OLT || o == OGT || o == OLO || o == OHI)
			a = ABNE;
		p->as = a;
		if(debug['g'])
			print("%P\n", p);
		return;
	}
	if(a == AGOK)
		diag(Z, "bad in gopcode %O", o);
	nextpc();
	p->as = a;
	if(f1 != Z)
		naddr(f1, &p->from);
	if(f2 != Z) {
		naddr(f2, &ta);
		p->reg = ta.reg;
		if(ta.type == D_CONST && ta.offset == 0)
			p->reg = REGZERO;
	}
	if(t != Z)
		naddr(t, &p->to);
	if(debug['g'])
		print("%P\n", p);
}

static void
gopconst64(int o, Node *f1, Node *f2, Node *t)
{
	uint32 lo, hi;
	Node nod;

	if(f2 == Z)
		f2 = t;
	lo = f1->vconst & MASK(32);
	hi = (f1->vconst>>32) & MASK(32);

	switch(o) {
	default:
		prtree(f1, "f1");
		prtree(f2, "f2");
		sysfatal("unexpected op: %O in gopconst64", o);
		return;

	case OADD:
	case OASADD:
		if(lo == 0 && hi == 0) {
			gmove(f2->right, t->right);
			gmove(f2->left, t->left);
		} else if(lo == 0) {
			gmove(f2->right, t->right);
			gopcode(o, nodconst(hi), f2->left, t->left);
		} else {
			regalloc(&nod, &regnode, Z);
			gopcode(OADD, nodconst(lo), f2->right, t->right);
			gopcode(OADD, nodconst(hi), f2->left, t->left);
			gopcode(OCOND, f2->right, t->right, &nod);
			gopcode(OADD, &nod, Z, t->left);
			regfree(&nod);
		}
		return;

	case OAND:
	case OASAND:
		if(lo == 0)
			gmove(nodconst(0), t->right);
		else
			gopcode(o, nodconst(lo), f2->right, t->right);
		if(hi == 0)
			gmove(nodconst(0), t->left);
		else
			gopcode(o, nodconst(hi), f2->left, t->left);
		return;

	case OOR:
	case OASOR:
	case OXOR:
	case OASXOR:
		if(lo & 0xffff)
			gopcode(o, nodconst(lo & 0xffff), f2->right, t->right);
		if((lo>>16) != 0)
			gopcode(o, nodconst(lo & 0xffff0000ul), f2->right, t->right);
		if(hi & 0xffff)
			gopcode(o, nodconst(hi & 0xffff), f2->left, t->left);
		if((hi>>16) != 0)
			gopcode(o, nodconst(hi & 0xffff0000ul), f2->left, t->left);
		return;

	case OASHL:
	case OASASHL:
		if(f1->vconst >= 64) {
			gmove(nodconst(0), t->left);
			gmove(nodconst(0), t->right);
		} else if(f1->vconst == 32) {
			gmove(f2->right, t->left);
			gmove(nodconst(0), t->right);
		} else if(f1->vconst > 32) {
			gopcode(o, nodconst(f1->vconst-32), f2->right, t->left);
			gmove(nodconst(0), t->right);
		} else if(f1->vconst > 0) {
			gopcode(o, nodconst(f1->vconst), f2->left, t->left);
			regalloc(&nod, &regnode, Z);
			gopcode(OLSHR, nodconst(32-f1->vconst), f2->right, &nod);
			gopcode(o, nodconst(f1->vconst), f2->right, t->right);
			gopcode(OOR, &nod, Z, t->left);
			regfree(&nod);
		}
		return;

	case OASHR:
	case OASASHR:
		if(f1->vconst >= 64) {
			gopcode(o, nodconst(31), f2->left, t->right);
			gopcode(o, nodconst(31), f2->left, t->left);
		} else if(f1->vconst == 32) {
			gmove(f2->left, t->right);
			gopcode(o, nodconst(31), f2->left, t->left);
		} else if(f1->vconst > 32) {
			gopcode(o, nodconst(f1->vconst-32), f2->left, t->right);
			gopcode(o, nodconst(31), f2->left, t->left);
		} else if(f1->vconst > 0)
			goto shr;
		return;

	case OLSHR:
	case OASLSHR:
		if(f1->vconst >= 64) {
			gmove(nodconst(0), t->left);
			gmove(nodconst(0), t->right);
		} else if(f1->vconst == 32) {
			gmove(f2->left, t->right);
			gmove(nodconst(0), t->left);
		} else if(f1->vconst > 32) {
			gopcode(o, nodconst(f1->vconst-32), f2->left, t->right);
			gmove(nodconst(0), t->left);
		} else if(f1->vconst > 0) {
	shr:
			gopcode(OLSHR, nodconst(f1->vconst), f2->right, t->right);
			regalloc(&nod, &regnode, Z);
			gopcode(OASHL, nodconst(32-f1->vconst), f2->left, &nod);
			gopcode(o, nodconst(f1->vconst), f2->left, t->left);
			gopcode(OOR, &nod, Z, t->right);
			regfree(&nod);
		}
		return;
	}
}

static void
gopcode64(int o, Node *f1, Node *f2, Node *t)
{
	Node nod, nod1, nod2;
	Node *nx;
	Prog *p1;

	if(o == OAS) {
		if(f2 != Z) {
			prtree(f2, "gopcode64:AS:f2");
			sysfatal("bad f2 in gopcode64 AS");
		}
		gmove(f1, t);
		return;
	}
	if(t->op != OREGPAIR || f2 != Z && f2->op != OREGPAIR) {
		diag(Z, "bad f2/dest in gopcode64 %O", o);
		prtree(f1, "f1");
		prtree(f2, "f2");
		return;
	}
	if(f1->op != OCONST)
	if(typev[f1->type->etype] && f1->op != OREGPAIR
	|| !typev[f1->type->etype] && f1->op != OREGISTER) {
		diag(Z, "bad f1[%O] in gopcode64 %O", f1->op, o);
		return;
	}

	if(f1->op == OCONST) {
		if(o == OSUB || o == OASSUB) {
			f1->vconst = -f1->vconst;
			o = OADD;
		}
			gopconst64(o, f1, f2, t);
			return;
	}


	switch(o) {
	case OASADD:
	case OADD:
		if(f2 == Z)
			f2 = t;
		regalloc(&nod, &regnode, Z);
		gopcode(OADD, f1->right, f2->right, t->right);
		gopcode(OADD, f1->left, f2->left, t->left);
		gopcode(OCOND, f1->right, t->right, &nod);
		gopcode(OADD, &nod, Z, t->left);
		regfree(&nod);
		return;

	case OASSUB:
	case OSUB:
		if(f2 == Z)
			f2 = t;
		regalloc(&nod, &regnode, Z);
		gopcode(OCOND, f1->right, f2->right, &nod);
		gopcode(OSUB, f1->right, f2->right, t->right);
		gopcode(OSUB, f1->left, f2->left, t->left);
		gopcode(OSUB, &nod, Z, t->left);
		regfree(&nod);
		return;

	case OASAND:
	case OAND:
	case OASOR:
	case OOR:
	case OASXOR:
	case OXOR:
		if(f2 == Z)
			f2 = nodconst(0);
		gopcode(o, f1->right, f2->right, t->right);
		gopcode(o, f1->left, f2->left, t->left);
		return;

	case OASLSHR:
	case OLSHR:
	case OASASHR:
	case OASHR:
		if(f2 == Z)
			f2 = t;
		regalloc(&nod, &regnode, Z);
		regalloc(&nod1, &regnode, Z);
		if(f1->op == OREGPAIR) {
			nx = f1->right;
		} else
			nx = f1;

		gopcode(OAND, nodconst(0x20), nx, &nod);
		gopcode(OEQ, &nod, nodconst(0), Z);
		p1 = p;
		gopcode(o, nx, f2->left, t->right);
		if(o == OASASHR || o == OASHR)
			gopcode(OASHR, nodconst(31), f2->left, t->left);
		else
			gmove(nodconst(0), t->left);
		gbranch(OGOTO);
		patch(p1, pc);
		p1 = p;

		gopcode(OASHL, nodconst(1), f2->left, &nod);
		gopcode(OCOM, nx, nodconst(0), &nod1);
		gopcode(OLSHR, nx, f2->right, t->right);
		gopcode(OASHL, &nod1, Z, &nod);
		gopcode(o, nx, f2->left, t->left);
		gopcode(OOR, &nod, Z, t->right);
		patch(p1, pc);

		regfree(&nod);
		regfree(&nod1);
		return;

	case OASASHL:
	case OASHL:
		if(f2 == Z)
			f2 = t;
		regalloc(&nod, &regnode, Z);
		regalloc(&nod1, &regnode, Z);
		if(f1->op == OREGPAIR) {
			nx = f1->right;
		} else
			nx = f1;

		gopcode(OAND, nodconst(0x20), nx, &nod);
		gopcode(OEQ, &nod, nodconst(0), Z);
		p1 = p;
		gmove(nodconst(0), t->right);
		gopcode(o, nx, f2->right, t->left);
		gbranch(OGOTO);
		patch(p1, pc);
		p1 = p;

		gopcode(OLSHR, nodconst(1), f2->right, &nod);
		gopcode(OCOM, nx, nodconst(0), &nod1);
		gopcode(o, nx, f2->left, t->left);
		gopcode(OLSHR, &nod1, Z, &nod);
		gopcode(o, nx, f2->right, t->right);
		gopcode(OOR, &nod, Z, t->left);
		patch(p1, pc);

		regfree(&nod);
		regfree(&nod1);
		return;

	case OASLMUL:
	case OLMUL:
	case OASMUL:
	case OMUL:
		if(f2 == Z)
			f2 = t;
		if(f1->right->reg == 0 && f1->left->reg == 0
		|| f2->right->reg == 0 && f2->left->reg == 0) {
			gmove(nodconst(0), t->left);
			gmove(nodconst(0), t->right);
			return;
		}
		if(f1->left->reg == 0 && f2->left->reg == 0) {
			gmul(f1->right, f2->right, t->right, t->left, 1);
			return;
		}
		regalloc(&nod, &regnode, Z);
		regalloc(&nod1, &regnode, Z);
		regalloc(&nod2, &regnode, Z);
		gmul(f1->right, f2->right, &nod, &nod1, 0);
		gmul(f2->right, f1->left, &nod2, nil, 1);
		gins(AADDU, &nod2, &nod1);
		gmul(f1->right, f2->left, &nod2, nil, 1);
		gins(AADDU, &nod2, &nod1);
		regfree(&nod2);
		gmove(&nod, t->right);
		gmove(&nod1, t->left);
		regfree(&nod);
		regfree(&nod1);
		return;

	case OCOM:
		gopcode(OCOM, t->right, nodconst(0), t->right);
		gopcode(OCOM, t->left, nodconst(0), t->left);
		return;

	case ONEG:
		regalloc(&nod, &regnode, Z);
		gopcode(OCOND, t->right, nodconst(0), &nod);
		gopcode(OSUB, t->right, nodconst(0), t->right);
		gopcode(OSUB, t->left, nodconst(0), t->left);
		gopcode(OSUB, &nod, Z, t->left);
		regfree(&nod);
		return;
	}
	sysfatal("bad in gopcode64 %O", o);
}

int
samaddr(Node *f, Node *t)
{

	if(f->op != t->op)
		return 0;
	switch(f->op) {
	case OREGISTER:
		if(f->reg != t->reg)
			break;
		return 1;

	case OREGPAIR:
		return samaddr(f->left, t->left) && samaddr(f->right, t->right);
	}
	return 0;
}

void
gbranch(int o)
{
	int a;

	a = AGOK;
	switch(o) {
	case ORETURN:
		a = ARET;
		break;
	case OGOTO:
		a = AJMP;
		break;
	}
	nextpc();
	if(a == AGOK) {
		diag(Z, "bad in gbranch %O",  o);
		nextpc();
	}
	p->as = a;
}

void
patch(Prog *op, int32 pc)
{

	op->to.offset = pc;
	op->to.type = D_BRANCH;
}

void
gpseudo(int a, Sym *s, Node *n)
{

	nextpc();
	p->as = a;
	p->from.type = D_OREG;
	p->from.sym = linksym(s);
	p->from.name = D_EXTERN;

	switch(a) {
	case ATEXT:
		p->reg = textflag;
		textflag = 0;
		break;
	case AGLOBL:
		p->reg = s->dataflag;
		break;
	}

	if(s->class == CSTATIC)
		p->from.name = D_STATIC;
	naddr(n, &p->to);
	if(a == ADATA || a == AGLOBL)
		pc--;
}

void
gpcdata(int index, int value)
{
	Node n1;

	n1 = *nodconst(index);
	gins(APCDATA, &n1, nodconst(value));
}

void
gprefetch(Node *n)
{
	USED(n);
}

int
sconst(Node *n)
{
	vlong vv;

	if(n->op == OCONST) {
		if(!typefd[n->type->etype]) {
			vv = n->vconst;
			if(vv >= (vlong)(-32766) && vv < (vlong)32766)
				return 1;
		}
	}
	return 0;
}

int
sval(int32 v)
{
	if(v >= -32766L && v < 32766L)
		return 1;
	return 0;
}

int32
exreg(Type *t)
{
	int32 o;

	if(typechlp[t->etype]) {
		if(exregoffset < REGEXT)
			return 0;
		o = exregoffset;
		exregoffset--;
		return o;
	}
	if(typefd[t->etype]) {
		if(exfregoffset <= 16)
			return 0;
		o = exfregoffset + NREG;
		exfregoffset--;
		return o;
	}
	return 0;
}

schar	ewidth[NTYPE] =
{
	-1,
	SZ_CHAR,
	SZ_CHAR,
	SZ_SHORT,
	SZ_SHORT,
	SZ_INT,
	SZ_INT,
	SZ_LONG,
	SZ_LONG,
	SZ_VLONG,
	SZ_VLONG,
	SZ_FLOAT,
	SZ_DOUBLE,
	SZ_IND,
	0,
	-1,
	0,
	-1,
	-1,
	SZ_INT,
};

int32	ncast[NTYPE] =
{
	0,
	BCHAR|BUCHAR,
	BCHAR|BUCHAR,
	BSHORT|BUSHORT,
	BSHORT|BUSHORT,
	BINT|BUINT|BLONG|BULONG|BIND,
	BINT|BUINT|BLONG|BULONG|BIND,
	BINT|BUINT|BLONG|BULONG|BIND,
	BINT|BUINT|BLONG|BULONG|BIND,
	BVLONG|BUVLONG,
	BVLONG|BUVLONG,
	BFLOAT,
	BDOUBLE,
	BLONG|BULONG|BIND,
	0,
	0,
	0,
	BSTRUCT,
	BUNION,
	0,
};
