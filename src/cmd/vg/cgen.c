
#include <u.h>
#include <libc.h>
#include "gg.h"

static void	agenr(Node *n, Node *a, Node *res);

void
cgen(Node *n, Node *res)
{
	Node *nl, *nr, *r, n1, n2, n3, zero;
	Prog *p1, *p2, *p3;
	int a, f;
	Addr addr;

	if(debug['g']) {
		dump("\ncgen-n", n);
		dump("cgen-res", res);
	}

	if(n == N || n->type == T)
		fatal("cgen: n nil");
	if(res == N || res->type == T)
		fatal("cgen: res nil");

	while(n->op == OCONVNOP)
		n = n->left;

	switch(n->op) {
	case OSLICE:
	case OSLICEARR:
	case OSLICESTR:
	case OSLICE3:
	case OSLICE3ARR:
		if (res->op != ONAME || !res->addable) {
			tempname(&n1, n->type);
			cgen_slice(n, &n1);
			cgen(&n1, res);
		} else
			cgen_slice(n, res);
		return;
	case OEFACE:
		if (res->op != ONAME || !res->addable) {
			tempname(&n1, n->type);
			cgen_eface(n, &n1);
			cgen(&n1, res);
		} else
			cgen_eface(n, res);
		return;
	}

	if(n->ullman >= UINF) {
		if(n->op == OINDREG)
			fatal("cgen: this is going to misscompile");
		if(res->ullman >= UINF) {
			tempname(&n1, n->type);
			cgen(n, &n1);
			cgen(&n1, res);
			return;
		}
	}

	if(isfat(n->type)) {
		if(n->type->width < 0)
			fatal("forgot to compute width for %T", n->type);
		sgen(n, res, n->type->width);
		return;
	}

	switch(n->op) {
	case OSPTR:
	case OLEN:
		if(isslice(n->left->type) || istype(n->left->type, TSTRING))
			n->addable = n->left->addable;
		break;
	case OCAP:
		if(isslice(n->left->type))
			n->addable = n->left->addable;
		break;
	case OITAB:
		n->addable = n->left->addable;
		break;
	}

	if(n->addable && res->addable) {
		f = is64(n->type) || is64(res->type)
		|| n->op == OREGISTER || res->op == OREGISTER
		|| iscomplex[n->type->etype] || iscomplex[res->type->etype];
		
		if(f)
			gmove(n, res);
		else {
			regalloc(&n1, n->type, N);
			gmove(n, &n1);
			cgen(&n1, res);
			regfree(&n1);
		}
		return;
	}

	if(!n->addable && !res->addable) {
		tempname(&n1, n->type);
		cgen(n, &n1);
		cgen(&n1, res);
		return;
	}

	if(!res->addable) {
		igen(res, &n1, N);
		cgen(n, &n1);
		regfree(&n1);
		return;
	}


	if(complexop(n, res)) {
		complexgen(n, res);
		return;
	}

	if(!is64(n->type) && !is64(res->type))
	if(!iscomplex[n->type->etype] && !iscomplex[res->type->etype]) {
		a = optoas(OAS, n->type);
		if(sudoaddable(a, n, &addr)) {
			if(res->op == OREGISTER) {
				p1 = gins(a, N, res);
				p1->from = addr;
			} else {
				regalloc(&n2, n->type, N);
				p1 = gins(a, N, &n2);
				p1->from = addr;
				gins(a, &n2, res);
				regfree(&n2);
			}
			sudoclean();
			return;
		}
	}

	nl = n->left;
	nr = n->right;
	if(nl != N && nl->ullman >= UINF)
	if(nr != N && nr->ullman >= UINF) {
		tempname(&n1, nl->type);
		cgen(nl, &n1);
		n2 = *n;
		n2.left = &n1;
		cgen(&n2, res);
		return;
	}

	if(is64(n->type) || is64(res->type) || n->left != N && is64(n->left->type)) {
		switch(n->op) {
		case OMINUS:
		case OCOM:
		case OADD:
		case OSUB:
		case OMUL:
		case OLROT:
		case OLSH:
		case ORSH:
		case OAND:
		case OOR:
		case OXOR:
			cgen64(n, res);
			return;
		}
	}

	switch(n->op) {
	default:
		dump("cgen", n);
		fatal("cgen %O", n->op);
		break;

	case OREAL:
	case OIMAG:
	case OCOMPLEX:
		fatal("unexpected complex");
		return;

	case OOROR:
	case OANDAND:
	case OEQ:
	case ONE:
	case OLT:
	case OLE:
	case OGE:
	case OGT:
	case ONOT:
		p1 = gjmp(P);
		p2 = pc;
		gmove(nodbool(1), res);
		p3 = gjmp(P);
		patch(p1, pc);
		bgen(n, 1, 0, p2);
		gmove(nodbool(0), res);
		patch(p3, pc);
		break;


	case OPLUS:
		cgen(nl, res);
		break;

	case OMINUS:
		regalloc(&n1, nl->type, N);
		cgen(nl, &n1);
		if(isfloat[nl->type->etype]) {
			if(simtype[nl->type->etype] == TFLOAT32)
				a = ANEGF;
			else
				a = ANEGD;
			gins(a, &n1, &n1);
		} else {
			nodreg(&zero, nl->type, 0);
			gins3(ASUBU, &n1, &zero, &n1);
		}
		gmove(&n1, res);
		regfree(&n1);
		return;

	case OCOM:
		regalloc(&n1, nl->type, N);
		cgen(nl, &n1);
		nodreg(&zero, nl->type, 0);
		gins3(ANOR, &n1, &zero, &n1);
		gmove(&n1, res);
		regfree(&n1);
		break;

	case OAND:
	case OOR:
	case OXOR:
	case OADD:
	case OMUL:
		a = optoas(n->op, nl->type);
		goto sbop;

	case OSUB:
		a = optoas(n->op, nl->type);
		goto abop;

	case OHMUL:
		cgen_hmul(nl, nr, res);
		break;

	case OLSH:
	case ORSH:
	case OLROT:
		cgen_shift(n->op, n->bounded, nl, nr, res);
		break;

	case OCONV:
		if(eqtype(n->type, nl->type) || noconv(n->type, nl->type)) {
			cgen(nl, res);
			break;
		}

		if(nl->addable && !is64(nl->type)) {
			regalloc(&n1, nl->type, res);
			gmove(nl, &n1);
		} else {
			if(n->type->width > widthptr || is64(nl->type) || isfloat[nl->type->etype])
				tempname(&n1, nl->type);
			else
				regalloc(&n1, nl->type, res);
			cgen(nl, &n1);
		}
		if(n->type->width > widthptr || is64(n->type) || isfloat[n->type->etype])
			tempname(&n2, n->type);
		else
			regalloc(&n2, n->type, N);
		gmove(&n1, &n2);
		gmove(&n2, res);
		if(n1.op == OREGISTER)
			regfree(&n1);
		if(n2.op == OREGISTER)
			regfree(&n2);
		break;

	case ODOT:
	case ODOTPTR:
	case OINDEX:
	case OIND:
	case ONAME:
		igen(n, &n1, res);
		gmove(&n1, res);
		regfree(&n1);
		break;

	case OITAB:
		igen(nl, &n1, res);
		n1.type = n->type;
		gmove(&n1, res);
		regfree(&n1);
		break;

	case OSPTR:
		if(isconst(nl, CTSTR)) {
			regalloc(&n1, types[tptr], res);
			p1 = gins(AMOVW, N, &n1);
			datastring(nl->val.u.sval->s, nl->val.u.sval->len, &p1->from);
			gmove(&n1, res);
			regfree(&n1);
			break;
		}
		igen(nl, &n1, res);
		n1.type = n->type;
		gmove(&n1, res);
		regfree(&n1);
		break;

	case OLEN:
		if(istype(nl->type, TMAP) || istype(nl->type, TCHAN)) {
			regalloc(&n1, types[tptr], res);
			cgen(nl, &n1);

			p1 = tbranch(ABEQ, &n1, N, -1);

			n2 = n1;
			n2.op = OINDREG;
			n2.type = types[TINT32];
			gmove(&n2, &n1);

			patch(p1, pc);

			gmove(&n1, res);
			regfree(&n1);
			break;
		}
		if(istype(nl->type, TSTRING) || isslice(nl->type)) {
			igen(nl, &n1, res);
			n1.type = types[TUINT32];
			n1.xoffset += Array_nel;
			gmove(&n1, res);
			regfree(&n1);
			break;
		}
		fatal("cgen: OLEN: unknown type %lT", nl->type);
		break;

	case OCAP:
		if(istype(nl->type, TCHAN)) {
			regalloc(&n1, types[tptr], res);
			cgen(nl, &n1);

			p1 = tbranch(ABEQ, &n1, N, -1);

			n2 = n1;
			n2.op = OINDREG;
			n2.xoffset = 4;
			n2.type = types[TINT32];
			gmove(&n2, &n1);

			patch(p1, pc);

			gmove(&n1, res);
			regfree(&n1);
			break;
		}
		if(isslice(nl->type)) {
			igen(nl, &n1, res);
			n1.type = types[TUINT32];
			n1.xoffset += Array_cap;
			gmove(&n1, res);
			regfree(&n1);
			break;
		}
		fatal("cgen: OCAP: unknown type %lT", nl->type);
		break;

	case OADDR:
		if(n->bounded)
			disable_checknil++;
		agen(nl, res);
		if(n->bounded)
			disable_checknil--;
		break;

	case OCALLMETH:
		cgen_callmeth(n, 0);
		cgen_callret(n, res);
		break;

	case OCALLFUNC:
		cgen_call(n, 0);
		cgen_callret(n, res);
		break;

	case OCALLINTER:
		cgen_callinter(n, res, 0);
		cgen_callret(n, res);
		break;

	case OMOD:
	case ODIV:
		if(isfloat[n->type->etype]) {
			a = optoas(n->op, nl->type);
			goto abop;
		}
		if(nl->ullman >= nr->ullman) {
			regalloc(&n1, nl->type, res);
			cgen(nl, &n1);
			cgen_div(n->op, &n1, nr, res);
			regfree(&n1);
		} else {
			if(!smallintconst(nr)) {
				regalloc(&n2, nr->type, res);
				cgen(nr, &n2);
			} else {
				n2 = *nr;
			}
			cgen_div(n->op, nl, &n2, res);
			if(n2.op != OLITERAL)
				regfree(&n2);
		}
		break;
	}
	return;

sbop:
	if(nl->ullman < nr->ullman ||
	(nl->ullman == nr->ullman &&
	(smallintconst(nl) || (nr->op == OLITERAL && !smallintconst(nr))))) {
		r = nl;
		nl = nr;
		nr = r;
	}

abop:
	if(nl->ullman >= nr->ullman) {
		regalloc(&n1, nl->type, res);
		cgen(nl, &n1);
		switch(n->op) {
		case OAND:
		case OOR:
		case OXOR:
		case OADD:
		case OSUB:
			if(!isfloat[nr->type->etype] && smallintconst(nr)) {
				n2 = *nr;
				break;
			}
		default:
			regalloc(&n2, nr->type, N);
			cgen(nr, &n2);
		}
	} else {
		switch(n->op) {
		case OAND:
		case OOR:
		case OXOR:
		case OADD:
		case OSUB:
			if(!isfloat[nr->type->etype] && smallintconst(nr)) {
				n2 = *nr;
				break;
			}
		default:
			regalloc(&n2, nr->type, res);
			cgen(nr, &n2);
		}
		regalloc(&n1, nl->type, N);
		cgen(nl, &n1);
	}
	if(a == AMUL || a == AMULU) {
		if(n2.op == OLITERAL) {
			regalloc(&n3, n1.type, N);
			gins(AMOVW, &n2, &n3);
			n2 = n3;
		}
		gins3(a, &n2, &n1, N);
		p1 = gins(AMOVW, N, &n1);
		p1->from.type = D_LO;
	} else
		gins(a, &n2, &n1);
	gmove(&n1, res);
	regfree(&n1);
	if(n2.op != OLITERAL)
		regfree(&n2);
	return;
}

static Prog*
cgenindex(Node *n, Node *res, int bounded)
{
	Node tmp, lo, hi, n1;
	Prog *p1;

	if(!is64(n->type)) {
		cgen(n, res);
		return nil;
	}

	tempname(&tmp, types[TINT64]);
	cgen(n, &tmp);
	split64(&tmp, &lo, &hi);
	gmove(&lo, res);
	if(bounded) {
		splitclean();
		return nil;
	}

	regalloc(&n1, types[TUINT32], N);
	gmove(&hi, &n1);
	splitclean();

	p1 = tbranch(ABNE, &n1, N, -1);
	regfree(&n1);

	return p1;
}

static void
gaddoff(vlong xoffset, Node *n)
{
	Node con, r1;


	regalloc(&r1, types[TINT32], n);
	gmove(n, &r1);

	nodconst(&con, types[TINT32], xoffset);
	gins3(AADDU, &con, &r1, &r1);
	gmove(&r1, n);
	regfree(&r1);
}

void
agen(Node *n, Node *res)
{
	Node *nl;
	Node n1, n2;
	Type *t;

	if(debug['g']) {
		dump("\nagen-res", res);
		dump("agen-r", n);
	}
	if(n == N || n->type == T || res == N || res->type == T)
		fatal("agen");

	while(n->op == OCONVNOP)
		n = n->left;

	if(isconst(n, CTNIL) && n->type->width > widthptr) {
		tempname(&n1, n->type);
		gvardef(&n1);
		clearfat(&n1);
		regalloc(&n2, types[tptr], res);
		gins(AMOVW, &n1, &n2);
		gmove(&n2, res);
		regfree(&n2);
		return;
	}
		
	if(n->addable) {
		if(0 && n->op == OREGISTER) {
			*(int*)(uintptr)0 = 0;
			fatal("agen OREGISTER");
		}
		memset(&n1, 0, sizeof n1);
		n1.op = OADDR;
		n1.left = n;
		regalloc(&n2, types[tptr], res);
		gins(AMOVW, &n1, &n2);
		gmove(&n2, res);
		regfree(&n2);
		return;
	}

	nl = n->left;

	switch(n->op) {
	default:
		fatal("agen: unknown op %+hN", n);

	case OCALLMETH:
		cgen_callmeth(n, 0);
		cgen_aret(n, res);
		break;

	case OCALLINTER:
		cgen_callinter(n, res, 0);
		cgen_aret(n, res);
		break;

	case OCALLFUNC:
		cgen_call(n, 0);
		cgen_aret(n, res);
		break;

	case OSLICE:
	case OSLICEARR:
	case OSLICESTR:
	case OSLICE3:
	case OSLICE3ARR:
		tempname(&n1, n->type);
		cgen_slice(n, &n1);
		agen(&n1, res);
		break;

	case OEFACE:
		tempname(&n1, n->type);
		cgen_eface(n, &n1);
		agen(&n1, res);
		break;

	case OINDEX:
		agenr(n, &n1, res);
		gmove(&n1, res);
		regfree(&n1);
		break;

	case ONAME:
		if(n->funcdepth > 0 && n->funcdepth != funcdepth) {
			dump("bad agen", n);
			fatal("agen: bad ONAME funcdepth %d != %d",
				n->funcdepth, funcdepth);
		}

		if(!(n->class & PHEAP) && n->class != PPARAMREF) {
			dump("bad agen", n);
			fatal("agen: bad ONAME class %#x", n->class);
		}
		cgen(n->heapaddr, res);
		if(n->xoffset != 0)
			gaddoff(n->xoffset, res);
		break;

	case OIND:
		cgen(nl, res);
		cgen_checknil(res);
		break;

	case ODOT:
		agen(nl, res);
		if(n->xoffset != 0)
			gaddoff(n->xoffset, res);
		break;

	case ODOTPTR:
		t = nl->type;
		if(!isptr[t->etype])
			fatal("agen: not ptr %N", n);
		cgen(nl, res);
		cgen_checknil(res);
		if(n->xoffset != 0)
			gaddoff(n->xoffset, res);
		break;
	}
}

void
igen(Node *n, Node *a, Node *res)
{
	Node n1;
	int r;

	if(debug['g']) {
		dump("\nigen-n", n);
	}
	switch(n->op) {
	case ONAME:
		if((n->class&PHEAP) || n->class == PPARAMREF)
			break;
		*a = *n;
		return;

	case OINDREG:
		if(n->val.u.reg != REGSP)
			reg[n->val.u.reg]++;
		*a = *n;
		return;

	case ODOT:
		igen(n->left, a, res);
		a->xoffset += n->xoffset;
		a->type = n->type;
		return;

	case ODOTPTR:
		if(n->left->addable ||
		n->left->op == OCALLFUNC ||
		n->left->op == OCALLMETH ||
		n->left->op == OCALLINTER) {
			igen(n->left, &n1, res);
			regalloc(a, types[tptr], &n1);
			gmove(&n1, a);
			regfree(&n1);
		} else {
			regalloc(a, types[tptr], res);
			cgen(n->left, a);
		}
		cgen_checknil(a);
		a->op = OINDREG;
		a->xoffset = n->xoffset;
		a->type = n->type;
		return;

	case OCALLFUNC:
	case OCALLMETH:
	case OCALLINTER:
		r = -1;
		if(n->ullman >= UINF) {
			if(res != N && (res->op == OREGISTER || res->op == OINDREG)) {
				r = res->val.u.reg;
				reg[r]--;
			}
		}
		switch(n->op) {
		case OCALLFUNC:
			cgen_call(n, 0);
			break;
		case OCALLMETH:
			cgen_callmeth(n, 0);
			break;
		case OCALLINTER:
			cgen_callinter(n, N, 0);
			break;
		}
		if(r > 0)
			reg[r]++;
		regalloc(a, types[tptr], res);
		cgen_aret(n, a);
		a->op = OINDREG;
		a->type = n->type;
		return;
	}

	agenr(n, a, res);
	a->op = OINDREG;
	a->type = n->type;
}

static void
cgenr(Node *n, Node *a, Node *res)
{
	Node n1;

	if(debug['g'])
		dump("cgenr-n", n);

	if(isfat(n->type))
		fatal("cgenr on fat node");
	if(n->addable) {
		regalloc(a, types[tptr], res);
		gmove(n, a);
		return;
	}

	switch(n->op) {
	case ONAME:
	case ODOT:
	case ODOTPTR:
	case OINDEX:
	case OCALLFUNC:
	case OCALLMETH:
	case OCALLINTER:
		igen(n, &n1, res);
		regalloc(a, types[tptr], &n1);
		gmove(&n1, a);
		regfree(&n1);
		break;
	default:
		regalloc(a, n->type, res);
		cgen(n, a);
		break;
	}
}

static void
agenr(Node *n, Node *a, Node *res)
{
	Node *nl, *nr, n1, n2, n3, n4, tmp;
	Prog *p1, *p2;
	uint32 w;
	uint64 v;
	int bounded;

	if(debug['g'])
		dump("agenr-n", n);

	nl = n->left;
	nr = n->right;
	switch(n->op) {
	default:
		regalloc(a, types[tptr], res);
		agen(n, a);
		break;
	case ODOT:
	case ODOTPTR:
	case OCALLFUNC:
	case OCALLMETH:
	case OCALLINTER:
		igen(n, &n1, res);
		regalloc(a, types[tptr], &n1);
		agen(&n1, a);
		regfree(&n1);
		break;
	case OIND:
		cgenr(n->left, a, res);
		cgen_checknil(a);
		break;
	case OINDEX:
		p2 = nil;
		w = n->type->width;
		bounded = debug['B'] || n->bounded;
		if(nr->addable) {
			if(!isconst(nr, CTINT))
				tempname(&tmp, types[TINT32]);
			if(!isconst(nl, CTSTR))
				agenr(nl, &n3, res);
			if(!isconst(nr, CTINT)) {
				p2 = cgenindex(nr, &tmp, bounded);
				regalloc(&n1, tmp.type, N);
				gmove(&tmp, &n1);
			}
		} else if(nl->addable) {
			if(!isconst(nr, CTINT)) {
				tempname(&tmp, types[TINT32]);
				p2 = cgenindex(nr, &tmp, bounded);
				regalloc(&n1, tmp.type, N);
				gmove(&tmp, &n1);
			}
			if(!isconst(nl, CTSTR))
				agenr(nl, &n3, res);
		} else {
			tempname(&tmp, types[TINT32]);
			p2 = cgenindex(nr, &tmp, bounded);
			nr = &tmp;
			if(!isconst(nl, CTSTR))
				agenr(nl, &n3, res);
			regalloc(&n1, types[TINT32], N);
			gins(AMOVW, &tmp, &n1);
		}

		
		if(isconst(nr, CTINT)) {
			if(isconst(nl, CTSTR))
				fatal("const string const index");
			v = mpgetfix(nr->val.u.xval);
			if(isslice(nl->type) || nl->type->etype == TSTRING) {
				if(!bounded) {
					n1 = n3;
					n1.op = OINDREG;
					n1.type = types[tptr];
					n1.xoffset = Array_nel;
					regalloc(&n4, n1.type, N);
					gmove(&n1, &n4);
					regalloc(&tmp, n1.type, N);
					nodconst(&n2, types[TUINT32], v);
					gmove(&n2, &tmp);
					gins3(ASGTU, &n4, &tmp, &tmp);
					p1 = tbranch(ABNE, &tmp, N, 1);
					regfree(&n4);
					regfree(&tmp);
					ginscall(panicindex, 0);
					patch(p1, pc);
				}

				n1 = n3;
				n1.op = OINDREG;
				n1.type = types[tptr];
				n1.xoffset = Array_array;
				gmove(&n1, &n3);
			}

			nodconst(&n2, types[tptr], v * w);
			gins(optoas(OADD, types[tptr]), &n2, &n3);
			*a = n3;
			break;
		}

		regalloc(&n2, types[TINT32], &n1);
		gmove(&n1, &n2);
		regfree(&n1);

		if(!bounded) {
			if(isconst(nl, CTSTR)) {
				nodconst(&n4, types[TUINT32], nl->val.u.sval->len);
			} else if(isslice(nl->type) || nl->type->etype == TSTRING) {
				n1 = n3;
				n1.op = OINDREG;
				n1.type = types[tptr];
				n1.xoffset = Array_nel;
				regalloc(&n4, types[TUINT32], N);
				gmove(&n1, &n4);
			} else {
				if(nl->type->bound < (1<<15)-1)
					nodconst(&n4, types[TUINT32], nl->type->bound);
				else {
					regalloc(&n4, types[TUINT32], N);
					gins(AMOVW, ncon(nl->type->bound), &n4);
				}
			}
			regalloc(&tmp, types[TUINT32], &n4);
			gins3(ASGTU, &n4, &n2, &tmp);
			if(n4.op == OREGISTER)
				regfree(&n4);
			p1 = tbranch(ABNE, &tmp, N, 1);
			regfree(&tmp);
			if(p2)
				patch(p2, pc);
			ginscall(panicindex, 0);
			patch(p1, pc);
		}

		if(isconst(nl, CTSTR)) {
			regalloc(&n3, types[tptr], res);
			p1 = gins(AMOVW, N, &n3);
			datastring(nl->val.u.sval->s, nl->val.u.sval->len, &p1->from);
			p1->from.type = D_CONST;
		} else if(isslice(nl->type) || nl->type->etype == TSTRING) {
			n1 = n3;
			n1.op = OINDREG;
			n1.type = types[tptr];
			n1.xoffset = Array_array;
			gmove(&n1, &n3);
		}

		if(w == 0) {
		} else if(w == 1) {
			gins(AADDU, &n2, &n3);
		} else if(w == 2 || w == 4 || w == 8 || w == 16) {
			switch(w) {
			case 2:
				w = 1;
				break;
			case 4:
				w = 2;
				break;
			case 8:
				w = 3;
				break;
			case 16:
				w = 4;
				break;
			}
			gins3(ASLL, ncon(w), &n2, &n2);
			gins(AADDU, &n2, &n3);
		} else {
			regalloc(&n4, types[TUINT32], N);
			gins(AMOVW, ncon(w), &n4);
			gins3(AMULU, &n4, &n2, N);
			p1 = gins(AMOVW, N, &n2);
			p1->from.type = D_LO;
			regfree(&n4);
			gins(AADDU, &n2, &n3);
		}

		*a = n3;
		regfree(&n2);
		break;
	}
}

void
bgen(Node *n, int true, int likely, Prog *to)
{
	int et, a;
	Node *nl, *nr, *r;
	NodeList *ll;
	Node n1, n2, tmp;
	Prog *p1, *p2;

	if(debug['g']) {
		if(true)
			dump("\nbgen, true", n);
		else
			dump("\nbgen, false", n);
	}

	if(n == N)
		n = nodbool(1);

	if(n->ninit != nil)
		genlist(n->ninit);

	if(n->type == T) {
		convlit(&n, types[TBOOL]);
		if(n->type == T)
			return;
	}

	et = n->type->etype;
	if(et != TBOOL) {
		yyerror("cgen: bad type %T for %O", n->type, n->op);
		patch(gins(AEND, N, N), to);
		return;
	}

	while(n->op == OCONVNOP) {
		n = n->left;
		if(n->ninit != nil)
			genlist(n->ninit);
	}

	nl = n->left;
	nr = N;

	switch(n->op) {
	default:
		regalloc(&n1, n->type, N);
		cgen(n, &n1);
		a = ABEQ;
		if(true)
			a = ABNE;
		patch(tbranch(a, &n1, N, 0), to);
		regfree(&n1);
		return;

	case OLITERAL:
		if(!true == !n->val.u.bval)
			gjmp(to);
		return;

	case OANDAND:
		if(!true)
			goto caseor;

	caseand:
		p1 = gjmp(P);
		p2 = gjmp(P);
		patch(p1, pc);
		bgen(n->left, !true, -likely, p2);
		bgen(n->right, !true, -likely, p2);
		gjmp(to);
		patch(p2, pc);
		return;

	case OOROR:
		if(!true)
			goto caseand;

	caseor:
		bgen(n->left, true, likely, to);
		bgen(n->right, true, likely, to);
		return;

	case OEQ:
	case ONE:
	case OLT:
	case OGT:
	case OLE:
	case OGE:
		nr = n->right;
		if(nr == N || nr->type == T)
			return;

	case ONOT:
		nl = n->left;
		if(nl == N || nl->type == T)
			return;
	}

	if(n->op == ONOT) {
		bgen(nl, !true, likely, to);
		return;
	}

	a = n->op;
	if(!true) {
		if(isfloat[nl->type->etype]) {
			p1 = gjmp(P);
			p2 = gjmp(P);
			patch(p1, pc);
			ll = n->ninit;
			n->ninit = nil;
			bgen(n, 1, -likely, p2);
			n->ninit = ll;
			gjmp(to);
			patch(p2, pc);
			return;
		}
		a = brcom(a);
		true = !true;
	}

	if(nl->ullman >= UINF && nr->ullman >= UINF) {
		tempname(&tmp, nr->type);
		cgen(nr, &tmp);
		n1 = *n;
		n1.right = &tmp;
		bgen(&n1, true, likely, to);
		return;
	}

	if(nl->op == OLITERAL) {
		a = brrev(a);
		r = nl;
		nl = nr;
		nr = r;
	}

	if(iscomplex[nl->type->etype]) {
		complexbool(a, nl, nr, true, likely, to);
		return;
	}

	if(is64(nr->type)) {
		if(!nl->addable) {
			tempname(&n1, nl->type);
			cgen(nl, &n1);
			nl = &n1;
		}
		if(!nr->addable) {
			tempname(&n2, nr->type);
			cgen(nr, &n2);
			nr = &n2;
		}
		cmp64(nl, nr, a, likely, to);
		return;
	}

	if(isslice(nl->type) || isinter(nl->type)) {
		if((a != OEQ && a != ONE) || nr->op != OLITERAL) {
			fatal("illegal slice comparison");
			return;
		}
		if(a == OEQ)
			a = ABEQ;
		else
			a = ABNE;

		igen(nl, &n1, N);

		if(isslice(nl->type))
			n1.xoffset += Array_array;
		n1.type = types[tptr];

		regalloc(&n2, types[tptr], N);
		cgen(&n1, &n2);

		patch(tbranch(a, &n2, N, likely), to);
		regfree(&n1);
		regfree(&n2);
		return;
	}

	if(nr->op == OLITERAL)
	if(isconst(nr, CTINT) || nr->val.ctype == CTNIL) {
		regalloc(&n1, nl->type, N);
		cgen(nl, &n1);
		gencmpcode(a, &n1, nr, likely, to);
		regfree(&n1);
		return;
	}


	if(nl->ullman >= UINF) {
		tempname(&tmp, nl->type);
		cgen(nl, &tmp);
		regalloc(&n1, nl->type, N);
		gmove(&tmp, &n1);
		regalloc(&n2, nr->type, N);
		cgen(nr, &n2);
	} else if(nl->ullman >= nr->ullman) {
		regalloc(&n1, nl->type, N);
		cgen(nl, &n1);
		regalloc(&n2, nr->type, N);
		cgen(nr, &n2);
	} else {
		regalloc(&n2, nr->type, N);
		cgen(nr, &n2);
		regalloc(&n1, nl->type, N);
		cgen(nl, &n1);
	}

	gencmpcode(a, &n1, &n2, likely, to);

	regfree(&n1);
	regfree(&n2);
}

int32
stkof(Node *n)
{
	Type *t;
	Iter flist;
	int32 off;

	switch(n->op) {
	case OINDREG:
		return n->xoffset;

	case ODOT:
		t = n->left->type;
		if(isptr[t->etype])
			break;
		off = stkof(n->left);
		if(off == -1000 || off == 1000)
			return off;
		return off + n->xoffset;

	case OINDEX:
		t = n->left->type;
		if(!isfixedarray(t))
			break;
		off = stkof(n->left);
		if(off == -1000 || off == 1000)
			return off;
		if(isconst(n->right, CTINT))
			return off + t->type->width * mpgetfix(n->right->val.u.xval);
		return 1000;
		
	case OCALLMETH:
	case OCALLINTER:
	case OCALLFUNC:
		t = n->left->type;
		if(isptr[t->etype])
			t = t->type;

		t = structfirst(&flist, getoutarg(t));
		if(t != T)
			return t->width;
		break;
	}

	return -1000;
}

static Prog*
gload(int as, Node *s, int off, Node *reg)
{
	Prog *p;

	p = gins(as, s, reg);
	p->from.type = D_OREG;
	p->from.offset = off;

	return p;
}

static Prog*
gstore(int as, Node *reg, Node *d, int off)
{
	Prog *p;

	p = gins(as, reg, d);
	p->to.type = D_OREG;
	p->to.offset = off;

	return p;
}

void
sgen(Node *n, Node *res, int64 w)
{
	Node dst, src, tdst, tsrc, end, reg[6], tmp[4], *f;
	Node save[6];
	int32 c, odst, osrc;
	int i, dir, align, as;
	NodeList *l;
	Prog *p0;

	if(debug['g']) {
		print("\nsgen w=%lld\n", w);
		dump("r", n);
		dump("res", res);
	}
	if(n->ullman >= UINF && res->ullman >= UINF)
		fatal("sgen UINF");

	if(w < 0 || (int32)w != w)
		fatal("sgen copy %lld", w);

	if(w == 0) {
		tempname(&tdst, types[tptr]);
		agen(res, &tdst);
		agen(n, &tdst);
		return;
	}

	if(res->op == ONAME && strcmp(res->sym->name, ".args") == 0)
		for(l = curfn->dcl; l != nil; l = l->next)
			if(l->n->class == PPARAMOUT)
				gvardef(l->n);

	if(componentgen(n, res))
		return;

	align = n->type->align;
	switch(align) {
	default:
		fatal("sgen: invalid alignment %d for %T", align, n->type);
	case 1:
		as = AMOVBU;
		break;
	case 2:
		as = AMOVHU;
		break;
	case 4:
		as = AMOVW;
		break;
	}
	if(w%align)
		fatal("sgen: unaligned size %lld (align=%d) for %T", w, align, n->type);
	c = w/align;

	osrc = stkof(n);
	odst = stkof(res);
	
	if(osrc != -1000 && odst != -1000)
	if(osrc == 1000 || odst == 1000) {
		tempname(&tsrc, n->type);
		sgen(n, &tsrc, w);
		sgen(&tsrc, res, w);
		return;
	}
	if(osrc%align != 0 || odst%align != 0)
		fatal("sgen: unaligned offset src %d or dst %d (align %d)",
		osrc, odst, align);

	dir = align;
	if(osrc < odst && odst < osrc+w)
		dir = -dir;

	if(0 && as == AMOVW && dir > 0 && c >= 4 && c <= 128) {
		for(i=0; i<6; i++) {
			reg[i].op = OREGISTER;
			reg[i].val.u.reg = REGRT1 + i;
			regalloc(&save[i], types[tptr], N);
		}
		regalloc(&src, types[tptr], &reg[4]);
		regalloc(&dst, types[tptr], &reg[5]);
		gins(AMOVW, &src, &save[4]);
		gins(AMOVW, &dst, &save[5]);
		if(n->ullman >= res->ullman) {
			agen(n, &src);
			if(res->op == ONAME)
				gvardef(res);
			agen(res, &dst);
		} else {
			if(res->op == ONAME)
				gvardef(res);
			agen(res, &dst);
			agen(n, &src);
		}
		gins(AADDU, nscon(16*(c/4-128/4)), &src);
		gins(AADDU, nscon(16*(c/4-128/4)), &dst);
		for(i=0; i<4; i++) {
			regalloc(&tmp[i], types[tptr], &reg[i]);
			gins(AMOVW, &tmp[i], &save[i]);
		}

		f = sysfunc("duffcopy");
		p0 = gins(ADUFFCOPY, N, f);
		afunclit(&p0->to, f);
		p0->to.offset = 8*4*(128/4-c/4);

		for(i=0; i<(c&3); i++) {
			p0 = gins(AMOVW, N, N);
			p0->from.type = D_OREG;
			p0->from.reg = src.val.u.reg;
			p0->from.offset = 512 + i*4;
			p0->to.type = D_REG;
			p0->to.reg = REGRT1 + i;
		}
		for(i=0; i<(c&3); i++) {
			p0 = gins(AMOVW, N, N);
			p0->from.type = D_REG;
			p0->from.reg = REGRT1 + i;
			p0->to.type = D_OREG;
			p0->to.reg = dst.val.u.reg;
			p0->to.offset = 512 + i*4;
		}

		for(i=0; i<4; i++) {
			gins(AMOVW, &save[i], &tmp[i]);
			regfree(&save[i]);
			regfree(&tmp[i]);
		}
		gins(AMOVW, &save[4], &src);
		gins(AMOVW, &save[5], &dst);
		regfree(&save[4]);
		regfree(&save[5]);
		regfree(&src);
		regfree(&dst);
		return;
	}

	if(n->ullman >= res->ullman) {
		agenr(n, &dst, res);
		regalloc(&src, types[tptr], N);
		gins(AMOVW, &dst, &src);
		if(res->op == ONAME)
			gvardef(res);
		agen(res, &dst);
	} else {
		if(res->op == ONAME)
			gvardef(res);
		agenr(res, &dst, res);
		agenr(n, &src, N);
	}

	if(dir<0) {
		dir = -dir;
		if(c < 4) {
			for(i=0; i<c; i++) {
				regalloc(&tmp[i], types[TUINT32], N);
				gload(as, &src, (c-i-1)*dir, &tmp[i]);
			}
			for(i=0; i<c; i++) {
				gstore(as, &tmp[i], &dst, (c-i-1)*dir);
				regfree(&tmp[i]);
			}
		} else {
			regalloc(&end, types[tptr], N);
			regalloc(&tmp[0], types[TUINT32], N);
			gins3(AADDU, ncon(w), &src, &end);
			p0 = gload(as, &end, -dir, &tmp[0]);
			gstore(as, &tmp[0], &dst, w-dir);
			gins(AADDU, nscon(-dir), &end);
			gins(AADDU, nscon(-dir), &dst);
			patch(tbranch(ABNE, &end, &src, 1), p0);
			regfree(&end);
			regfree(&tmp[0]);
		}
	} else {
		if(c < 4) {
			for(i=0; i<c; i++) {
				regalloc(&tmp[i], types[TUINT32], N);
				gload(as, &src, i*dir, &tmp[i]);
			}
			for(i=0; i<c; i++) {
				gstore(as, &tmp[i], &dst, i*dir);
				regfree(&tmp[i]);
			}
		} else {
			regalloc(&end, types[tptr], N);
			regalloc(&tmp[0], types[TUINT32], N);

			gins3(AADDU, ncon(w), &src, &end);
			p0 = gload(as, &src, 0, &tmp[0]);
			gstore(as, &tmp[0], &dst, 0);
			gins(AADDU, ncon(dir), &src);
			gins(AADDU, ncon(dir), &dst);
			patch(tbranch(ABNE, &src, &end, 1), p0);

			regfree(&end);
			regfree(&tmp[0]);
		}
	}

	regfree(&dst);
	regfree(&src);
}

static int
cadable(Node *n)
{
	if(!n->addable) {
		return 0;
	}

	switch(n->op) {
	case ONAME:
		return 1;
	}
	return 0;
}

int
componentgen(Node *nr, Node *nl)
{
	Node nodl, nodr;
	Type *t;
	int freel, freer;
	vlong fldcount;
	vlong loffset, roffset;

	freel = 0;
	freer = 0;

	switch(nl->type->etype) {
	default:
		goto no;

	case TARRAY:
		t = nl->type;

		if(isslice(t))
			break;
		if(is64(t->type) && t->bound > 0 && t->bound <=3)
			break;
		if(t->bound > 0 && t->bound <= 5 && !isfat(t->type))
			break;

		goto no;

	case TSTRUCT:
		fldcount = 0;
		for(t=nl->type->type; t; t=t->down) {
			if(isfat(t->type))
				goto no;
			if(t->etype != TFIELD)
				fatal("componentgen: not a TFIELD: %lT", t);
			fldcount++;
			if(is64(t->type))
					fldcount++;
		}
		if(fldcount == 0 || fldcount > 5)
			goto no;

		break;

	case TSTRING:
	case TINTER:
		break;
	}

	nodl = *nl;
	if(!cadable(nl)) {
		if(nr == N || !cadable(nr))
			goto no;
		igen(nl, &nodl, N);
		freel = 1;
	}

	if(nr != N) {
		nodr = *nr;
		if(!cadable(nr)) {
			igen(nr, &nodr, N);
			freer = 1;
		}
	}
	
	if(nr != N && nl->op == ONAME && nr->op == ONAME && nl == nr)
		goto yes;

	switch(nl->type->etype) {
	case TARRAY:
		if(nl->op == ONAME)
			gvardef(nl);
		t = nl->type;
		if(!isslice(t)) {
			nodl.type = t->type;
			nodr.type = nodl.type;
			for(fldcount=0; fldcount < t->bound; fldcount++) {
				if(nr == N)
					clearslim(&nodl);
				else
					gmove(&nodr, &nodl);
				nodl.xoffset += t->type->width;
				nodr.xoffset += t->type->width;
			}
			goto yes;
		}

		nodl.xoffset += Array_array;
		nodl.type = ptrto(nl->type->type);

		if(nr != N) {
			nodr.xoffset += Array_array;
			nodr.type = nodl.type;
		} else
			nodconst(&nodr, nodl.type, 0);
		gmove(&nodr, &nodl);

		nodl.xoffset += Array_nel-Array_array;
		nodl.type = types[simtype[TUINT]];

		if(nr != N) {
			nodr.xoffset += Array_nel-Array_array;
			nodr.type = nodl.type;
		} else
			nodconst(&nodr, nodl.type, 0);
		gmove(&nodr, &nodl);

		nodl.xoffset += Array_cap-Array_nel;
		nodl.type = types[simtype[TUINT]];

		if(nr != N) {
			nodr.xoffset += Array_cap-Array_nel;
			nodr.type = nodl.type;
		} else
			nodconst(&nodr, nodl.type, 0);
		gmove(&nodr, &nodl);

		goto yes;

	case TSTRING:
		if(nl->op == ONAME)
			gvardef(nl);
		nodl.xoffset += Array_array;
		nodl.type = ptrto(types[TUINT8]);

		if(nr != N) {
			nodr.xoffset += Array_array;
			nodr.type = nodl.type;
		} else
			nodconst(&nodr, nodl.type, 0);
		gmove(&nodr, &nodl);

		nodl.xoffset += Array_nel-Array_array;
		nodl.type = types[simtype[TUINT]];

		if(nr != N) {
			nodr.xoffset += Array_nel-Array_array;
			nodr.type = nodl.type;
		} else
			nodconst(&nodr, nodl.type, 0);
		gmove(&nodr, &nodl);

		goto yes;

	case TINTER:
		if(nl->op == ONAME)
			gvardef(nl);
		nodl.xoffset += Array_array;
		nodl.type = ptrto(types[TUINT8]);

		if(nr != N) {
			nodr.xoffset += Array_array;
			nodr.type = nodl.type;
		} else
			nodconst(&nodr, nodl.type, 0);
		gmove(&nodr, &nodl);

		nodl.xoffset += Array_nel-Array_array;
		nodl.type = ptrto(types[TUINT8]);

		if(nr != N) {
			nodr.xoffset += Array_nel-Array_array;
			nodr.type = nodl.type;
		} else
			nodconst(&nodr, nodl.type, 0);
		gmove(&nodr, &nodl);

		goto yes;

	case TSTRUCT:
		if(nl->op == ONAME)
			gvardef(nl);
		loffset = nodl.xoffset;
		roffset = nodr.xoffset;
		if(nl->type->etype == TSTRUCT && nl->type->funarg && nl->type->type)
			loffset -= nl->type->type->width;
		if(nr != N && nr->type->etype == TSTRUCT && nr->type->funarg && nr->type->type)
			roffset -= nr->type->type->width;

		for(t=nl->type->type; t; t=t->down) {
			nodl.xoffset = loffset + t->width;
			nodl.type = t->type;

			if(nr == N)
				clearslim(&nodl);
			else {
				nodr.xoffset = roffset + t->width;
				nodr.type = nodl.type;
				gmove(&nodr, &nodl);
			}
		}
		goto yes;
	}

no:
	if(freer)
		regfree(&nodr);
	if(freel)
		regfree(&nodl);
	return 0;

yes:
	if(freer)
		regfree(&nodr);
	if(freel)
		regfree(&nodl);
	return 1;
}
