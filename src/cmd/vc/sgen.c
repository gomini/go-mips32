#include "gc.h"

Prog*
gtext(Sym *s, int32 stkoff)
{
	int32 a;

	a = argsize(1);
	if((textflag & NOSPLIT) != 0 && stkoff >= 128)
		yyerror("stack frame too large for NOSPLIT function");

	gpseudo(ATEXT, s, nodconst(stkoff));
	p->to.type = D_CONST2;
	p->to.offset2 = a;
	return p;
}

void
noretval(int n)
{

	if(n & 1) {
		gins(ANOP, Z, Z);
		p->to.type = D_REG;
		p->to.reg = REGRET;
	}
	if(n & 2) {
		gins(ANOP, Z, Z);
		p->to.type = D_FREG;
		p->to.reg = FREGRET;
	}
}

void
xcom(Node *n)
{
	Node *l, *r;
	int t;

	if(n == Z)
		return;
	l = n->left;
	r = n->right;
	n->addable = 0;
	n->complex = 0;
	switch(n->op) {
	case OCONST:
		n->addable = 20;
		return;

	case OREGISTER:
		n->addable = 12;
		return;

	case OINDREG:
		n->addable = 15;
		return;

	case ONAME:
		n->addable = 10;
		if(n->class == CPARAM || n->class == CAUTO)
			n->addable = 11;
		return;

	case OADDR:
		xcom(l);
		if(l->addable == 10)
			n->addable = 13;
		if(l->addable == 11)
			n->addable = 2;
		if(l->addable == 15)
			n->addable = 3;
		break;

	case OIND:
		xcom(l);
		if(l->addable == 12)
			n->addable = 15;
		if(l->addable == 13)
			n->addable = 10;
		if(l->addable == 2)
			n->addable = 11;
		if(l->addable == 3)
			n->addable = 15;
		break;

	case OADD:
		xcom(l);
		xcom(r);
		if(r->addable == 20) {
			if((t = l->addable) == 13 || t == 2 || t == 3) {
			comadd:
				l->type = n->type;
				*n = *l;
				l = new(0, Z, Z);
				*l = *(n->left);
				l->xoffset += r->vconst;
				n->left = l;
				r = n->right;
			}
			break;
		}
		if(l->addable == 20) {
			if((t = r->addable) == 13 || t == 2 || t == 3) {
				r = n->left;
				l = n->right;
				n->left = l;
				n->right = r;
				goto comadd;
			}
		}
		break;

	case OASLMUL:
	case OASMUL:
		xcom(l);
		xcom(r);
		t = vlog(r);
		if(t >= 0) {
			n->op = OASASHL;
			r->vconst = t;
			r->type = types[TINT];
		}
		break;

	case OMUL:
	case OLMUL:
		xcom(l);
		xcom(r);
		t = vlog(l);
		if(t >= 0) {
			n->left = r;
			n->right = l;
			l = r;
			r = n->right;
		}
		t = vlog(r);
		if(t >= 0) {
			n->op = OASHL;
			r->vconst = t;
			r->type = types[TINT];
			simplifyshift(n);
		}
		break;

	case OASLDIV:
		xcom(l);
		xcom(r);
		t = vlog(r);
		if(t >= 0) {
			n->op = OASLSHR;
			r->vconst = t;
			r->type = types[TINT];
		}
		break;

	case OLDIV:
		xcom(l);
		xcom(r);
		t = vlog(r);
		if(t >= 0) {
			n->op = OLSHR;
			r->vconst = t;
			r->type = types[TINT];
			simplifyshift(n);
		}
		break;

	case OASLMOD:
		xcom(l);
		xcom(r);
		t = vlog(r);
		if(t >= 0) {
			n->op = OASAND;
			r->vconst--;
		}
		break;

	case OLMOD:
		xcom(l);
		xcom(r);
		t = vlog(r);
		if(t >= 0) {
			n->op = OAND;
			r->vconst--;
		}
		break;

	case OLSHR:
	case OASHL:
	case OASHR:
		xcom(l);
		xcom(r);
		simplifyshift(n);
		break;

	default:
		if(l != Z)
			xcom(l);
		if(r != Z)
			xcom(r);
		break;
	}
	if(n->addable >= 10)
		return;

	if(l != Z)
		n->complex = l->complex;
	if(r != Z) {
		if(r->complex == n->complex)
			n->complex = r->complex+1;
		else
		if(r->complex > n->complex)
			n->complex = r->complex;
	}
	if(n->complex == 0)
		n->complex++;

	if(com64(n))
		return;

	switch(n->op) {
	case OFUNC:
		n->complex = FNX;
		break;

	case OADD:
	case OXOR:
	case OAND:
	case OOR:
	case OEQ:
	case ONE:
		if(l->op == OCONST) {
			n->left = r;
			n->right = l;
		}
		break;
	}
}
