#include "gc.h"
#include "../../runtime/funcdata.h"

static void	cmpv(Node*, int);
static void	testv(Node*, int);
static void	cgen64(Node*, Node*);
static int	isvconstable(int, vlong);

void
cgen(Node *n, Node *nn)
{
	Node *l, *r;
	Prog *p1;
	Node nod, nod1, nod2, nod3, nod4;
	int o;
	int32 v, curs;

	if(debug['g']) {
		prtree(nn, "cgen lhs");
		prtree(n, "cgen");
	}
	if(n == Z || n->type == T)
		return;

	if(typesu[n->type->etype] && (n->op != OFUNC || nn != Z)) {
		sugen(n, nn, n->type->width);
		return;
	}
	if(typev[n->type->etype]) {
		switch(n->op) {
		case OFUNC:
			if(hasdotdotdot(n->left->type)) {
				cgen64(n, nn);
				return;
			}
			break;
		case OCONST:
			cgen64(n, nn);
			return;
		}
	}
	l = n->left;
	r = n->right;
	o = n->op;
	if(n->addable >= INDEXED) {
		if(nn == Z) {
			switch(o) {
			default:
				nullwarn(Z, Z);
				break;
			case OINDEX:
				nullwarn(l, r);
				break;
			}
			return;
		}
		gmove(n, nn);
		return;
	}
	curs = cursafe;

	if(n->complex >= FNX)
	if(l->complex >= FNX)
	if(r != Z && r->complex >= FNX)
	switch(o) {
	default:
		if(!typev[r->type->etype]) {
			regret(&nod, r, 0, 0);
			cgen(r, &nod);
			regsalloc(&nod1, r);
			gmove(&nod, &nod1);
			regfree(&nod);
		} else {
			regsalloc(&nod1, r);
			cgen(r, &nod1);
		}
		nod = *n;
		nod.right = &nod1;
		cgen(&nod, nn);
		return;

	case OFUNC:
	case OCOMMA:
	case OANDAND:
	case OOROR:
	case OCOND:
	case ODOT:
		break;
	}

	switch(o) {
	default:
		diag(n, "unknown op in cgen: %O", o);
		break;

	case OCOM:
		if(nn == Z) {
			nullwarn(l, Z);
			break;
		}
		regalloc(&nod, l, nn);
		cgen(l, &nod);
		gopcode(o, &nod, nodconst(0), &nod);
		gmove(&nod, nn);
		regfree(&nod);
		break;

	case OAS:
		if(l->op == OBIT)
			goto bitas;
		if(l->addable >= INDEXED) {
			if(nn != Z || r->addable < INDEXED) {
				regalloc(&nod, r, nn);
				cgen(r, &nod);
				gmove(&nod, l);
				if(nn != Z)
					gmove(&nod, nn);
				regfree(&nod);
			} else
				gmove(r, l);
			break;
		}
		if(l->complex >= r->complex) {
			reglcgen(&nod1, l, Z);
			if(r->addable >= INDEXED) {
				gmove(r, &nod1);
				if(nn != Z)
					gmove(&nod1, nn);
				regfree(&nod1);
				break;
			}
			regalloc(&nod, r, nn);
			cgen(r, &nod);
		} else {
			regalloc(&nod, r, nn);
			cgen(r, &nod);
			reglcgen(&nod1, l, Z);
		}
		gmove(&nod, &nod1);
		regfree(&nod);
		regfree(&nod1);
		break;

	bitas:
		n = l->left;
		regalloc(&nod, r, nn);
		if(l->complex >= r->complex) {
			reglcgen(&nod1, n, Z);
			cgen(r, &nod);
		} else {
			cgen(r, &nod);
			reglcgen(&nod1, n, Z);
		}
		regalloc(&nod2, n, Z);
		gopcode(OAS, &nod1, Z, &nod2);
		bitstore(l, &nod, &nod1, &nod2, nn);
		break;

	case OBIT:
		if(nn == Z) {
			nullwarn(l, Z);
			break;
		}
		bitload(n, &nod, Z, Z, nn);
		gopcode(OAS, &nod, Z, nn);
		regfree(&nod);
		break;

	case OADD:
	case OSUB:
	case OAND:
	case OOR:
	case OXOR:
	case OLSHR:
	case OASHL:
	case OASHR:
		if(nn != Z && r->op == OCONST && !typefd[n->type->etype])
		if(!typev[n->type->etype] || isvconstable(o, r->vconst)) {
			regalloc(&nod, l, nn);
			cgen(l, &nod);
			if(o == OAND || r->vconst != 0)
				gopcode(o, r, Z, &nod);
			gmove(&nod, nn);
			regfree(&nod);
			break;
		}
	case ODIV:
	case OMOD:
	case OMUL:
	case OLMUL:
	case OLDIV:
	case OLMOD:
		if(nn == Z) {
			nullwarn(l, r);
			break;
		}
		if(o == OMUL || o == OLMUL)
		if(!typev[n->type->etype]) {
			if(mulcon(n, nn))
				break;
			if(debug['M'])
				print("%L multiply\n", n->lineno);
		}
		if(l->complex >= r->complex) {
			regalloc(&nod, l, nn);
			cgen(l, &nod);
			regalloc(&nod1, r, Z);
			cgen(r, &nod1);
			gopcode(o, &nod1, Z, &nod);
		} else {
			regalloc(&nod, r, nn);
			cgen(r, &nod);
			regalloc(&nod1, l, Z);
			cgen(l, &nod1);
			gopcode(o, &nod, &nod1, &nod);
		}
		gopcode(OAS, &nod, Z, nn);
		regfree(&nod);
		regfree(&nod1);
		break;

	case OASLSHR:
	case OASASHL:
	case OASASHR:
	case OASAND:
	case OASADD:
	case OASSUB:
	case OASXOR:
	case OASOR:
		if(l->op == OBIT)
			goto asbitop;
		if(r->op == OCONST)
		if(!typefd[r->type->etype] && !typefd[n->type->etype])
		if(!typev[n->type->etype] || isvconstable(o, r->vconst)) {
			if(l->addable < INDEXED)
				reglcgen(&nod2, l, Z);
			else
				nod2 = *l;
			regalloc(&nod, l, nn);
			gopcode(OAS, &nod2, Z, &nod);
			gopcode(o, r, Z, &nod);
			gopcode(OAS, &nod, Z, &nod2);

			regfree(&nod);
			if(l->addable < INDEXED)
				regfree(&nod2);
			break;
		}

	case OASLMUL:
	case OASLDIV:
	case OASLMOD:
	case OASMUL:
	case OASDIV:
	case OASMOD:
		if(l->op == OBIT)
			goto asbitop;
		if(l->complex >= r->complex) {
			if(l->addable < INDEXED)
				reglcgen(&nod2, l, Z);
			else
				nod2 = *l;
			regalloc(&nod1, r, Z);
			cgen(r, &nod1);
		} else {
			regalloc(&nod1, r, Z);
			cgen(r, &nod1);
			if(l->addable < INDEXED)
				reglcgen(&nod2, l, Z);
			else
				nod2 = *l;
		}

		regalloc(&nod, n, nn);
		gmove(&nod2, &nod);
		gopcode(o, &nod1, Z, &nod);
		gmove(&nod, &nod2);
		if(nn != Z)
			gopcode(OAS, &nod, Z, nn);
		regfree(&nod);
		regfree(&nod1);
		if(l->addable < INDEXED)
			regfree(&nod2);
		break;

	asbitop:
		regalloc(&nod4, n, nn);
		regalloc(&nod3, r, Z);
		if(l->complex >= r->complex) {
			bitload(l, &nod, &nod1, &nod2, &nod4);
			cgen(r, &nod3);
		} else {
			cgen(r, &nod3);
			bitload(l, &nod, &nod1, &nod2, &nod4);
		}
		gmove(&nod, &nod4);
		gopcode(o, &nod3, Z, &nod4);
		regfree(&nod3);
		gmove(&nod4, &nod);
		regfree(&nod4);
		bitstore(l, &nod, &nod1, &nod2, nn);
		break;

	case OADDR:
		if(nn == Z) {
			nullwarn(l, Z);
			break;
		}
		lcgen(l, nn);
		break;

	case OFUNC:
		if(l->complex >= FNX) {
			if(l->op != OIND)
				diag(n, "bad function call");

			regret(&nod, l->left, 0, 0);
			cgen(l->left, &nod);
			regsalloc(&nod1, l->left);
			gopcode(OAS, &nod, Z, &nod1);
			regfree(&nod);

			nod = *n;
			nod.left = &nod2;
			nod2 = *l;
			nod2.left = &nod1;
			nod2.complex = 1;
			cgen(&nod, nn);

			return;
		}
		if(REGARG>0)
			o = reg[REGARG];
		gargs(r, &nod, &nod1);
		if(l->addable < INDEXED) {
			reglcgen(&nod, l, Z);
			gopcode(OFUNC, Z, Z, &nod);
			regfree(&nod);
		} else
			gopcode(OFUNC, Z, Z, l);
		if(REGARG > 0)
			if(o != reg[REGARG])
				reg[REGARG]--;
		regret(&nod, n, l->type, 1);
		if(nn != Z)
			gopcode(OAS, &nod, Z, nn);
		if(nod.op == OREGISTER)
			regfree(&nod);
		break;

	case OIND:
		if(nn == Z) {
			nullwarn(l, Z);
			break;
		}
		regialloc(&nod, n, nn);
		r = l;
		while(r->op == OADD)
			r = r->right;
		if(sconst(r)) {
			v = r->vconst;
			r->vconst = 0;
			cgen(l, &nod);
			nod.xoffset += v;
			r->vconst = v;
		} else
			cgen(l, &nod);
		regind(&nod, n);
		gopcode(OAS, &nod, Z, nn);
		regfree(&nod);
		break;

	case OEQ:
	case ONE:
	case OLE:
	case OLT:
	case OGE:
	case OGT:
	case OLO:
	case OLS:
	case OHI:
	case OHS:
		if(nn == Z) {
			nullwarn(l, r);
			break;
		}
		boolgen(n, 1, nn);
		break;

	case OANDAND:
	case OOROR:
		boolgen(n, 1, nn);
		if(nn == Z)
			patch(p, pc);
		break;

	case ONOT:
		if(nn == Z) {
			nullwarn(l, Z);
			break;
		}
		boolgen(n, 1, nn);
		break;

	case OCOMMA:
		cgen(l, Z);
		cgen(r, nn);
		break;

	case OCAST:
		if(nn == Z) {
			nullwarn(l, Z);
			break;
		}
		if(nocast(l->type, n->type) && nocast(n->type, nn->type)) {
			cgen(l, nn);
			break;
		}
		if(typev[l->type->etype] || typev[n->type->etype]) {
			cgen64(n, nn);
			break;
		}
		regalloc(&nod, l, nn);
		cgen(l, &nod);
		regalloc(&nod1, n, &nod);
		gopcode(OAS, &nod, Z, &nod1);
		gopcode(OAS, &nod1, Z, nn);
		regfree(&nod1);
		regfree(&nod);
		break;

	case ODOT:
		sugen(l, nodrat, l->type->width);
		if(nn != Z) {
			warn(n, "non-interruptable temporary");
			nod = *nodrat;
			if(!r || r->op != OCONST) {
				diag(n, "DOT and no offset");
				break;
			}
			nod.xoffset += (int32)r->vconst;
			nod.type = n->type;
			cgen(&nod, nn);
		}
		break;

	case OCOND:
		bcgen(l, 1);
		p1 = p;
		cgen(r->left, nn);
		gbranch(OGOTO);
		patch(p1, pc);
		p1 = p;
		cgen(r->right, nn);
		patch(p1, pc);
		break;

	case OPOSTINC:
	case OPOSTDEC:
		v = 1;
		if(l->type->etype == TIND)
			v = l->type->link->width;
		if(o == OPOSTDEC)
			v = -v;
		if(l->op == OBIT)
			goto bitinc;
		if(nn == Z)
			goto pre;

		if(l->addable < INDEXED)
			reglcgen(&nod2, l, Z);
		else
			nod2 = *l;

		regalloc(&nod, l, nn);
		gopcode(OAS, &nod2, Z, &nod);
		regalloc(&nod1, l, Z);
		if(typefd[l->type->etype]) {
			regalloc(&nod3, l, Z);
			if(v < 0) {
				gopcode(OAS, nodfconst(-v), Z, &nod3);
				gopcode(OSUB, &nod3, &nod, &nod1);
			} else {
				gopcode(OAS, nodfconst(v), Z, &nod3);
				gopcode(OADD, &nod3, &nod, &nod1);
			}
			regfree(&nod3);
		} else
			gopcode(OADD, nodconst(v), &nod, &nod1);
		gopcode(OAS, &nod1, Z, &nod2);

		regfree(&nod);
		regfree(&nod1);
		if(l->addable < INDEXED)
			regfree(&nod2);
		break;

	case OPREINC:
	case OPREDEC:
		v = 1;
		if(l->type->etype == TIND)
			v = l->type->link->width;
		if(o == OPREDEC)
			v = -v;
		if(l->op == OBIT)
			goto bitinc;

	pre:
		if(l->addable < INDEXED)
			reglcgen(&nod2, l, Z);
		else
			nod2 = *l;

		regalloc(&nod, l, nn);
		gopcode(OAS, &nod2, Z, &nod);
		if(typefd[l->type->etype]) {
			regalloc(&nod3, l, Z);
			if(v < 0) {
				gopcode(OAS, nodfconst(-v), Z, &nod3);
				gopcode(OSUB, &nod3, Z, &nod);
			} else {
				gopcode(OAS, nodfconst(v), Z, &nod3);
				gopcode(OADD, &nod3, Z, &nod);
			}
			regfree(&nod3);
		} else
			gopcode(OADD, nodconst(v), Z, &nod);
		gopcode(OAS, &nod, Z, &nod2);
		if(nn && l->op == ONAME)
			gins(ANOP, l, Z);

		regfree(&nod);
		if(l->addable < INDEXED)
			regfree(&nod2);
		break;

	bitinc:
		if(nn != Z && (o == OPOSTINC || o == OPOSTDEC)) {
			bitload(l, &nod, &nod1, &nod2, Z);
			gopcode(OAS, &nod, Z, nn);
			gopcode(OADD, nodconst(v), Z, &nod);
			bitstore(l, &nod, &nod1, &nod2, Z);
			break;
		}
		bitload(l, &nod, &nod1, &nod2, nn);
		gopcode(OADD, nodconst(v), Z, &nod);
		bitstore(l, &nod, &nod1, &nod2, nn);
		break;
	}
	cursafe = curs;
}

void
reglcgen(Node *t, Node *n, Node *nn)
{
	Node *r;
	int32 v;

	regialloc(t, n, nn);
	if(n->op == OIND) {
		r = n->left;
		while(r->op == OADD)
			r = r->right;
		if(sconst(r)) {
			v = r->vconst;
			r->vconst = 0;
			lcgen(n, t);
			t->xoffset += v;
			r->vconst = v;
			regind(t, n);
			return;
		}
	}
	lcgen(n, t);
	regind(t, n);
}

void
lcgen(Node *n, Node *nn)
{
	Prog *p1;
	Node nod;

	if(debug['g']) {
		prtree(nn, "lcgen lhs");
		prtree(n, "lcgen");
	}
	if(n == Z || n->type == T)
		return;
	if(nn == Z) {
		nn = &nod;
		regalloc(&nod, n, Z);
	}
	switch(n->op) {
	default:
		if(n->addable < INDEXED) {
			diag(n, "unknown op in lcgen: %O", n->op);
			break;
		}
		nod = *n;
		nod.op = OADDR;
		nod.left = n;
		nod.right = Z;
		nod.type = types[TIND];
		gopcode(OAS, &nod, Z, nn);
		break;

	case OCOMMA:
		cgen(n->left, n->left);
		lcgen(n->right, nn);
		break;

	case OIND:
		cgen(n->left, nn);
		break;

	case OCOND:
		bcgen(n->left, 1);
		p1 = p;
		lcgen(n->right->left, nn);
		gbranch(OGOTO);
		patch(p1, pc);
		p1 = p;
		lcgen(n->right->right, nn);
		patch(p1, pc);
		break;
	}
}

void
bcgen(Node *n, int true)
{

	if(n->type == T)
		gbranch(OGOTO);
	else
		boolgen(n, true, Z);
}

static uchar arel[12] =
{
	OXXX, OXXX, OLE, OLS, OLE, OLS, OGT, OHI, OGT, OHI,
};

static uchar brel[12] =
{
	OXXX, OXXX, OLT, OLO, OLT, OLO, OGE, OHS, OGE, OHS,
};

void
boolgen(Node *n, int true, Node *nn)
{
	int o, o1;
	Prog *p1, *p2;
	Node *l, *r, nod, nod1;
	int32 curs;

	if(debug['g']) {
		prtree(nn, "boolgen lhs");
		prtree(n, "boolgen");
	}
	curs = cursafe;
	l = n->left;
	r = n->right;
	switch(n->op) {

	default:
		if(typev[n->type->etype]) {
			testv(n, true);
			goto com;
		}
		regalloc(&nod, n, nn);
		cgen(n, &nod);
		if(nn == Z || typefd[n->type->etype]) {
			o = ONE;
			if(true)
				o = comrel[relindex(o)];
			if(typefd[n->type->etype]) {
				nodreg(&nod1, n, NREG+FREGZERO);
				gopcode(o, &nod, &nod1, Z);
			} else
				gopcode(o, &nod, Z, Z);
			regfree(&nod);
			goto com;
		}
		if(true)
			gopcode(OCOND, &nod, nodconst(0), &nod);
		else
			gopcode(OCOND, nodconst(1), &nod, &nod);
		gopcode(OAS, &nod, Z, nn);
		regfree(&nod);
		break;

	case OCONST:
		o = vconst(n);
		if(!true)
			o = !o;
		gbranch(OGOTO);
		if(o) {
			p1 = p;
			gbranch(OGOTO);
			patch(p1, pc);
		}
		goto com;

	case OCOMMA:
		cgen(l, Z);
		boolgen(r, true, nn);
		break;

	case ONOT:
		boolgen(l, !true, nn);
		break;

	case OCOND:
		bcgen(l, 1);
		p1 = p;
		bcgen(r->left, true);
		p2 = p;
		gbranch(OGOTO);
		patch(p1, pc);
		p1 = p;
		bcgen(r->right, !true);
		patch(p2, pc);
		p2 = p;
		gbranch(OGOTO);
		patch(p1, pc);
		patch(p2, pc);
		goto com;

	case OANDAND:
		if(!true)
			goto caseor;

	caseand:
		bcgen(l, true);
		p1 = p;
		bcgen(r, !true);
		p2 = p;
		patch(p1, pc);
		gbranch(OGOTO);
		patch(p2, pc);
		goto com;

	case OOROR:
		if(!true)
			goto caseand;

	caseor:
		bcgen(l, !true);
		p1 = p;
		bcgen(r, !true);
		p2 = p;
		gbranch(OGOTO);
		patch(p1, pc);
		patch(p2, pc);
		goto com;

	case OEQ:
	case ONE:
	case OLE:
	case OLT:
	case OGE:
	case OGT:
	case OHI:
	case OHS:
	case OLO:
	case OLS:
		if(typev[l->type->etype]) {
			cmpv(n, true);
			goto com;
		}

		if(l->complex >= FNX && r->complex >= FNX) {
			regret(&nod, r, 0, 0);
			cgen(r, &nod);
			regsalloc(&nod1, r);
			gopcode(OAS, &nod, Z, &nod1);
			regfree(&nod);
			nod = *n;
			nod.right = &nod1;
			boolgen(&nod, true, nn);
			break;
		}
		
		o = n->op;

		o1 = OCOMMA;
		if(o == OLS || o == OHI || o == OLO || o == OHS)
			o1 = OCOND;

		if(nn != Z && !typefd[l->type->etype]) {
			if(l->complex >= r->complex) {
				regalloc(&nod1, l, nn);
				cgen(l, &nod1);
				regalloc(&nod, r, Z);
				cgen(r, &nod);
			} else {
				regalloc(&nod, r, nn);
				cgen(r, &nod);
				regalloc(&nod1, l, Z);
				cgen(l, &nod1);
			}
			switch(o) {
			case OEQ:
				gopcode(OSUB, &nod1, &nod, &nod);
				gopcode(OCOND, nodconst(1), &nod, &nod);
				break;
			case ONE:
				gopcode(OSUB, &nod1, &nod, &nod);
				gopcode(OCOND, &nod, nodconst(0), &nod);
				break;
			case OLE:
			case OLS:
				gopcode(o1, &nod1, &nod, &nod);
				gopcode(OXOR, nodconst(1), &nod, &nod);
				break;
			case OGE:
			case OHS:
				gopcode(o1, &nod, &nod1, &nod);
				gopcode(OXOR, nodconst(1), &nod, &nod);
				break;
			case OGT:
			case OHI:
				gopcode(o1, &nod1, &nod, &nod);
				break;
			case OLT:
			case OLO:
				gopcode(o1, &nod, &nod1, &nod);
				break;
			}
			gopcode(OAS, &nod, Z, nn);
			regfree(&nod);
			regfree(&nod1);
			break;
		}


		if(true)
		if(typefd[l->type->etype] && (o==OEQ || o==ONE)) {
			boolgen(n, 0, Z);
			p1 = p;
			gbranch(OGOTO);
			patch(p1, pc);
			goto com;
		}

		if(true)
			o = comrel[relindex(o)];

		if(sconst(l)) {
			switch(o) {
			default:
				if(l->vconst != 0)
					break;

				if(o == OEQ || o == ONE)
				if(l->vconst != 0)
					break;
				if(l->vconst != 0) {
					l->vconst++;
					if(!sconst(l)) {
						l->vconst--;
						break;
					}
					o = arel[relindex(o)];
				}
			case OGT:
			case OHI:
			case OLE:
			case OLS:
				regalloc(&nod, r, nn);
				cgen(r, &nod);
				gopcode(o, l, &nod, Z);
				regfree(&nod);
				goto com;
			}
		}
		if(sconst(r)) {
			switch(o) {
			default:
				if(r->vconst != 0)
					break;

				if(o == OEQ || o == ONE)
				if(r->vconst != 0)
					break;
				if(r->vconst != 0) {
					break;
					r->vconst++;
					if(!sconst(r)) {
						r->vconst--;
						break;
					}
					o = brel[relindex(o)];
				}
			case OGE:
			case OHS:
			case OLT:
			case OLO:
				regalloc(&nod, l, nn);
				cgen(l, &nod);
				gopcode(o, &nod, r, Z);
				regfree(&nod);
				goto com;
			}
		}
		if(l->complex >= r->complex) {
			regalloc(&nod1, l, nn);
			cgen(l, &nod1);
			regalloc(&nod, r, Z);
			cgen(r, &nod);
		} else {
			regalloc(&nod, r, nn);
			cgen(r, &nod);
			regalloc(&nod1, l, Z);
			cgen(l, &nod1);
		}
		gopcode(o, &nod1, &nod, Z);
		regfree(&nod);
		regfree(&nod1);

	com:
		if(nn != Z) {
			p1 = p;
			gopcode(OAS, nodconst(1), Z, nn);
			gbranch(OGOTO);
			p2 = p;
			patch(p1, pc);
			gopcode(OAS, nodconst(0), Z, nn);
			patch(p2, pc);
		}
		break;
	}
	cursafe = curs;
}

void
sugen(Node *n, Node *nn, int32 w)
{
	Prog *p1;
	Node nod0, nod1, nod2, nod3, nod4, *l, *r;
	Type *t;
	int32 pc1;
	int i, m, c;

	if(n == Z || n->type == T)
		return;
	if(nn == nodrat)
		if(w > nrathole)
			nrathole = w;
	if(debug['g']) {
		prtree(nn, "sugen lhs");
		prtree(n, "sugen");
	}
	if(typev[n->type->etype]) {
		cgen(n, nn);
		return;
	}
	switch(n->op) {
	case OIND:
		if(nn == Z) {
			nullwarn(n->left, Z);
			break;
		}

	default:
		goto copy;

	case ODOT:
		l = n->left;
		sugen(l, nodrat, l->type->width);
		if(nn != Z) {
			warn(n, "non-interruptable temporary");
			nod1 = *nodrat;
			r = n->right;
			if(!r || r->op != OCONST) {
				diag(n, "DOT and no offset");
				break;
			}
			nod1.xoffset += (int32)r->vconst;
			nod1.type = n->type;
			sugen(&nod1, nn, w);
		}
		break;

	case OSTRUCT:
		if(nn != Z && side(nn)) {
			nod1 = *n;
			nod1.type = typ(TIND, n->type);
			regret(&nod2, &nod1, 0, 0);
			lcgen(nn, &nod2);
			regsalloc(&nod0, &nod1);
			gopcode(OAS, &nod2, Z, &nod0);
			regfree(&nod2);

			nod1 = *n;
			nod1.op = OIND;
			nod1.left = &nod0;
			nod1.right = Z;
			nod1.complex = 1;

			sugen(n, &nod1, w);
			return;
		}

		r = n->left;
		for(t = n->type->link; t != T; t = t->down) {
			l = r;
			if(r->op == OLIST) {
				l = r->left;
				r = r->right;
			}
			if(nn == Z) {
				cgen(l, nn);
				continue;
			}
			nod0 = znode;
			nod0.op = OAS;
			nod0.type = t;
			nod0.left = &nod1;
			nod0.right = l;

			nod1 = znode;
			nod1.op = OIND;
			nod1.type = t;
			nod1.left = &nod2;

			nod2 = znode;
			nod2.op = OADD;
			nod2.type = typ(TIND, t);
			nod2.left = &nod3;
			nod2.right = &nod4;

			nod3 = znode;
			nod3.op = OADDR;
			nod3.type = nod2.type;
			nod3.left = nn;

			nod4 = znode;
			nod4.op = OCONST;
			nod4.type = nod2.type;
			nod4.vconst = t->offset;

			ccom(&nod0);
			acom(&nod0);
			xcom(&nod0);
			nod0.addable = 0;

			cgen(&nod0, Z);
		}
		break;

	case OAS:
		if(nn == Z) {
			if(n->addable < INDEXED)
				sugen(n->right, n->left, w);
			break;
		}
		sugen(n->right, nodrat, w);
		warn(n, "non-interruptable temporary");
		sugen(nodrat, n->left, w);
		sugen(nodrat, nn, w);
		break;

	case OFUNC:
		if(!hasdotdotdot(n->left->type)) {
			cgen(n, Z);
			if(nn != Z) {
				curarg -= n->type->width;
				regret(&nod1, n, n->left->type, 1);
				if(nn->complex >= FNX) {
					regsalloc(&nod2, n);
					cgen(&nod1, &nod2);
					nod1 = nod2;
				}
				cgen(&nod1, nn);
			}
			break;
		}
		if(nn == Z) {
			sugen(n, nodrat, w);
			break;
		}
		if(nn->op != OIND) {
			nn = new1(OADDR, nn, Z);
			nn->type = types[TIND];
			nn->addable = 0;
		} else
			nn = nn->left;
		n = new(OFUNC, n->left, new(OLIST, nn, n->right));
		n->complex = FNX;
		n->type = types[TVOID];
		n->left->type = types[TVOID];
		cgen(n, Z);
		break;

	case OCOND:
		bcgen(n->left, 1);
		p1 = p;
		sugen(n->right->left, nn, w);
		gbranch(OGOTO);
		patch(p1, pc);
		p1 = p;
		sugen(n->right->right, nn, w);
		patch(p1, pc);
		break;

	case OCOMMA:
		cgen(n->left, Z);
		sugen(n->right, nn, w);
		break;
	}
	return;

copy:
	if(nn == Z)
		return;
	if(n->complex >= FNX && nn->complex >= FNX) {
		t = nn->type;
		nn->type = types[TLONG];
		regialloc(&nod1, nn, Z);
		lcgen(nn, &nod1);
		regsalloc(&nod2, nn);
		nn->type = t;

		gopcode(OAS, &nod1, Z, &nod2);
		regfree(&nod1);

		nod2.type = typ(TIND, t);

		nod1 = nod2;
		nod1.op = OIND;
		nod1.left = &nod2;
		nod1.right = Z;
		nod1.complex = 1;
		nod1.type = t;

		sugen(n, &nod1, w);
		return;
	}

	if(n->complex > nn->complex) {
		t = n->type;
		n->type = types[TLONG];
		reglcgen(&nod1, n, Z);
		n->type = t;

		t = nn->type;
		nn->type = types[TLONG];
		reglcgen(&nod2, nn, Z);
		nn->type = t;
	} else {
		t = nn->type;
		nn->type = types[TLONG];
		reglcgen(&nod2, nn, Z);
		nn->type = t;

		t = n->type;
		n->type = types[TLONG];
		reglcgen(&nod1, n, Z);
		n->type = t;
	}

	w /= SZ_LONG;
	if(w <= 5) {
		layout(&nod1, &nod2, w, 0, Z);
		goto out;
	}

	c = 0;
	m = 100;
	i = 3;
	if(w <= 15)
		i = 2;
	for(; i<=5; i++)
		if(i + w%i <= m) {
			c = i;
			m = c + w%c;
		}

	regalloc(&nod3, &regnode, Z);
	layout(&nod1, &nod2, w%c, w/c, &nod3);
	
	pc1 = pc;
	layout(&nod1, &nod2, c, 0, Z);

	gopcode(OSUB, nodconst(1), Z, &nod3);
	nod1.op = OREGISTER;
	gopcode(OADD, nodconst(c*SZ_LONG), Z, &nod1);
	nod2.op = OREGISTER;
	gopcode(OADD, nodconst(c*SZ_LONG), Z, &nod2);
	
	gopcode(OEQ, &nod3, Z, Z);
	p->as = ABGTZ;
	patch(p, pc1);

	regfree(&nod3);
out:
	regfree(&nod1);
	regfree(&nod2);
}

void
layout(Node *f, Node *t, int c, int cv, Node *cn)
{
	Node t1, t2;

	while(c > 3) {
		layout(f, t, 2, 0, Z);
		c -= 2;
	}

	regalloc(&t1, &regnode, Z);
	regalloc(&t2, &regnode, Z);
	t1.type = types[TLONG];
	t2.type = types[TLONG];
	if(c > 0) {
		gopcode(OAS, f, Z, &t1);
		f->xoffset += SZ_LONG;
	}
	if(cn != Z)
		gopcode(OAS, nodconst(cv), Z, cn);
	if(c > 1) {
		gopcode(OAS, f, Z, &t2);
		f->xoffset += SZ_LONG;
	}
	if(c > 0) {
		gopcode(OAS, &t1, Z, t);
		t->xoffset += SZ_LONG;
	}
	if(c > 2) {
		gopcode(OAS, f, Z, &t1);
		f->xoffset += SZ_LONG;
	}
	if(c > 1) {
		gopcode(OAS, &t2, Z, t);
		t->xoffset += SZ_LONG;
	}
	if(c > 2) {
		gopcode(OAS, &t1, Z, t);
		t->xoffset += SZ_LONG;
	}
	regfree(&t1);
	regfree(&t2);
}

static int
isvaddr(Node *n)
{
	switch(n->op) {
	case ONAME:
	case OCONST:
	case OINDREG:
		return 1;
	case OREGISTER:
		diag(n, "botch: vlong in a register?");
		return 1;
	}
	return 0;
}

static int
isvconstable(int o, vlong v)
{
	switch(o) {
	case OADD:
	case OASADD:
	case OSUB:
	case OASSUB:
		return v == 0;
	case OAND:
	case OOR:
	case OXOR:
	case OLSHR:
	case OASHL:
	case OASHR:
	case OASLSHR:
	case OASASHL:
	case OASASHR:
		return 1;
	}
	return 0;
}


static void
vcgen(Node *n, Node *o, int *f)
{
	*f = 0;
	if(!isvaddr(n)) {
		if(n->complex >= FNX) {
			regsalloc(o, n);
			cgen(n, o);
			return;
		}
		*f = 1;
		if(n->addable < INDEXED && n->op != OIND && n->op != OINDEX) {
			regalloc(o, n, Z);
			cgen(n, o);
		} else
			reglcgen(o, n, Z);
	} else
		*o = *n;
}

static int
isuns(int op)
{
	switch(op) {
	case OLO:
	case OLS:
	case OHI:
	case OHS:
		return 1;
	}
	return 0;
}

static void
testv(Node *n, int true)
{
	Node nod;

	nod = znode;
	nod.op = ONE;
	nod.left = n;
	nod.right = new1(0, Z, Z);
	*nod.right = *nodconst(0);
	nod.right->type = n->type;
	nod.type = types[TLONG];
	cmpv(&nod, true);
}

static void
cmpv(Node *n, int true)
{
	Node *l, *r, nl, nr, r1, r2, rx;
	int o, f1, f2;
	Prog *p1, *p2, *p3;
	int32 curs;

	if(debug['g'])
		prtree(n, "cmpv");

	curs = cursafe;
	l = n->left;
	r = n->right;

	if(l->complex >= FNX && r->complex >= FNX) {
		regsalloc(&r1, r);
		cgen(r, &r1);
		rx = *n;
		rx.right = &r1;
		cmpv(&rx, true);
		cursafe = curs;
		return;
	}
	if(l->complex >= r->complex) {
		vcgen(l, &nl, &f1);
		vcgen(r, &nr, &f2);
	} else {
		vcgen(r, &nr, &f2);
		vcgen(l, &nl, &f1);
	}
	nl.type = types[TLONG];
	nr.type = types[TLONG];

	o = n->op;
	if(!true)
		o = comrel[relindex(o)];
	switch(o) {
	case OEQ:
		regalloc(&r1, &regnode, Z);
		regalloc(&r2, &regnode, Z);
		gloadmsb(&nl, &r1);
		gloadmsb(&nr, &r2);
		gopcode(ONE, &r1, &r2, Z);
		p1 = p;
		gloadlsb(&nl, &r1);
		gloadlsb(&nr, &r2);
		gopcode(OEQ, &r1, &r2, Z);
		p2 = p;
		patch(p1, pc);
		gbranch(OGOTO);
		patch(p2, pc);
		regfree(&r1);
		regfree(&r2);
		break;

	case ONE:
		regalloc(&r1, &regnode, Z);
		regalloc(&r2, &regnode, Z);
		gloadmsb(&nl, &r1);
		gloadmsb(&nr, &r2);
		gopcode(ONE, &r1, &r2, Z);
		p1 = p;
		gloadlsb(&nl, &r1);
		gloadlsb(&nr, &r2);
		gopcode(ONE, &r1, &r2, Z);
		p2 = p;
		gbranch(OGOTO);
		patch(p1, pc);
		patch(p2, pc);
		regfree(&r1);
		regfree(&r2);
		break;

	case OGT:
	case OHI:
	case OLT:
	case OLO:
	case OLE:
	case OLS:
	case OGE:
	case OHS:
		l = &nl;
		r = &nr;
		if(o == OLT || o == OLO || o == OLE || o == OLS) {
			l = &nr;
			r = &nl;
		}
		regalloc(&r1, &regnode, Z);
		regalloc(&r2, &regnode, Z);
		regalloc(&rx, &regnode, Z);
		gloadmsb(l, &r1);
		gloadmsb(r, &r2);
		gopcode(OCOMMA, &r1, &r2, &rx);
		if(isuns(o))
			p->as = ASGTU;
		gopcode(ONE, &rx, nodconst(0), Z);
		p1 = p;
		gopcode(ONE, &r1, &r2, Z);
		p2 = p;
		gloadlsb(l, &r1);
		gloadlsb(r, &r2);
		if(o == OGE || o == OHS || o == OLE || o == OLS) {
			gopcode(OCOND, &r2, &r1, &rx);
			gopcode(OEQ, &rx, nodconst(0), Z);
		} else {
			gopcode(OCOND, &r1, &r2, &rx);
			gopcode(ONE, &rx, nodconst(0), Z);
		}
		p3 = p;
		patch(p2, pc);
		gbranch(OGOTO);
		patch(p1, pc);
		patch(p3, pc);
		regfree(&r1);
		regfree(&r2);
		regfree(&rx);
		break;
	default:
		diag(n, "bad cmpv");
		return;
	}
	if(f1)
		regfree(&nl);
	if(f2)
		regfree(&nr);

}

static void
cgen64(Node *n, Node *nn)
{
	Node *l, *r, *d;
	Node nod, nod1;
	int32 curs;
	Type *t;
	int o, m;

	curs = cursafe;
	l = n->left;
	r = n->right;
	o = n->op;
	switch(o) {
	case OCONST:
		if(nn == Z) {
			nullwarn(n->left, Z);
			break;
		}
		if(nn->op != OREGPAIR) {
			t = nn->type;
			nn->type = types[TLONG];
			reglcgen(&nod1, nn, Z);
			nn->type = t;

			if(isbigendian)
				gmove(nod32const(n->vconst>>32), &nod1);
			else
				gmove(nod32const(n->vconst), &nod1);
			nod1.xoffset += SZ_LONG;
			if(isbigendian)
				gmove(nod32const(n->vconst), &nod1);
			else
				gmove(nod32const(n->vconst>>32), &nod1);
			regfree(&nod1);
		} else
			gmove(n, nn);
		break;
	case OCAST:
		if(typev[l->type->etype]) {
			if(!isvaddr(l)) {
				if(l->addable < INDEXED)
				if(l->op != OIND && l->op != OINDEX) {
					regalloc(&nod, l, l);
					cgen(l, &nod);
					regalloc(&nod1, n, nn);
					gmove(nod.right, &nod1);
				} else {
					reglcgen(&nod, l, Z);
					regalloc(&nod1, n, nn);
					gloadlsb(&nod, &nod1);
				}
				regfree(&nod);
			} else {
				regalloc(&nod1, n, nn);
				gloadlsb(l, &nod1);
			}
		} else {
			regalloc(&nod, l, Z);
			cgen(l, &nod);
			regalloc(&nod1, n, nn);
			gmove(&nod, nod1.right);
			if(typeu[l->type->etype])
				gmove(nodconst(0), nod1.left);
			else
				gopcode(OASHR, nodconst(31), nod1.right, nod1.left);
			regfree(&nod);
		}
		gmove(&nod1, nn);
		regfree(&nod1);
		break;

	case OFUNC:
		if(nn == Z) {
			regsalloc(&nod1, n);
			nn = &nod1;
		}
		m = 0;
		if(nn->op != OIND) {
			if(nn->op == OREGPAIR) {
				m = 1;
				regsalloc(&nod1, nn);
				d = &nod1;
			} else
				d = nn;
			d = new1(OADDR, d, Z);
			d->type = types[TIND];
			d->addable = 0;
		} else
			d = nn->left;
		n = new(OFUNC, l, new(OLIST, d, r));
		n->complex = FNX;
		n->type = types[TVOID];
		n->left->type = types[TVOID];
		cgen(n, Z);
		if(m)
			gmove(&nod1, nn);
		break;

	default:
		diag(n, "bad cgen64 %O", o);
		break;
	}
	cursafe = curs;
}
