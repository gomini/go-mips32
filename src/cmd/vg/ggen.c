
#undef	EXTERN
#define	EXTERN
#include <u.h>
#include <libc.h>
#include "gg.h"
#include "opt.h"

static Prog* appendpp(Prog*, int, int, int, int32, int, int, int32);
static Prog *zerorange(Prog *p, vlong frame, vlong lo, vlong hi);

void
defframe(Prog *ptxt)
{
	uint32 frame;
	Prog *p;
	vlong hi, lo;
	NodeList *l;
	Node *n;

	ptxt->to.type = D_CONST2;
	ptxt->to.offset2 = rnd(curfn->type->argwid, widthptr);

	frame = rnd(stksize+maxarg, widthptr);
	ptxt->to.offset = frame;
	
	p = ptxt;
	lo = hi = 0;
	for(l=curfn->dcl; l != nil; l = l->next) {
		n = l->n;
		if(!n->needzero)
			continue;
		if(n->class != PAUTO)
			fatal("needzero class %d", n->class);
		if(n->type->width % widthptr != 0 || n->xoffset % widthptr != 0 || n->type->width == 0)
			fatal("var %lN has size %d offset %d", n, (int)n->type->width, (int)n->xoffset);
		if(lo != hi && n->xoffset + n->type->width >= lo - 2*widthptr) {
			lo = rnd(n->xoffset, widthptr);
			continue;
		}
		p = zerorange(p, frame, lo, hi);

		hi = n->xoffset + n->type->width;
		lo = n->xoffset;
	}
	zerorange(p, frame, lo, hi);
}

static Prog*
zerorange(Prog *p, vlong frame, vlong lo, vlong hi)
{
	vlong cnt, i;
	Prog *p0;

	cnt = hi - lo;
	if(cnt == 0)
		return p;

	if(cnt < 4*widthptr) {
		for(i = 0; i < cnt; i += widthptr) {
			p = appendpp(p, AMOVW, D_REG, REGZERO, 0, D_OREG, REGSP, 4+frame+lo+i);
		}
	} else {
		p = appendpp(p, AADDU, D_CONST, NREG, 4+frame+lo, D_REG, REGRT1, 0);
		p->reg = REGSP;
		p = appendpp(p, AADDU, D_CONST, NREG, cnt, D_REG, REGRT2, 0);
		p->reg = REGRT1;

		p = appendpp(p, AMOVW, D_REG, REGZERO, 0, D_OREG, REGRT1, 0);
		p0 = p;
		p = appendpp(p, AADDU, D_CONST, NREG, widthptr, D_REG, REGRT1, 0);
		p->reg = REGRT1;

		p = appendpp(p, ABNE, D_REG, REGRT1, 0, D_NONE, 0, 0);
		p->reg = REGRT2;
		p->to.type = D_BRANCH;
		p->to.u.branch = P;
		patch(p, p0);
	}
	return p;
}

static Prog*	
appendpp(Prog *p, int as, int ftype, int freg, int32 foffset, int ttype, int treg, int32 toffset)	
{	
	Prog *q;	
		
	q = mal(sizeof(*q));	
	clearp(q);	
	q->as = as;	
	q->lineno = p->lineno;	
	q->from.type = ftype;	
	q->from.reg = freg;	
	q->from.offset = foffset;	
	q->to.type = ttype;	
	q->to.reg = treg;	
	q->to.offset = toffset;	
	q->link = p->link;	
	p->link = q;	
	return q;	
}

void
markautoused(Prog* p)
{
	for (; p; p = p->link) {
		if (p->as == ATYPE || p->as == AVARDEF || p->as == AVARKILL)
			continue;

		if (p->from.node)
			p->from.node->used = 1;

		if (p->to.node)
			p->to.node->used = 1;
	}
}

void
fixautoused(Prog* p)
{
	Prog **lp;

	for (lp=&p; (p=*lp) != P; ) {
		if (p->as == ATYPE && p->from.node && p->from.name == D_AUTO && !p->from.node->used) {
			*lp = p->link;
			continue;
		}
		if ((p->as == AVARDEF || p->as == AVARKILL) && p->to.node && !p->to.node->used) {
			p->to.type = D_NONE;
			p->to.node = N;
			p->as = ANOP;
			continue;
		}

		if (p->from.name == D_AUTO && p->from.node)
			p->from.offset += p->from.node->stkdelta;

		if (p->to.name == D_AUTO && p->to.node)
			p->to.offset += p->to.node->stkdelta;

		lp = &p->link;
	}
}

void
ginscall(Node *f, int proc)
{
	Prog *p;
	Node n1, r, r1;

	if(f->type != T)
		setmaxarg(f->type);

	switch(proc) {
	default:
		fatal("ginscall: bad proc %d", proc);
		break;

	case 0:
	case -1:
		if(f->op == ONAME && f->class == PFUNC) {
			if(f == deferreturn) {
				nodreg(&r, types[TINT], 0);
				gins3(ASLL, ncon(0), &r, &r);
			}
			p = gins(AJAL, N, f);
			afunclit(&p->to, f);
			if(proc == -1 || noreturn(p))
				gins(AUNDEF, N, N);
			break;
		}

		nodreg(&r, types[tptr], REGENV);
		nodreg(&r1, types[tptr], REGRT1);
		gmove(f, &r);
		r.op = OINDREG;
		gmove(&r, &r1);
		r.op = OREGISTER;
		r1.op = OINDREG;
		gins(AJAL, &r, &r1);
		break;

	case 3:
		gins(AJAL, N, f);
		break;

	case 1:
	case 2:


		p = gins(AADDU, nscon(-12), N);
		p->reg = REGSP;
		p->to.type = D_REG;
		p->to.reg = REGSP;

		regalloc(&r, types[tptr], N);

		memset(&n1, 0, sizeof n1);
		n1.op = OADDR;
		n1.left = f;
		gins(AMOVW, &n1, &r);

		p = gins(AMOVW, &r, N);
		p->to.type = D_OREG;
		p->to.reg = REGSP;
		p->to.offset = 8;

		gins(AMOVW, ncon(argsize(f->type)), &r);
		p = gins(AMOVW, &r, N);
		p->to.type = D_OREG;
		p->to.reg = REGSP;
		p->to.offset = 4;

		regfree(&r);

		if(proc == 1)
			ginscall(newproc, 0);
		else
			ginscall(deferproc, 0);

		p = gins(AADDU, ncon(12), N);
		p->reg = REGSP;
		p->to.reg = REGSP;
		p->to.type = D_REG;

		if(proc == 2) {
			nodreg(&r, types[TINT32], REGRT1);
			p = zbranch(ABEQ, &r, +1);
			cgen_ret(N);
			patch(p, pc);
		}
		break;
	}
}

void
cgen_callinter(Node *n, Node *res, int proc)
{
	int r;
	Node *i, *f;
	Node tmpi, nodo, nodr, nodsp;
	Prog *p;

	i = n->left;
	if(i->op != ODOTINTER)
		fatal("cgen_callinter: not ODOTINTER %O", i->op);

	f = i->right;
	if(f->op != ONAME)
		fatal("cgen_callinter: not ONAME %O", f->op);

	i = i->left;

	r = -1;
	if(res != N && (res->op == OREGISTER || res->op == OINDREG))
	if(res->val.u.reg > 0) {
		r = res->val.u.reg;
		reg[r]--;
	}

	if(!i->addable) {
		tempname(&tmpi, i->type);
		cgen(i, &tmpi);
		i = &tmpi;
	}

	genlist(n->list);
	if(r > 0)
		reg[r]++;

	regalloc(&nodr, types[tptr], res);
	regalloc(&nodo, types[tptr], &nodr);
	nodo.op = OINDREG;

	agen(i, &nodr);

	nodindreg(&nodsp, types[tptr], REGSP);
	nodsp.xoffset = 4;
	nodo.xoffset += widthptr;
	cgen(&nodo, &nodsp);

	nodo.xoffset -= widthptr;
	cgen(&nodo, &nodr);
	cgen_checknil(&nodr);

	nodo.xoffset = n->left->xoffset + 3*widthptr + 8;
	
	if(proc == 0) {
		cgen(&nodo, &nodr);
		nodr.op = OINDREG;
		proc = 3;
	} else {
		p = gins(AMOVW, &nodo, &nodr);
		p->from.type = D_CONST;
	}

	nodr.type = n->left->type;
	ginscall(&nodr, proc);

	regfree(&nodr);
	regfree(&nodo);
}

void
cgen_call(Node *n, int proc)
{
	Type *t;
	Node nod, afun;

	if(n == N)
		return;

	if(n->left->ullman >= UINF) {
		tempname(&afun, types[tptr]);
		cgen(n->left, &afun);
	}

	genlist(n->list);
	t = n->left->type;

	if(n->left->ullman >= UINF) {
		regalloc(&nod, types[tptr], N);
		cgen_as(&nod, &afun);
		nod.type = t;
		ginscall(&nod, proc);
		regfree(&nod);
		return;
	}

	if(n->left->op != ONAME || n->left->class != PFUNC) {
		regalloc(&nod, types[tptr], N);
		cgen_as(&nod, n->left);
		nod.type = t;
		ginscall(&nod, proc);
		regfree(&nod);
		return;
	}

	n->left->method = 1;
	ginscall(n->left, proc);
}

void
cgen_callret(Node *n, Node *res)
{
	Node nod;
	Type *fp, *t;
	Iter flist;

	t = n->left->type;
	if(t->etype == TPTR32 || t->etype == TPTR64)
		t = t->type;

	fp = structfirst(&flist, getoutarg(t));
	if(fp == T)
		fatal("cgen_callret: nil");

	memset(&nod, 0, sizeof(nod));
	nod.op = OINDREG;
	nod.val.u.reg = REGSP;
	nod.addable = 1;

	nod.xoffset = fp->width + widthptr;
	nod.type = fp->type;
	cgen_as(res, &nod);
}

void
cgen_aret(Node *n, Node *res)
{
	Node nod1, nod2;
	Type *fp, *t;
	Iter flist;

	t = n->left->type;
	if(isptr[t->etype])
		t = t->type;

	fp = structfirst(&flist, getoutarg(t));
	if(fp == T)
		fatal("cgen_aret: nil");

	memset(&nod1, 0, sizeof(nod1));
	nod1.op = OINDREG;
	nod1.val.u.reg = REGSP;
	nod1.addable = 1;

	nod1.xoffset = fp->width + widthptr;
	nod1.type = fp->type;

	if(res->op != OREGISTER) {
		regalloc(&nod2, types[tptr], res);
		agen(&nod1, &nod2);
		gins(AMOVW, &nod2, res);
		regfree(&nod2);
	} else
		agen(&nod1, res);
}

void
cgen_ret(Node *n)
{
	Prog *p;

	if(n != N)
		genlist(n->list);
	if(hasdefer)
		ginscall(deferreturn, 0);
	genlist(curfn->exit);
	p = gins(ARET, N, N);
	if(n != N && n->op == ORETJMP) {
		p->to.name = D_EXTERN;
		p->to.type = D_CONST;
		p->to.sym = linksym(n->left->sym);
	}
}

void
cgen_asop(Node *n)
{
	Node n1, n2, n3, n4;
	Node *nl, *nr;
	Prog *p1;
	Addr addr;
	int a, w;

	nl = n->left;
	nr = n->right;

	if(nr->ullman >= UINF && nl->ullman >= UINF) {
		tempname(&n1, nr->type);
		cgen(nr, &n1);
		n2 = *n;
		n2.right = &n1;
		cgen_asop(&n2);
		return;
	}

	if(!isint[nl->type->etype])
		goto hard;
	if(!isint[nr->type->etype])
		goto hard;
	if(is64(nl->type) || is64(nr->type))
		goto hard64;

	switch(n->etype) {
	case OADD:
	case OSUB:
	case OXOR:
	case OAND:
	case OOR:
		a = optoas(n->etype, nl->type);
		if(nl->addable) {
			if(smallintconst(nr))
				n3 = *nr;
			else {
				regalloc(&n3, nr->type, N);
				cgen(nr, &n3);
			}
			regalloc(&n2, nl->type, N);
			cgen(nl, &n2);
			gins(a, &n3, &n2);
			cgen(&n2, nl);
			regfree(&n2);
			if(n3.op != OLITERAL)
				regfree(&n3);
			return;
		}
		if(nr->ullman < UINF)
		if(sudoaddable(a, nl, &addr)) {
			w = optoas(OAS, nl->type);
			regalloc(&n2, nl->type, N);
			p1 = gins(w, N, &n2);
			p1->from = addr;
			regalloc(&n3, nr->type, N);
			cgen(nr, &n3);
			gins(a, &n3, &n2);
			p1 = gins(w, &n2, N);
			p1->to = addr;
			regfree(&n2);
			regfree(&n3);
			sudoclean();
			return;
		}
	}

hard:
	n2.op = 0;
	n1.op = 0;
	if(nr->op == OLITERAL) {
	} else if(nr->ullman >= nl->ullman || nl->addable) {
		regalloc(&n2, nr->type, N);
		cgen(nr, &n2);
		nr = &n2;
	} else {
		tempname(&n2, nr->type);
		cgen(nr, &n2);
		nr = &n2;
	}
	if(!nl->addable) {
		igen(nl, &n1, N);
		nl = &n1;
	}

	n3 = *n;
	n3.left = nl;
	n3.right = nr;
	n3.op = n->etype;

	regalloc(&n4, nl->type, N);
	cgen(&n3, &n4);
	gmove(&n4, nl);

	if(n1.op)
		regfree(&n1);
	if(n2.op == OREGISTER)
		regfree(&n2);
	regfree(&n4);
	return;

hard64:
	if(nr->ullman > nl->ullman) {
		tempname(&n2, nr->type);
		cgen(nr, &n2);
		igen(nl, &n1, N);
	} else {
		igen(nl, &n1, N);
		tempname(&n2, nr->type);
		cgen(nr, &n2);
	}

	n3 = *n;
	n3.left = &n1;
	n3.right = &n2;
	n3.op = n->etype;

	cgen(&n3, &n1);

	if(n1.op == OREGISTER || n1.op == OINDREG)
		regfree(&n1);
}


void
cgen_div(int op, Node *nl, Node *nr, Node *res)
{
	int a, check;
	Type *t, *t0;
	Node tl, tr, tl2, tr2, z, neg;
	Prog *p1, *p2;

	if(is64(nl->type))
		fatal("cgen_div %T", nl->type);


	t = nl->type;
	t0 = t;
	check = 0;
	nodreg(&z, types[TINT32], 0);

	a = ADIV;
	if(issigned[t->etype]) {
		check = 1;
		if(isconst(nl, CTINT) && mpgetfix(nl->val.u.xval) != -1LL<<(t->width*8-1))
			check = 0;
		else if(isconst(nr, CTINT) && mpgetfix(nr->val.u.xval) != -1)
			check = 0;
	} else
		a = ADIVU;
	if(t->width < 4) {
		if(issigned[t->etype])
			t = types[TINT32];
		else
			t = types[TUINT32];
		check = 0;
	}

	regalloc(&tl, t0, N);
	regalloc(&tr, t0, N);
	if(nl->ullman >= nr->ullman) {
		cgen(nl, &tl);
		cgen(nr, &tr);
	} else {
		cgen(nr, &tr);
		cgen(nl, &tl);
	}
	if(t != t0) {
		tl2 = tl;
		tr2 = tr;
		tl.type = tr.type = t;
		gmove(&tl2, &tl);
		gmove(&tr2, &tr);
	}

	p1 = zbranch(ABNE, &tr, +1);
	if(panicdiv == N)
		panicdiv = sysfunc("panicdivide");
	ginscall(panicdiv, -1);
	patch(p1, pc);

	p2 = nil;
	if(check) {
		regalloc(&neg, t, N);
		gins3(AADDU, nscon(-1), &z, &neg);
		p1 = tbranch(ABNE, &tr, &neg, +1);
		if(op == ODIV) {
			gins3(ASUBU, &tl, &z, &tl);
			gmove(&tl, res);
		} else {
			gmove(&z, res);
		}
		p2 = gjmp(P);
		patch(p1, pc);
		regfree(&neg);
	}
	gins3(a, &tr, &tl, N);
	regfree(&tr);
	if(op == ODIV) {
		p1 = gins(AMOVW, N, &tl);
		p1->from.type = D_LO;
	} else {
		p1 = gins(AMOVW, N, &tl);
		p1->from.type = D_HI;
	}
	gmove(&tl, res);
	regfree(&tl);
	if(check)
		patch(p2, pc);
}

void
cgen_hmul(Node *nl, Node *nr, Node *res)
{
	int w, tt, a, a1;
	Node n1, n2, *tmp;
	Type *t;
	Prog *p;

	if(nl->ullman < nr->ullman) {
		tmp = nl;
		nl = nr;
		nr = tmp;
	}
	
	t = nl->type;
	w = t->width * 8;
	
	regalloc(&n1, t, res);
	cgen(nl, &n1);
	regalloc(&n2, t, N);
	cgen(nr, &n2);

	tt = simtype[t->etype];
	a = AMULU;
	a1 = ASRL;
	if(issigned[t->etype]) {
		a = AMUL;
		a1 = ASRA;
	}

	switch(tt) {
	case TINT8:
	case TINT16:
	case TUINT8:
	case TUINT16:
		gins3(a, &n1, &n2, N);
		p = gins(AMOVW, N, &n1);
		p->from.type = D_LO;
		gins3(a1, ncon(w), &n1, &n1);
		break;
	case TINT32:
	case TUINT32:
		gins3(a, &n1, &n2, N);
		p = gins(AMOVW, N, &n1);
		p->from.type = D_HI;
		break;
	default:
		fatal("cgen_hmul %T", t);
		break;
	}
	regfree(&n2);

	cgen(&n1, res);
	regfree(&n1);
}

void
cgen_shift(int op, int bounded, Node *nl, Node *nr, Node *res)
{
	Node n1, n2, n3, nt, lo, hi, z;
	int w, v;
	Prog *p1, *p2, *p3, *p4;
	Type *tr;
	uvlong sc;

	USED(bounded);
	if(nl->type->width > 4)
		fatal("cgen_shift %T", nl->type);

	w = nl->type->width * 8;

	if(op == OLROT) {
		v = mpgetfix(nr->val.u.xval);
		regalloc(&n1, nl->type, res);
		cgen(nl, &n1);
		if(w == 32) {
		}
		regalloc(&n2, nl->type, N);
		gins3(ASLL, ncon(v), &n1, &n2);
		gins3(ASRL, ncon(w-v), &n1, &n1);
		gins(AOR, &n2, &n1);
		regfree(&n2);
		if(w != 32)
			gins(optoas(OAS, nl->type), &n1, &n1);
		gmove(&n1, res);
		regfree(&n1);
		return;
	}

	nodreg(&z, types[TINT32], 0);

	if(nr->op == OLITERAL) {
		regalloc(&n1, nl->type, res);
		cgen(nl, &n1);
		sc = mpgetfix(nr->val.u.xval);
		if(sc == 0) {
		} else if(sc >= w) {
			if(op == ORSH && issigned[nl->type->etype])
				gins3(ASRA, ncon(31), &n1, &n1);
			else
				gins(AMOVW, &z, &n1);
		} else {
			if(op == ORSH && issigned[nl->type->etype])
				gins3(ASRA, ncon(sc), &n1, &n1);
			else if(op == ORSH)
				gins3(ASRL, ncon(sc), &n1, &n1);
			else
				gins3(ASLL, ncon(sc), &n1, &n1);
		}
		if(w < 32 && op == OLSH)
			gins(optoas(OAS, nl->type), &n1, &n1);
		gmove(&n1, res);
		regfree(&n1);
		return;
	}

	tr = nr->type;
	p1 = p2 = nil;

	if(tr->width > 4) {
		tempname(&nt, nr->type);
		if(nl->ullman >= nr->ullman) {
			regalloc(&n2, nl->type, res);
			cgen(nl, &n2);
			cgen(nr, &nt);
		} else {
			cgen(nr, &nt);
			regalloc(&n2, nl->type, res);
			cgen(nl, &n2);
		}
		split64(&nt, &lo, &hi);
		regalloc(&n1, types[TUINT32], N);
		regalloc(&n3, types[TUINT32], N);
		gmove(&lo, &n1);
		gmove(&hi, &n3);
		splitclean();

		p1 = zbranch(ABNE, &n3, -1);

		gins3(ASGTU, ncon(w), &n1, &n3);
		p2 = zbranch(ABEQ, &n3, -1);
		tr = types[TUINT32];
		regfree(&n3);
	} else {
		if(nl->ullman >= nr->ullman) {
			regalloc(&n2, nl->type, res);
			cgen(nl, &n2);
			regalloc(&n1, nr->type, N);
			cgen(nr, &n1);
		} else {
			regalloc(&n1, nr->type, N);
			cgen(nr, &n1);
			regalloc(&n2, nl->type, res);
			cgen(nl, &n2);
		}

		regalloc(&n3, types[TINT32], N);
		if(issigned[nr->type->etype])
			gins3(ASGT, ncon(w), &n1, &n3);
		else
			gins3(ASGTU, ncon(w), &n1, &n3);
		p2 = zbranch(ABEQ, &n3, -1);
		regfree(&n3);
	}

	p3 = zbranch(ABEQ, &n1, -1);

	if(op == ORSH) {
		if(issigned[nl->type->etype])
			gins3(ASRA, &n1, &n2, &n2);
		else
			gins3(ASRL, &n1, &n2, &n2);
	} else
		gins3(ASLL, &n1, &n2, &n2);

	p4 = gjmp(P);

	if(p1)
		patch(p1, pc);
	if(p2)
		patch(p2, pc);
	if(op == ORSH && issigned[nl->type->etype])
		gins3(ASRA, ncon(w-1), &n2, &n2);
	else
		gins(AMOVW, &z, &n2);

	patch(p4, pc);
	if(w < 32 && op == OLSH)
		gins(optoas(OAS, nl->type), &n2, &n2);
	patch(p3, pc);

	gmove(&n2, res);

	regfree(&n1);
	regfree(&n2);
}

void
clearfat(Node *nl)
{
	uint32 w, c, q, i;
	Node dst, end, z;
	Prog *p, *p0;

	if(debug['g'])
		dump("\nclearfat", nl);

	w = nl->type->width;
	if(componentgen(N, nl))
		return;

	regalloc(&dst, types[tptr], N);
	agen(nl, &dst);

	nodreg(&z, types[TINT32], 0);
	c = w & 3;
	q = w >> 2;

	if(q < 4) {
		for(i=0; i < q; i++) {
			p = gins(AMOVW, &z, &dst);
			p->to.type = D_OREG;
			p->to.offset = i*4;
		}
		if(q > 0 && c > 0) {
			p = gins(AADDU, ncon(4*q), &dst);
			p->to.type = D_REG;
			p->reg = dst.val.u.reg;
		}
	} else {
		regalloc(&end, types[tptr], N);
		p = gins3(AADDU, ncon(q*4), &dst, &end);

		p = gins(AMOVW, &z, &dst);
		p0 = p;
		p->to.type = D_OREG;
		p->to.offset = 0;

		p = gins(AADDU, ncon(widthptr), &dst);
		p->to.type = D_REG;
		p->reg = dst.val.u.reg;

		p = tbranch(ABNE, &dst, &end, -1);
		p->from.type = D_REG;
		patch(p, p0);

		regfree(&end);
	}

	for(i=0; i<c; i++) {
		p = gins(AMOVB, &z, &dst);
		p->to.type = D_OREG;
		p->to.offset = i;
	}
	regfree(&dst);

}

void
expandchecks(Prog *firstp)
{
	Prog *p, *p1, *q;
	int32 i;

	for(p = firstp; p != P; p = p->link) {
	cont:
		if(p->as != ACHECKNIL)
			continue;
		if(debug_checknil && p->lineno > 1)
			warnl(p->lineno, "generated nil check");
		if(p->from.type != D_REG) {
			p1 = p;
			goto failed;
		}
		if(p->from.reg == REGTMP) {
			p1 = p;
			goto failed;
		}

		for(i = 0, q = p->link; q != P && i < 5; q = q->link, i++) {
			switch(q->as) {
			case AMOVW: case AMOVB: case AMOVBU: case AMOVH: case AMOVHU:
				if((q->from.type == D_OREG && q->from.reg == p->from.reg) ||
				(q->to.type == D_OREG && q->to.reg == p->from.reg)) {
					*p = *p->link;
					goto cont;
				}
				break;
			case ADUFFCOPY:
				if(p->from.reg == (REGRT1+4) || p->from.reg == (REGRT1+5)) {
					*p = *p->link;
					goto cont;
				}
				break;
			case ADUFFZERO:
				if(p->from.reg == REGRT1) {
					*p = *p->link;
					goto cont;
				}
				break;
			}
		}

		p1 = mal(sizeof *p1);
		clearp(p1);

		p1->link = p->link;
		p->link = p1;

		p1->lineno = p->lineno;
		p1->pc = 9999;

		p->as = ABNE;
		p->reg = REGZERO;
		p->to.type = D_BRANCH;
		p->to.u.branch = p1->link;

		p1->as = AMOVW;
		p1->from.type = D_CONST;
		p1->from.offset = 0;
		p1->to.type = D_OREG;
		p1->to.reg = REGZERO;
		p1->to.offset = 0;
	}
	return;
failed:
	fatal("CHECKNIL %P", p1);
}
