
#include <u.h>
#include <libc.h>
#include "gg.h"

int
dsname(Sym *sym, int off, char *t, int n)
{
	Prog *p;

	p = gins(ADATA, N, N);
	p->from.type = D_OREG;
	p->from.name = D_EXTERN;
	p->from.etype = TINT32;
	p->from.offset = off;
	p->from.reg = NREG;
	p->from.sym = linksym(sym);
	
	p->reg = n;
	
	p->to.type = D_SCONST;
	p->to.name = D_NONE;
	p->to.reg = NREG;
	p->to.offset = 0;
	memmove(p->to.u.sval, t, n);
	return off + n;
}

void
datastring(char *s, int len, Addr *a)
{
	Sym *sym;
	
	sym = stringsym(s, len);
	a->type = D_OREG;
	a->name = D_EXTERN;
	a->etype = TINT32;
	a->offset = widthptr+4;
	a->reg = NREG;
	a->sym = linksym(sym);
	a->node = sym->def;
}

void
datagostring(Strlit *sval, Addr *a)
{
	Sym *sym;
	
	sym = stringsym(sval->s, sval->len);
	a->type = D_OREG;
	a->name = D_EXTERN;
	a->etype = TINT32;
	a->offset = 0;
	a->reg = NREG;
	a->sym = linksym(sym);
	a->node = sym->def;
}

void
gdata(Node *nam, Node *nr, int wid)
{
	Prog *p;
	vlong v;

	if(nr->op == OLITERAL) {
		switch(nr->val.ctype) {
		case CTCPLX:
			gdatacomplex(nam, nr->val.u.cval);
			return;
		case CTSTR:
			gdatastring(nam, nr->val.u.sval);
			return;
		}
	}

	if(wid == 8 && is64(nr->type)) {
		v = mpgetfix(nr->val.u.xval);
		
		if(isbigendian)
			p = gins(ADATA, nam, nodintconst(v>>32));
		else
			p = gins(ADATA, nam, nodintconst(v));
		p->reg = 4;

		if(isbigendian)
			p = gins(ADATA, nam, nodintconst(v));
		else
			p = gins(ADATA, nam, nodintconst(v>>32));
		p->reg = 4;
		p->from.offset += 4;
		return;
	}
	p = gins(ADATA, nam, nr);
	p->reg = wid;
}

void
gdatacomplex(Node *nam, Mpcplx *cval)
{
	Prog *p;
	int w;

	w = cplxsubtype(nam->type->etype);
	w = types[w]->width;

	p = gins(ADATA, nam, N);
	p->reg = w;
	p->to.type = D_FCONST;
	p->to.u.dval = mpgetflt(&cval->real);

	p = gins(ADATA, nam, N);
	p->reg = w;
	p->from.offset += w;
	p->to.type = D_FCONST;
	p->to.u.dval = mpgetflt(&cval->imag);
}

void
gdatastring(Node *nam, Strlit *sval)
{
	Prog *p;
	Node nod1;

	p = gins(ADATA, nam, N);
	datastring(sval->s, sval->len, &p->to);
	p->reg = types[tptr]->width;
	p->to.type = D_CONST;
	p->to.etype = TINT32;

	nodconst(&nod1, types[TINT32], sval->len);
	p = gins(ADATA, nam, &nod1);
	p->reg = types[TINT32]->width;
	p->from.offset += types[tptr]->width;
}

int
dstringptr(Sym *s, int off, char *str)
{
	Prog *p;

	off = rnd(off, widthptr);
	p = gins(ADATA, N, N);
	p->from.type = D_OREG;
	p->from.name = D_EXTERN;
	p->from.sym = linksym(s);
	p->from.offset = off;
	p->reg = widthptr;

	datastring(str, strlen(str)+1, &p->to);
	p->to.type = D_CONST;
	p->to.etype = TINT32;
	off += widthptr;

	return off;
}

int
dgostrlitptr(Sym *s, int off, Strlit *lit)
{
	Prog *p;

	if(lit == nil)
		return duintptr(s, off, 0);

	off = rnd(off, widthptr);
	p = gins(ADATA, N, N);
	p->from.type = D_OREG;
	p->from.name = D_EXTERN;
	p->from.sym = linksym(s);
	p->from.offset = off;
	p->reg = widthptr;
	datagostring(lit, &p->to);
	p->to.type = D_CONST;
	p->to.etype = TINT32;
	off += widthptr;

	return off;
}

int
dgostringptr(Sym *s, int off, char *str)
{
	int n;
	Strlit *lit;

	if(str == nil)
		return duintptr(s, off, 0);

	n = strlen(str);
	lit = mal(sizeof *lit + n);
	strcpy(lit->s, str);
	lit->len = n;
	return dgostrlitptr(s, off, lit);
}

int
dsymptr(Sym *s, int off, Sym *x, int xoff)
{
	Prog *p;

	off = rnd(off, widthptr);

	p = gins(ADATA, N, N);
	p->from.type = D_OREG;
	p->from.name = D_EXTERN;
	p->from.sym = linksym(s);
	p->from.offset = off;
	p->reg = widthptr;
	p->to.type = D_CONST;
	p->to.name = D_EXTERN;
	p->to.sym = linksym(x);
	p->to.offset = xoff;
	off += widthptr;

	return off;
}

void
nopout(Prog *p)
{
	p->as = ANOP;
}
