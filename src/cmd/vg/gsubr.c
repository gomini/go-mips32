
#include <u.h>
#include <libc.h>
#include "gg.h"
#include "../../runtime/funcdata.h"

long unmappedzero = 4096;

void
clearp(Prog *p)
{
	*p = zprog;
	p->as = AEND;
	p->pc = pcloc;
	pcloc++;
}

static int ddumped;
static Prog *dfirst;
static Prog *dpc;

static Prog*
prog(int as)
{
	Prog *p;

	if(as == ADATA || as == AGLOBL) {
		if(ddumped)
			fatal("already dumped data");
		if(dpc == nil) {
			dpc = mal(sizeof(*dpc));
			dfirst = dpc;
		}
		p = dpc;
		dpc = mal(sizeof(*dpc));
		p->link = dpc;
		p->reg = 0;
	} else {
		p = pc;
		pc = mal(sizeof(*pc));
		clearp(pc);
		p->link = pc;
	}

	if(lineno == 0) {
		if(debug['K'])
			warn("prog: line 0");
	}

	p->as = as;
	p->lineno = lineno;
	return p;
}

void
dumpdata(void)
{
	ddumped = 1;
	if(dfirst == nil)
		return;
	newplist();
	*pc = *dfirst;
	pc = dpc;
	clearp(pc);
}


Prog*
tbranch(int as, Node *s, Node *t, int likely)
{
	Prog *p;

	if(s == N)
		fatal("tbranch: s is N");
	
	if(s->op != OREGISTER)
		fatal("tbranch: bad s: %O", s->op);

	if(t == N)
		return zbranch(as, s, likely);
	if(t->op == OLITERAL && mpgetfix(t->val.u.xval) == 0)
		return zbranch(as, s, likely);
	
	if(t->op != OREGISTER)
		fatal("tbranch: bad t: %O", t->op);

	if(as != ABEQ && as != ABNE)
		fatal("tbranch: bad instr %A", as);

	p = prog(as);
	naddr(s, &p->from, 1);
	raddr(t, p);
	p->to.type = D_BRANCH;
	p->to.u.branch = P;
	return p;
}

Prog*
zbranch(int as, Node *s, int likely)
{
	Prog *p;

	USED(likely);

	if(s != N && s->op != OREGISTER)
		fatal("zbranch: bad %O", s->op);

	p = prog(as);
	if(s != N)
		naddr(s, &p->from, 1);
	p->to.type = D_BRANCH;
	p->to.u.branch = P;

	switch(as) {
	case AJMP:
	case AJAL:
		break;

	case ABEQ:
	case ABNE:
		p->reg = 0;
		break;

	case ABLTZ:
	case ABGEZ:
	case ABLEZ:
	case ABGTZ:

	case ABLTZAL:
	case ABGEZAL:
		break;

	case ABFPT:
	case ABFPF:
		if(s == N)
			break;
	default:
		fatal("zbranch: bad as: %A", as);
	}

	return p;
}

Prog*
gjmp(Prog *to)
{
	Prog *p;

	p = zbranch(AJMP, N, 0);
	if(to != P)
		patch(p, to);
	return p;
}


void
patch(Prog *p, Prog *to)
{
	if(p->to.type != D_BRANCH) {
		fatal("patch: not a branch");
	}
	p->to.u.branch = to;
	p->to.offset = to->pc;
}

Prog*
unpatch(Prog *p)
{
	Prog *q;

	if(p->to.type != D_BRANCH)
		fatal("unpatch: not a branch");
	q = p->to.u.branch;
	p->to.u.branch = P;
	p->to.offset = 0;
	return q;
}

Plist*
newplist(void)
{
	Plist *pl;

	pl = linknewplist(ctxt);

	pc = mal(sizeof(*pc));
	clearp(pc);
	pl->firstpc = pc;

	return pl;
}

void
gused(Node *n)
{
	gins(ANOP, n, N);
}

void
ggloblnod(Node *nam)
{
	Prog *p;

	p = gins(AGLOBL, nam, N);
	p->lineno = nam->lineno;
	p->from.sym->gotype = linksym(ngotype(nam));
	p->to.sym = nil;
	p->to.type = D_CONST;
	p->to.offset = nam->type->width;
	if(nam->readonly)
		p->reg = RODATA;
	if(nam->type != T && !haspointers(nam->type))
		p->reg |= NOPTR;
}

void
ggloblsym(Sym *s, int32 width, int8 flags)
{
	Prog *p;

	p = gins(AGLOBL, N, N);
	p->from.type = D_OREG;
	p->from.name = D_EXTERN;
	p->from.sym = linksym(s);
	p->to.type = D_CONST;
	p->to.name = D_NONE;
	p->to.offset = width;
	p->reg = flags;
}

void
gtrack(Sym *s)
{
	Prog *p;
	
	p = gins(AUSEFIELD, N, N);
	p->from.type = D_OREG;
	p->from.name = D_EXTERN;
	p->from.sym = linksym(s);
}

int
isfat(Type *t)
{
	if(t != T)
	switch(t->etype) {
	case TSTRUCT:
	case TARRAY:
	case TSTRING:
	case TINTER:
		return 1;
	}
	return 0;
}

void
afunclit(Addr *a, Node *n)
{
	if(a->type == D_CONST && a->name == D_EXTERN || a->type == D_REG) {
		a->type = D_OREG;
		if(n->op == ONAME)
			a->sym = linksym(n->sym);
	}
}

static	int	resvd[] =
{
	REGZERO,
	REGTMP,
	REGSP,
	REGLINK,
	REGG,
	26,
	27,
	FREGZERO + NREG,
	FREGHALF + NREG,
	FREGONE + NREG,
	FREGTWO + NREG,
};

static	uintptr	regpc[nelem(reg)];

void
ginit(void)
{
	int i;

	for(i=0; i<nelem(reg); i++)
		reg[i] = 0;
	for(i=NREG+1; i<nelem(reg); i+=2)
		reg[i] = 1;
	for(i=0; i<nelem(resvd); i++)
		reg[resvd[i]]++;
}

void
gclean(void)
{
	int i;

	for(i=0; i<nelem(resvd); i++)
		reg[resvd[i]]--;
	for(i=NREG+1; i<nelem(reg); i+=2)
		reg[i]--;

	for(i=0; i<nelem(reg); i++)
		if(reg[i])
			yyerror("reg %R left allocated\n%p\n", i, regpc[i]);
}

int32
anyregalloc(void)
{
	int i, j;

	for(i=0; i<nelem(reg); i++) {
		if(reg[i] == 0)
			goto ok;
		for(j=0; j<nelem(resvd); j++)
			if(resvd[j] == i)
				goto ok;
		return 1;
	ok:;
	}
	return 0;
}

void
regalloc(Node *n, Type *t, Node *o)
{
	int i, et, fixfree, fltfree;

	if(t == T)
		fatal("regalloc: t nil");
	if(is64(t))
		fatal("regalloc: 64 bit type %T");

	if(debug['r']) {
		fixfree = 0;
		fltfree = 0;
		for(i=0; i < nelem(reg); i++)
			if(reg[i] == 0) {
				if(i < NREG)
					fixfree++;
				else
					fltfree++;
			}
		print("regalloc fix %d float %d\n", fixfree, fltfree);
	}

	et = simtype[t->etype];
	switch(et) {
	case TINT8:
	case TUINT8:
	case TINT16:
	case TUINT16:
	case TINT32:
	case TUINT32:
	case TPTR32:
	case TBOOL:
		if(o != N && o->op == OREGISTER) {
			i = o->val.u.reg;
			if(i>0 && i<NREG)
				goto out;
		}
		for(i=1; i<=REGMAX; i++)
			if(reg[i] == 0) {
				regpc[i] = (uintptr)getcallerpc(&n);
				goto out;
			}
		flusherrors();
		for(i=0; i<NREG; i++)
			print("R%d %p\n", i, regpc[i]);
		fatal("out of fixed registers");

	case TFLOAT32:
	case TFLOAT64:
		if(o != N && o->op == OREGISTER) {
			i = o->val.u.reg;
			if(i >= NREG && i <nelem(reg))
				goto out;
		}
		for(i=NREG; i<nelem(reg); i+=2)
			if(reg[i] == 0) {
				regpc[i] = (uintptr)getcallerpc(&n);
				goto out;
			}
		flusherrors();
		for(i=NREG; i<nelem(reg); i+=2)
			print("F%d %p\n", i, regpc[i]);
		fatal("out of floating point registers");

	case TCOMPLEX64:
	case TCOMPLEX128:
		tempname(n, t);
		return;
	}
	fatal("regalloc: unknown type %T", t);
	return;

out:
	reg[i]++;
	nodreg(n, t, i);
}

void
regfree(Node *n)
{
	int i;

	if(n->op == ONAME)
		return;
	if(n->op != OREGISTER && n->op != OINDREG) {
		print("regfree: %O\n", n->op);
		dump("\nregfree", n);
		fatal("regfree: not a register");
	}
	i = n->val.u.reg;
	if(i == REGSP)
		return;
	if(i <= 0 || i >= nelem(reg) || i >= nelem(regpc)) {
		fatal("regfree: reg out of range, R%d", i);
	}
	if(reg[i] <= 0)
		fatal("regfree: reg %R not allocated", i);
	reg[i]--;
	if(reg[i] == 0)
		regpc[i] = 0;
}

void
nodreg(Node *n, Type *t, int r)
{
	if(t == T)
		fatal("nodreg: t nil");

	memset(n, 0, sizeof(*n));
	n->op = OREGISTER;
	n->addable = 1;
	ullmancalc(n);
	n->val.u.reg = r;
	n->type = t;
}

void
nodindreg(Node *n, Type *t, int r)
{
	nodreg(n, t, r);
	n->op = OINDREG;
}

Node*
nodarg(Type *t, int fp)
{
	Node *n;
	NodeList *l;
	Type *first;
	Iter savet;

	if(t->etype == TSTRUCT && t->funarg) {
		n = nod(ONAME, N, N);
		n->sym = lookup(".args");
		n->type = t;
		first = structfirst(&savet, &t);
		if(first == nil)
			fatal("nodarg: bad struct");
		if(first->width == BADWIDTH)
			fatal("nodarg: offset not computed for %T", t);
		n->xoffset = first->width;
		n->addable = 1;
		goto fp;
	}

	if(t->etype != TFIELD)
		fatal("nodarg: not field %T", t);

	if(fp == 1) {
		for(l=curfn->dcl; l; l=l->next) {
			n = l->n;
			if((n->class == PPARAM || n->class == PPARAMOUT) && !isblanksym(t->sym) && n->sym == t->sym)
				return n;
		}
	}

	n = nod(ONAME, N, N);
	n->type = t->type;
	n->sym = t->sym;
	if(t->width == BADWIDTH)
		fatal("nodarg: offset not computed for %T", t);
	n->xoffset = t->width;
	n->addable = 1;
	n->orig = t->nname;

fp:
	if(isblank(n))
		n->sym = lookup("__");

	switch(fp) {
	default:
		fatal("nodarg %T %d", t, fp);

	case 0:
		n->op = OINDREG;
		n->val.u.reg = REGSP;
		n->xoffset += 4;
		break;

	case 1:
		n->class = PPARAM;
		break;
	}
	n->typecheck = 1;
	return n;
}

Node*
ncon(uint32 i)
{
	static Node n;

	if(n.type == T)
		nodconst(&n, types[TUINT32], 0);
	mpmovecfix(n.val.u.xval, i);
	return &n;
}

Node*
nscon(int32 i)
{
	static Node n;

	if(n.type == T)
		nodconst(&n, types[TINT32], 0);
	mpmovecfix(n.val.u.xval, i);
	return &n;
}

static Node sclean[10];
static int nsclean;

int
split64(Node *n, Node *lo, Node *hi)
{
	Node n1;
	int64 i;

	if(!is64(n->type))
		fatal("split64 %T", n->type);

	if(nsclean >= nelem(sclean)) {
		fatal("split64 clean");
	}
	sclean[nsclean].op = OEMPTY;
	nsclean++;
	switch(n->op) {
	default:
		if(!dotaddable(n, &n1)) {
			igen(n, &n1, N);
			sclean[nsclean-1] = n1;
		}
		n = &n1;
		goto common;
	case ONAME:
		if(n->class == PPARAMREF) {
			fatal("split64: unaddressable node:\n%N", n);
		}
		goto common;
	case OINDREG:
	common:
		*lo = *n;
		*hi = *n;
		lo->type = types[TUINT32];
		if(n->type->etype == TINT64)
			hi->type = types[TINT32];
		else
			hi->type = types[TUINT32];
		if(isbigendian)
			lo->xoffset += 4;
		else
			hi->xoffset += 4;
		break;

	case OLITERAL:
		convconst(&n1, n->type, &n->val);
		i = mpgetfix(n1.val.u.xval);
		if(i == 0) {
			nodreg(lo, types[TUINT32], 0);
			if(n->type->etype == TINT64)
				nodreg(hi, types[TINT32], 0);
			else
				nodreg(hi, types[TUINT32], 0);
			return 0;
		}
		nodconst(lo, types[TUINT32], (uint32)i);
		i >>= 32;
		if(n->type->etype == TINT64)
			nodconst(hi, types[TINT32], (int32)i);
		else
			nodconst(hi, types[TUINT32], (uint32)i);
		break;
	}
	return 1;
}

void
splitclean(void)
{
	if(nsclean <= 0)
		fatal("splitclean");
	nsclean--;
	if(sclean[nsclean].op != OEMPTY)
		regfree(&sclean[nsclean]);
}

static int
ismem(Node *n)
{
	switch(n->op) {
	case OITAB:
	case OSPTR:
	case OLEN:
	case OCAP:
	case OINDREG:
	case ONAME:
	case OPARAM:
	case OCLOSUREVAR:
	case OADDR:
		return 1;
	}
	return 0;
}

static void
fop(int as, int f1, int f2, Type *ft, Node *t)
{
	Node n1, n2, n3;

	nodreg(&n1, ft, NREG+f1);
	nodreg(&n2, ft, NREG+f2);
	regalloc(&n3, ft, t);
	gins3(as, &n1, &n2, &n3);
	gmove(&n3, t);
	regfree(&n3);
}

static int
fmoveconst(Node *f, Node *t)
{
	Node con, nod;
	double d;
	int ft, a;

	ft = simsimtype(f->type);
	
	convconst(&con, types[TFLOAT64], &f->val);
	d = mpgetflt(f->val.u.fval);

	if(ft == TFLOAT64) {
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
			regalloc(&nod, f->type, t);
			fop(ASUBD, FREGHALF, FREGZERO, f->type, &nod);
		} else if(d == -1.0) {
			regalloc(&nod, f->type, t);
			fop(ASUBD, FREGONE, FREGZERO, f->type, &nod);
		} else if(d == -2.0) {
			regalloc(&nod, f->type, t);
			fop(ASUBD, FREGTWO, FREGZERO, f->type, &nod);
		} else if(d == 1.5) {
			regalloc(&nod, f->type, t);
			fop(AADDD, FREGONE, FREGHALF, f->type, &nod);
		} else if(d == 2.5) {
			regalloc(&nod, f->type, t);
			fop(AADDD, FREGTWO, FREGHALF, f->type, &nod);
		} else if(d == 3.0) {
			regalloc(&nod, f->type, t);
			fop(AADDD, FREGTWO, FREGONE, f->type, &nod);
		} else
			return 0;

		gmove(&nod, t);
		regfree(&nod);
		return 1;
	}
	if(ft == TFLOAT32 && d == 0.) {
		a = FREGZERO;
	ffreg:
		nodreg(&nod, f->type, NREG+a);
		gmove(&nod, t);
		return 1;
	}
	return 0;
}

static void
fadd2e32(Node *n)
{
	Prog *p;
	int a, a1, a2;
	Node tmp, fc;
	Mpflt f;
	double d;

	d = 4294967296.;
	mpmovecflt(&f, d);

	if(simsimtype(n->type) == TFLOAT32) {
		a = ACMPGEF;
		a1 = AMOVF;
		a2 = AADDF;
		d = mpgetflt32(&f);
		mpmovecflt(&f, d);
	} else {
		a = ACMPGED;
		a1 = AMOVD;
		a2 = AADDD;
	}

	nodfconst(&fc, n->type, &f);

	p = gins(a, n, N);
	p->reg = FREGZERO;
	
	p = zbranch(ABFPT, N, 0);

	regalloc(&tmp, n->type, N);
	gins(a1, &fc, &tmp);
	gins(a2, &tmp, n);
	patch(p, pc);
	regfree(&tmp);
}

static void
flt2u(Node *f, Node *t)
{
	int ft, a, a1, a2, a3;
	Node r1, e31;
	Prog *p, *p1;
	double d;
	Mpflt val;

	d = 2147483648.;
	mpmovecflt(&val, d);

	regalloc(&r1, f->type, N);

	ft = simsimtype(f->type);
	if(ft == TFLOAT64) {
		a = AMOVD;
		a1 = ACMPGED;
		a2 = ASUBD;
		a3 = AMOVDW;
	} else {
		a = AMOVF;
		a1 = ACMPGEF;
		a2 = ASUBF;
		a3 = AMOVFW;
		d = mpgetflt32(&val);
		mpmovecflt(&val, d);
	}
	nodfconst(&e31, f->type, &val);

	gins(a, &e31, &r1);
	p = gins(a1, f, N);
	p->reg = r1.val.u.reg;
	p = zbranch(ABFPT, N, 0);

	gins(a3, f, &r1);
	gins(AMOVW, &r1, t);

	p1 = gjmp(P);
	patch(p, pc);

	gins3(a2, &r1, f, &r1);
	gins(a3, &r1, &r1);
	gins(AMOVW, &r1, t);
	p = zbranch(ABEQ, t, -1);
	gins3(AOR, nscon(1<<31), t, t);
	patch(p1, pc);
	patch(p, pc);

	regfree(&r1);
}

#define	CASE(a,b)	(((a)<<16)|((b)<<0))

void
gmove(Node *f, Node *t)
{
	int a, a1, ft, tt, spl;
	Node r1, r2, flo, fhi, tlo, thi, con;
	Node f1;

	if(debug['M'])
		print("gmove %N -> %N\n", f, t);

	ft = simsimtype(f->type);
	tt = simsimtype(t->type);

	if(iscomplex[ft] || iscomplex[tt]) {
		complexmove(f, t);
		return;
	}

	if(!is64(f->type) && !is64(t->type) && ismem(f) && ismem(t))
		goto hard;

	if(isfloat[ft] && f->op == OLITERAL)
		if(fmoveconst(f, t))
			return;

	if(f->op == OLITERAL) {
		if(isconst(f, CTINT) && mpgetfix(f->val.u.xval) == 0)
		if(!is64(t->type)) {
			nodreg(&r1, t->type, 0);
			gmove(&r1, t);
			return;
		}
		switch(tt) {
		default:
			convconst(&con, t->type, &f->val);
			break;

		case TINT16:
		case TINT8:
			convconst(&con, types[TINT32], &f->val);
			regalloc(&r1, con.type, t);
			gins(AMOVW, &con, &r1);
			gmove(&r1, t);
			regfree(&r1);
			return;

		case TUINT16:
		case TUINT8:
			convconst(&con, types[TUINT32], &f->val);
			regalloc(&r1, con.type, t);
			gins(AMOVW, &con, &r1);
			gmove(&r1, t);
			regfree(&r1);
			return;
		}

		f = &con;
		ft = simsimtype(con.type);

		if(ismem(t) && !is64(t->type))
			goto hard;
		if(ft == TINT64 || ft == TUINT64)
			goto loadi64;
	}

	if(ismem(f)) {
		switch(ft) {
		case TINT8:
			a = AMOVB;
			break;
		case TUINT8:
			a = AMOVBU;
			break;
		case TINT16:
			a = AMOVH;
			break;
		case TUINT16:
			a = AMOVHU;
			break;
		case TFLOAT32:
			a = AMOVF;
			break;
		case TFLOAT64:
			a = AMOVD;
			break;
		case TINT64:
		case TUINT64:
			goto loadi64;
		case TINT32:
		case TUINT32:
			if(isfloat[tt]) {
				regalloc(&f1, t->type, t);
				gins(AMOVW, f, &f1);
				a = AMOVWD;
				if(tt == TFLOAT32)
					a = AMOVWF;
				gins(a, &f1, &f1);
				if(ft == TUINT32)
					fadd2e32(&f1);
				gmove(&f1, t);
				regfree(&f1);
				return;
			}
			a = AMOVW;
			break;
		default:
			goto fatal;
		}

		if(types[ft]->width <= widthptr && !isfloat[ft]
		&& types[tt]->width == widthptr && !isfloat[tt])
			regalloc(&r1, t->type, t);
		else
			regalloc(&r1, f->type, t);
		gins(a, f, &r1);
		gmove(&r1, t);
		regfree(&r1);
		return;
	}

	if(ismem(t)) {
		switch(tt) {
		case TINT8:
		case TUINT8:
			a = AMOVB;
			break;
		case TINT16:
		case TUINT16:
			a = AMOVH;
			break;
		case TINT32:
		case TUINT32:
			a = AMOVW;
			break;
		case TINT64:
		case TUINT64:
			goto storei64;
		case TFLOAT32:
		case TFLOAT64:
			a = AMOVF;
			if(tt == TFLOAT64)
				a = AMOVD;
			if(ft == TINT32 || ft == TUINT32) {
				regalloc(&r1, t->type, N);
				gins(AMOVW, f, &r1);
				a1 = AMOVWD;
				if(tt == TFLOAT32)
					a1 = AMOVWF;
				gins(a1, &r1, &r1);
				if(ft == TUINT32)
					fadd2e32(&r1);
				gins(a, &r1, t);
				regfree(&r1);
				return;
			}
			break;
		default:
			goto fatal;
		}
		if(ft == tt)
			regalloc(&r1, t->type, f);
		else
			regalloc(&r1, t->type, N);
		gmove(f, &r1);
		gins(a, &r1, t);
		regfree(&r1);
		return;
	}


	switch(CASE(ft, tt)) {
	default:
		goto fatal;

	case CASE(TFLOAT64, TFLOAT64):
		a = AMOVD;
		break;
	case CASE(TFLOAT64, TFLOAT32):
		a = AMOVDF;
		break;
	case CASE(TFLOAT32, TFLOAT64):
		a = AMOVFD;
		break;
	case CASE(TFLOAT32, TFLOAT32):
		a = AMOVF;
		break;
	case CASE(TFLOAT64, TUINT32):
	case CASE(TFLOAT32, TUINT32):
		flt2u(f, t);
		return;

	case CASE(TFLOAT64, TINT32):
	case CASE(TFLOAT64, TINT16):
	case CASE(TFLOAT64, TUINT16):
	case CASE(TFLOAT64, TINT8):
	case CASE(TFLOAT64, TUINT8):
	case CASE(TFLOAT32, TINT32):
	case CASE(TFLOAT32, TINT16):
	case CASE(TFLOAT32, TUINT16):
	case CASE(TFLOAT32, TINT8):
	case CASE(TFLOAT32, TUINT8):
		regalloc(&r1, f->type, N);
		a = AMOVDW;
		if(ft == TFLOAT32)
			a = AMOVFW;
		gins(a, f, &r1);
		gins(AMOVW, &r1, t);
		regfree(&r1);
		a = 0;
		switch(tt) {
		case TINT16:
			a = AMOVH;
			break;
		case TUINT16:
			a = AMOVHU;
			break;
		case TINT8:
			a = AMOVB;
			break;
		case TUINT8:
			a = AMOVBU;
			break;
		}
		if(a)
			gins(a, t, t);
		return;

	case CASE(TINT32, TINT16):
	case CASE(TUINT32, TINT16):
		a = AMOVH;
		break;
	case CASE(TINT32, TUINT16):
	case CASE(TUINT32, TUINT16):
		a = AMOVHU;
		break;
	case CASE(TINT32, TINT8):
	case CASE(TUINT32, TINT8):
	case CASE(TINT16, TINT8):
	case CASE(TUINT16, TINT8):
		a = AMOVB;
		break;
	case CASE(TINT32, TUINT8):
	case CASE(TUINT32, TUINT8):
	case CASE(TINT16, TUINT8):
	case CASE(TUINT16, TUINT8):
		a = AMOVBU;
		break;

	case CASE(TINT32, TINT32):
	case CASE(TINT32, TUINT32):
	case CASE(TUINT32, TINT32):
	case CASE(TUINT32, TUINT32):
	case CASE(TINT16, TINT16):
	case CASE(TUINT16, TUINT16):
	case CASE(TINT8, TINT8):
	case CASE(TUINT8, TUINT8):
		a = AMOVW;
		break;


	case CASE(TUINT16, TINT16):
	case CASE(TINT16, TINT32):
	case CASE(TINT16, TUINT32):
		a = AMOVH;
		break;
	case CASE(TINT16, TUINT16):
	case CASE(TUINT16, TINT32):
	case CASE(TUINT16, TUINT32):
		a = AMOVHU;
		break;

	case CASE(TINT8, TINT32):
	case CASE(TINT8, TUINT32):
	case CASE(TINT8, TINT16):
	case CASE(TINT8, TUINT16):
	case CASE(TUINT8, TINT8):
		a = AMOVB;
		break;
	case CASE(TUINT8, TINT32):
	case CASE(TUINT8, TUINT32):
	case CASE(TUINT8, TINT16):
	case CASE(TUINT8, TUINT16):
	case CASE(TINT8, TUINT8):
		a = AMOVBU;
		break;


	case CASE(TINT32, TFLOAT64):
	case CASE(TINT32, TFLOAT32):
	case CASE(TUINT32, TFLOAT64):
	case CASE(TUINT32, TFLOAT32):

	case CASE(TINT16, TFLOAT64):
	case CASE(TINT16, TFLOAT32):
	case CASE(TUINT16, TFLOAT64):
	case CASE(TUINT16, TFLOAT32):
	case CASE(TINT8, TFLOAT64):
	case CASE(TINT8, TFLOAT32):
	case CASE(TUINT8, TFLOAT64):
	case CASE(TUINT8, TFLOAT32):
		gins(AMOVW, f, t);
		if(tt == TFLOAT64)
			a = AMOVWD;
		else
			a = AMOVWF;
		gins(a, t, t);
		if(ft == TUINT32)
			fadd2e32(t);
		return;
	}

	gins(a, f, t);
	return;

hard:
	regalloc(&r1, t->type, t);
	gmove(f, &r1);
	gmove(&r1, t);
	regfree(&r1);
	return;

loadi64:
	spl = split64(f, &flo, &fhi);
	switch(tt) {
	case TINT8:
	case TINT16:
	case TINT32:
	case TUINT8:
	case TUINT16:
	case TUINT32:
		if(!spl) {
			gmove(&flo, t);
			break;
		}
		regalloc(&r1, t->type, t);
		gmove(&flo, &r1);
		gmove(&r1, t);
		regfree(&r1);
		break;
	case TINT64:
	case TUINT64:
		split64(t, &tlo, &thi);
		if(!spl) {
			gins(AMOVW, &flo, &tlo);
			gins(AMOVW, &flo, &thi);
		} else {
			regalloc(&r1, flo.type, N);
			regalloc(&r2, fhi.type, N);
			gins(AMOVW, &flo, &r1);
			gins(AMOVW, &fhi, &r2);
			gins(AMOVW, &r1, &tlo);
			gins(AMOVW, &r2, &thi);
			regfree(&r1);
			regfree(&r2);
		}
		splitclean();
		break;
	case TFLOAT32:
	case TFLOAT64:
		goto fatal;
	}
	splitclean();
	return;

storei64:
	switch(ft) {
	case TINT8:
	case TINT16:
	case TINT32:
	case TUINT8:
	case TUINT16:
	case TUINT32:
		break;

	case TINT64:
	case TUINT64:
		goto loadi64;

	case TFLOAT32:
	case TFLOAT64:
	default:
		goto fatal;
	}
	if(f->op == OLITERAL && mpgetfix(f->val.u.xval) == 0)
		nodreg(&r1, types[TINT32], 0);
	else {
		if(issigned[f->type->etype])
			regalloc(&r1, types[TINT32], f);
		else
			regalloc(&r1, types[TUINT32], f);
		gmove(f, &r1);
	}
	split64(t, &tlo, &thi);
	gins(AMOVW, &r1, &tlo);
	if(issigned[f->type->etype]) {
		gins3(ASRA, ncon(31), &r1, &r1);
		gins(AMOVW, &r1, &thi);
	} else {
		nodreg(&r2, types[TINT32], 0);
		gins(AMOVW, &r2, &thi);
	}
	splitclean();
	if(r1.val.u.reg != 0)
		regfree(&r1);
	return;

fatal:
	fatal("gmove %N -> %N", f, t);
}

int
samaddr(Node *f, Node *t)
{

	if(f->op != t->op)
		return 0;

	switch(f->op) {
	case OREGISTER:
		if(f->val.u.reg != t->val.u.reg)
			break;
		return 1;
	}
	return 0;
}

Prog*
gins(int as, Node *f, Node *t)
{
	Prog *p;
	Addr af, at;

	if(f != N && f->op == OINDEX)
		fatal("gins OINDEX not implemented");
	if(t != N && t->op == OINDEX)
		fatal("gins OINDEX not implemented");

	if(as == AMOVW || as == AMOVF || as == AMOVD)
	if(f != N && t != N)
	if(samaddr(f, t))
		return P;

	memset(&af, 0, sizeof af);
	memset(&at, 0, sizeof at);
	if(f != N)
		naddr(f, &af, 1);
	if(t != N)
		naddr(t, &at, 1);
	p = prog(as);
	if(f != N)
		p->from = af;
	if(t != N)
		p->to = at;
	if(as == ATEXT)
		p->reg = 0;
	if(debug['g'])
		print("%P\n", p);
	return p;
}

Prog*
gins3(int as, Node *f1, Node *f2, Node *t)
{
	Prog *p;

	p = gins(as, f1, t);
	if(f2 != nil)
		raddr(f2, p);
	return p;
}

void
raddr(Node *n, Prog *p)
{
	Addr a;

	naddr(n, &a, 1);
	if(a.type != D_REG && a.type != D_FREG) {
		if(a.type == D_CONST && a.offset == 0)
			p->reg = 0;
		else {
			if(n)
				fatal("bad in raddr: %O", n->op);
			else
				fatal("bad in raddr: <null>");
			p->reg = NREG;
		}
	} else
		p->reg = a.reg;
}

void
naddr(Node *n, Addr *a, int canemitcode)
{
	Sym *s;

	memset(a, 0, sizeof(*a));
	a->type = D_NONE;
	a->name = D_NONE;
	a->reg = NREG;
	a->gotype = nil;
	a->node = N;
	a->etype = 0;
	if(n == N)
		return;

	if(n->type != T && n->type->etype != TIDEAL) {
		dowidth(n->type);
		a->width = n->type->width;
	}

	switch(n->op) {
	default:
		fatal("naddr: bad %O %D", n->op, a);
		break;

	case OREGISTER:
		if(n->val.u.reg < NREG) {
			a->type = D_REG;
			a->reg = n->val.u.reg;
		} else {
			a->type = D_FREG;
			a->reg = n->val.u.reg - NREG;
		}
		a->sym = nil;
		break;

	case OINDEX:
	case OIND:
		fatal("naddr: %O", n->op);

	case OINDREG:
		a->type = D_OREG;
		a->reg = n->val.u.reg;
		a->sym = linksym(n->sym);
		a->offset = n->xoffset;
		break;

	case OPARAM:
		a->etype = simtype[n->left->type->etype];
		a->width = n->left->type->width;
		a->offset = n->xoffset;
		a->sym = linksym(n->left->sym);
		a->type = D_OREG;
		a->name = D_PARAM;
		a->node = n->left->orig;
		break;
	
	case OCLOSUREVAR:
		if(!curfn->needctxt)
			fatal("closurevar without needctxt");
		a->type = D_OREG;
		a->reg = REGENV;
		a->offset = n->xoffset;
		a->sym = nil;
		break;		

	case OCFUNC:
		naddr(n->left, a, canemitcode);
		a->sym = linksym(n->left->sym);
		break;

	case ONAME:
		a->etype = 0;
		a->width = 0;
		a->reg = NREG;
		if(n->type != T) {
			a->etype = simtype[n->type->etype];
			a->width = n->type->width;
		}
		a->offset = n->xoffset;
		s = n->sym;
		a->node = n->orig;
		if(s == S)
			s = lookup(".noname");
		if(n->method) {
			if(n->type != T)
			if(n->type->sym != S)
			if(n->type->sym->pkg != nil)
				s = pkglookup(s->name, n->type->sym->pkg);
		}

		a->type = D_OREG;
		switch(n->class) {
		default:
			fatal("naddr: ONAME class %S %d\n", n->sym, n->class);
		case PEXTERN:
			a->name = D_EXTERN;
			break;
		case PAUTO:
			a->name = D_AUTO;
			break;
		case PPARAM:
		case PPARAMOUT:
			a->name = D_PARAM;
			break;
		case PFUNC:
			a->name = D_EXTERN;
			a->type = D_CONST;
			s = funcsym(s);
			break;
		}
		a->sym = linksym(s);
		break;

	case OLITERAL:
		switch(n->val.ctype) {
		default:
			fatal("naddr: const %lT", n->type);
			break;
		case CTFLT:
			a->type = D_FCONST;
			a->u.dval = mpgetflt(n->val.u.fval);
			break;
		case CTINT:
		case CTRUNE:
			a->sym = nil;
			a->type = D_CONST;
			a->offset = mpgetfix(n->val.u.xval);
			break;
		case CTSTR:
			datagostring(n->val.u.sval, a);
			break;
		case CTBOOL:
			a->sym = nil;
			a->type = D_CONST;
			a->offset = n->val.u.bval;
			break;
		case CTNIL:
			a->sym = nil;
			a->type = D_CONST;
			a->offset = 0;
			break;
		}
		break;

	case OITAB:
		naddr(n->left, a, canemitcode);
		a->etype = TINT32;
		if(a->type == D_CONST && a->offset == 0)
			break;
		break;

	case OSPTR:
		naddr(n->left, a, canemitcode);
		if(a->type == D_CONST && a->offset == 0)
			break;
		a->etype = simtype[tptr];
		a->offset += Array_array;
		a->width = widthptr;
		break;

	case OLEN:
		naddr(n->left, a, canemitcode);
		a->etype = TINT32;
		if(a->type == D_CONST && a->offset == 0)
			break;
		a->offset += Array_nel;
		break;

	case OCAP:
		naddr(n->left, a, canemitcode);
		a->etype = TINT32;
		if(a->type == D_CONST && a->offset == 0)
			break;
		a->offset += Array_cap;
		break;

	case OADDR:
		naddr(n->left, a, canemitcode);
		a->etype = tptr;
		switch(a->type) {
		case D_OREG:
			a->type = D_CONST;
			break;

		case D_REG:
		case D_CONST:
			break;
		
		default:
			fatal("naddr: OADDR %d\n", a->type);
		}
	}
	
	if(a->width < 0)
		fatal("naddr: bad width for %N -> %D", n, a);
}

void
gencmpcode(int op, Node *left, Node *right, int likely, Prog *to)
{
	int a, a1, lit, z, f;
	Node con, reg, freg, *swp;
	double d;

	switch(op) {
	default:
fatal:
		fatal("gencmpcode: unexpected %O", op);
		return;
	case OEQ:
	case ONE:
	case OLT:
	case OGT:
	case OLE:
	case OGE:
		break;
	}
	if(left->op == OLITERAL)
		fatal("gencmpcode: LITERAL should be on the right");

	if(isfloat[left->type->etype]) {
		f = simtype[left->type->etype] == TFLOAT32;
		z = 0;
		if(right->op == OLITERAL) {
			d = mpgetflt(right->val.u.fval);
			if(d == 0.)
				nodreg(&freg, left->type, FREGZERO+NREG);
			else if(d == 0.5)
				nodreg(&freg, left->type, FREGHALF+NREG);
			else if(d == 1.)
				nodreg(&freg, left->type, FREGONE+NREG);
			else if(d == 2.)
				nodreg(&freg, left->type, FREGTWO+NREG);
			else
				goto fmov;
			right = &freg;
		}
	fcmp:
		a1 = ABFPT;
		switch(op) {
		case ONE:
			a1 = ABFPF;
		case OEQ:
			a = ACMPEQD;
			if(f)
				a = ACMPEQF;
			break;
		case OLT:
			swp = left;
			left = right;
			right = swp;
		case OGT:
			a = ACMPGTD;
			if(f)
				a = ACMPGTF;
			break;
		case OLE:
			swp = left;
			left = right;
			right = swp;
		case OGE:
			a = ACMPGED;
			if(f)
				a = ACMPGEF;
			break;
		default:
			goto fatal;
		}

		gins3(a, left, right, N);
		patch(zbranch(a1, N, likely), to);
		if(z)
			regfree(right);
		return;
	fmov:
		z = 1;
		regalloc(&freg, left->type, N);
		if(f)
			gins(AMOVF, right, &freg);
		else
			gins(AMOVD, right, &freg);
		right = &freg;
		goto fcmp;
	}

	lit = right->op == OLITERAL;
	z = lit && right->val.ctype == CTNIL;
	if(lit && (z || isconst(right, CTINT))) {
		if(z || mpgetfix(right->val.u.xval) == 0) {
			switch(op) {
			case OEQ:
				a = ABEQ;
				break;
			case ONE:
				a = ABNE;
				break;
			case OLT:
				a = ABLTZ;
				break;
			case OGT:
				a = ABGTZ;
				break;
			case OLE:
				a = ABLEZ;
				break;
			case OGE:
				a = ABGEZ;
				break;
			}
			if(z && a != ABEQ && a != ABNE)
				fatal("gencmpcode: %O to nil", op);
			patch(zbranch(a, left, likely), to);
			return;
		}

		nodconst(&con, left->type, mpgetfix(right->val.u.xval));
		regalloc(&reg, left->type, N);
		gmove(&con, &reg);
		right = &reg;
	} else if(op != OEQ && op != ONE)
		regalloc(&reg, types[TINT32], N);
	else
		reg.op = 0;

	switch(op) {
	case OEQ:
	case ONE:
		a = ABEQ;
		if(op == ONE)
			a = ABNE;
		patch(tbranch(a, left, right, likely), to);
		break;
	default:
		a = ASGTU;
		if(issigned[left->type->etype])
			a = ASGT;
		if(op == OLE || op == OGT)
			gins3(a, left, right, &reg);
		else
			gins3(a, right, left, &reg);
		a = ABEQ;
		if(op == OLT || op == OGT)
			a = ABNE;
		patch(zbranch(a, &reg, likely), to);
		break;
	}
	if(reg.op != 0)
		regfree(&reg);
	return;
}

int
optoas(int op, Type *t)
{
	int a;

	if(t == T)
		fatal("optoas: t is nil");

	a = AGOK;
	switch(op) {
	case OADDR:
	case OEQ:
	case ONE:
	case OLT:
	case OLE:
	case OGT:
	case OGE:
	case OCMP:
	case OMINUS:
	case OEXTEND:
		fatal("optoas: %O", op);
	}

	switch(CASE(op, simtype[t->etype])) {
	default:
		if(simtype[t->etype] != TFLOAT32)
		if(simtype[t->etype] != TFLOAT64) {
			switch(op) {
			case OADD:
				a = AADDU;
				break;
			case OSUB:
				a = ASUBU;
				break;
			case OAND:
				a = AAND;
				break;
			case OOR:
				a = AOR;
				break;
			case OXOR:
				a = AXOR;
				break;
			case OLSH:
				a = ASLL;
				break;
			default:
				fatal("optoas: no entry %O-%T etype %T simtype %T", op, t, types[t->etype], types[simtype[t->etype]]);
			}
		}
		break;

	case CASE(OAS, TBOOL):
	case CASE(OAS, TINT8):
		a = AMOVB;
		break;

	case CASE(OAS, TUINT8):
		a = AMOVBU;
		break;

	case CASE(OAS, TINT16):
		a = AMOVH;
		break;

	case CASE(OAS, TUINT16):
		a = AMOVHU;
		break;

	case CASE(OAS, TINT32):
	case CASE(OAS, TUINT32):
	case CASE(OAS, TPTR32):
		a = AMOVW;
		break;

	case CASE(OAS, TFLOAT32):
		a = AMOVF;
		break;

	case CASE(OAS, TFLOAT64):
		a = AMOVD;
		break;

	case CASE(OADD, TFLOAT32):
		a = AADDF;
		break;

	case CASE(OADD, TFLOAT64):
		a = AADDD;
		break;

	case CASE(OSUB, TFLOAT32):
		a = ASUBF;
		break;

	case CASE(OSUB, TFLOAT64):
		a = ASUBD;
		break;


	case CASE(ORSH, TUINT8):
	case CASE(ORSH, TUINT16):
	case CASE(ORSH, TUINT32):
	case CASE(ORSH, TPTR32):
		a = ASRL;
		break;

	case CASE(ORSH, TINT8):
	case CASE(ORSH, TINT16):
	case CASE(ORSH, TINT32):
		a = ASRA;
		break;

	case CASE(OMUL, TUINT8):
	case CASE(OMUL, TUINT16):
	case CASE(OMUL, TUINT32):
	case CASE(OMUL, TPTR32):
		a = AMULU;
		break;

	case CASE(OMUL, TINT8):
	case CASE(OMUL, TINT16):
	case CASE(OMUL, TINT32):
		a = AMUL;
		break;

	case CASE(OMUL, TFLOAT32):
		a = AMULF;
		break;

	case CASE(OMUL, TFLOAT64):
		a = AMULD;
		break;

	case CASE(ODIV, TUINT8):
	case CASE(ODIV, TUINT16):
	case CASE(ODIV, TUINT32):
	case CASE(ODIV, TPTR32):
		a = ADIVU;
		break;

	case CASE(ODIV, TINT8):
	case CASE(ODIV, TINT16):
	case CASE(ODIV, TINT32):
		a = ADIV;
		break;

	case CASE(OMOD, TUINT8):
	case CASE(OMOD, TUINT16):
	case CASE(OMOD, TUINT32):
	case CASE(OMOD, TPTR32):
		a = AREMU;
		break;

	case CASE(OMOD, TINT8):
	case CASE(OMOD, TINT16):
	case CASE(OMOD, TINT32):
		a = AREM;
		break;

	case CASE(ODIV, TFLOAT32):
		a = ADIVF;
		break;

	case CASE(ODIV, TFLOAT64):
		a = ADIVD;
		break;

	}
	return a;
}

int
dotaddable(Node *n, Node *n1)
{
	int o;
	int64 oary[10];
	Node *nn;

	if(n->op != ODOT)
		return 0;

	o = dotoffset(n, oary, &nn);
	if(nn != N && nn->addable && o == 1 && oary[0] >= 0) {
		*n1 = *nn;
		n1->type = n->type;
		n1->xoffset += oary[0];
		return 1;
	}
	return 0;
}

enum
{
	ODynam	= 1<<0,
	OPtrto	= 1<<1,
};

static Node	clean[20];
static int	cleani = 0;

void
sudoclean(void)
{
	if(clean[cleani-1].op != OEMPTY)
		regfree(&clean[cleani-1]);
	if(clean[cleani-2].op != OEMPTY)
		regfree(&clean[cleani-2]);
	cleani -= 2;
}

int
sudoaddable(int as, Node *n, Addr *a)
{
	int o, i;
	int64 oary[10];
	int64 v;
	Node n1, *nn, *r1, *r2;

	if(n->type == T)
		return 0;

	switch(n->op) {
	case OLITERAL:
		if(!isconst(n, CTINT))
			break;
		v = mpgetfix(n->val.u.xval);
		if(v >=32766 || v < -32766)
			break;
		goto lit;

	case ODOT:
	case ODOTPTR:
		r1 = &clean[cleani++];
		r2 = &clean[cleani++];
		r1->op = r2->op = OEMPTY;
		goto odot;

	case OINDEX:
		return 0;
	}
	return 0;

lit:
	switch(as) {
	default:
		return 0;
	case AADD: case AADDU: case ASUB: case ASUBU:
	case AMOVB: case AMOVBU: case AMOVH: case AMOVHU:
	case AMOVW:
		break;
	case AAND: case AOR: case AXOR:
		if(v<0)
			return 0;
		break;
	}
	clean[cleani++].op = OEMPTY;
	clean[cleani++].op = OEMPTY;
	naddr(n, a, 1);
	return 1;

odot:
	o = dotoffset(n, oary, &nn);
	if(nn == N)
		goto no;
	if(nn->addable && o == 1 && oary[0] >= 0) {
		n1 = *nn;
		n1.type = n->type;
		n1.xoffset += oary[0];
		naddr(&n1, a, 1);
		return 1;
	}
	regalloc(r1, types[tptr], N);
	n1 = *r1;
	n1.op = OINDREG;
	if(oary[0] >= 0) {
		agen(nn, r1);
		n1.xoffset = oary[0];
	} else {
		cgen(nn, r1);
		cgen_checknil(r1);
		n1.xoffset = -(oary[0]+1);
	}
	for(i=1; i<o; i++) {
		if(oary[i] >= 0)
			fatal("can't happen");
		gins(AMOVW, &n1, r1);
		cgen_checknil(r1);
		n1.xoffset = -(oary[i]+1);
	}
	a->type = D_NONE;
	a->name = D_NONE;
	n1.type = n->type;
	naddr(&n1, a, 1);
	return 1;

no:
	sudoclean();
	return 0;
}
