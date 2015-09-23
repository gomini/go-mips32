
#include <u.h>
#include <libc.h>
#include "gg.h"

void
cgen64(Node *n, Node *res)
{
	Node t1, t2, *l, *r;
	Node lo1, lo2, hi1, hi2, z;
	Node al, ah, bl, bh, s, n1, creg;
	Prog *p1, *p2, *p3, *p4, *p5;

	uint64 v;
	int se;

	if(res->op != OINDREG && res->op != ONAME) {
		dump("n", n);
		dump("res", res);
		fatal("cgen64 %O of %O", n->op, res->op);
	}

	l = n->left;
	if(!l->addable) {
		tempname(&t1, l->type);
		cgen(l, &t1);
		l = &t1;
	}

	split64(l, &lo1, &hi1);
	if(n->op == OMINUS) {
		regalloc(&al, types[TUINT32], N);
		regalloc(&ah, hi1.type, N);
		regalloc(&creg, types[TUINT32], N);
		gmove(&lo1, &al);
		gmove(&hi1, &ah);
		nodreg(&z, types[TINT32], 0);
		gins3(ASGTU, &al, &z, &creg);
		gins3(ASUBU, &al, &z, &al);
		gins3(ASUBU, &ah, &z, &ah);
		gins(ASUBU, &creg, &ah);
		splitclean();
		regfree(&creg);
		split64(res, &lo1, &hi1);
		gmove(&al, &lo1);
		gmove(&ah, &hi1);
		regfree(&al);
		regfree(&ah);
		splitclean();
		return;
	}
	switch(n->op) {
	default:
		fatal("cgen64 %O", n->op);

	case OCOM:
		regalloc(&t1, lo1.type, N);
		regalloc(&t2, hi1.type, N);
		nodreg(&z, types[TUINT32], 0);
		
		gmove(&lo1, &t1);
		gmove(&hi1, &t2);
		gins3(ANOR, &t1, &z, &t1);
		gins3(ANOR, &t2, &z, &t2);

		split64(res, &lo2, &hi2);
		gmove(&t1, &lo2);
		gmove(&t2, &hi2);

		regfree(&t1);
		regfree(&t2);
		splitclean();
		splitclean();
		return;

	case OADD:
	case OSUB:
	case OMUL:
	case OLSH:
	case ORSH:
	case OAND:
	case OOR:
	case OXOR:
	case OLROT:
		break;
	}

	r = n->right;
	if(r != N && !r->addable) {
		tempname(&t2, r->type);
		cgen(r, &t2);
		r = &t2;
	}
	if(is64(r->type))
		split64(r, &lo2, &hi2);

	regalloc(&al, lo1.type, N);
	regalloc(&ah, hi1.type, N);

	switch(n->op) {
	default:
		fatal("cgen64: not implemented: %N\n", n);

	case OADD:
		regalloc(&bl, types[TPTR32], N);
		regalloc(&bh, types[TPTR32], N);
		regalloc(&creg, types[TPTR32], N);

		gins(AMOVW, &lo1, &al);
		gins(AMOVW, &lo2, &bl);
		gins(AMOVW, &hi1, &ah);
		gins(AMOVW, &hi2, &bh);

		gins(AADDU, &bl, &al);
		gins(AADDU, &bh, &ah);
		gins3(ASGTU, &bl, &al, &creg);
		gins(AADDU, &creg, &ah);

		regfree(&bl);
		regfree(&bh);
		regfree(&creg);
		break;

	case OSUB:
		regalloc(&bl, types[TPTR32], N);
		regalloc(&bh, types[TPTR32], N);
		regalloc(&creg, types[TPTR32], N);

		gins(AMOVW, &lo1, &al);
		gins(AMOVW, &lo2, &bl);
		gins(AMOVW, &hi1, &ah);
		gins(AMOVW, &hi2, &bh);

		gins3(ASGTU, &bl, &al, &creg);
		gins(ASUBU, &bl, &al);
		gins(ASUBU, &bh, &ah);
		gins(ASUBU, &creg, &ah);

		regfree(&bl);
		regfree(&bh);
		regfree(&creg);
		break;

	case OMUL:

		regalloc(&bl, types[TPTR32], N);
		regalloc(&bh, types[TPTR32], N);
		regalloc(&creg, types[TPTR32], N);
		regalloc(&n1, types[TPTR32], N);

		gins(AMOVW, &lo1, &al);
		gins(AMOVW, &lo2, &bl);
		gins(AMOVW, &hi1, &ah);
		gins(AMOVW, &hi2, &bh);

		gins3(AMULU, &al, &bl, N);
		p1 = gins(AMOVW, N, &n1);
		p2 = gins(AMOVW, N, &creg);
		p1->from.type = D_LO;
		p2->from.type = D_HI;

		gins3(AMUL, &bl, &ah, N);
		p1 = gins(AMOVW, N, &bl);
		p1->from.type = D_LO;

		gins(AADDU, &bl, &creg);

		gins3(AMUL, &al, &bh, N);
		p1 = gins(AMOVW, N, &bl);
		p1->from.type = D_LO;

		gins3(AADDU, &bl, &creg, &ah);
		gins(AMOVW, &n1, &al);

		regfree(&bh);
		regfree(&bl);
		regfree(&creg);
		regfree(&n1);
		break;

	case OXOR:
	case OAND:
	case OOR:
		regalloc(&bl, lo2.type, N);
		regalloc(&bh, hi2.type, N);

		gins(AMOVW, &lo1, &al);
		gins(AMOVW, &lo2, &bl);
		gins(AMOVW, &hi1, &ah);
		gins(AMOVW, &hi2, &bh);

		gins(optoas(n->op, lo1.type), &bl, &al);
		gins(optoas(n->op, hi1.type), &bh, &ah);

		regfree(&bl);
		regfree(&bh);
		break;

	case OLROT:
		v = mpgetfix(r->val.u.xval);
		if(v == 0) {
			gins(AMOVW, &lo1, &al);
			gins(AMOVW, &hi1, &ah);
			break;
		}

		regalloc(&bl, lo1.type, N);
		regalloc(&bh, hi1.type, N);
		if(v >= 32) {
			v -= 32;
			gins(AMOVW, &hi1, &bl);
			gins(AMOVW, &lo1, &bh);
		} else {
			gins(AMOVW, &lo1, &bl);
			gins(AMOVW, &hi1, &bh);
		}

		gins3(ASLL, ncon(v), &bl, &al);
		gins3(ASLL, ncon(v), &bh, &ah);
		gins3(ASRL, ncon(32-v), &bl, &bl);
		gins3(ASRL, ncon(32-v), &bh, &bh);
		gins3(AOR, &bl, &ah, &ah);
		gins3(AOR, &bh, &al, &al);

		regfree(&bl);
		regfree(&bh);
		break;

	case OLSH:
		nodreg(&n1, types[TINT32], 0);

		if(r->op == OLITERAL) {
			v = mpgetfix(r->val.u.xval);
			if(v >= 64) {
				gins(AMOVW, ncon(0), &al);
				gins(AMOVW, ncon(0), &ah);
			} else if(v > 32) {
				regalloc(&bl, lo1.type, N);
				gins(AMOVW, &lo1, &bl);
				gins(AMOVW, ncon(0), &al);
				gins3(ASLL, ncon(v-32), &bl, &ah);
				regfree(&bl);
			} else if(v == 32) {
				gins(AMOVW, &lo1, &ah);
				gins(AMOVW, ncon(0), &al);
			} else if(v > 0) {
				regalloc(&bl, lo1.type, N);
				regalloc(&bh, hi1.type, N);

				gins(AMOVW, &lo1, &bl);
				gins(AMOVW, &hi1, &bh);
				
				gins3(ASLL, ncon(v), &bl, &al);
				gins3(ASLL, ncon(v), &bh, &ah);
				gins3(ASRL, ncon(32-v), &bl, &bl);
				gins3(AOR, &bl, &ah, &ah);

				regfree(&bl);
				regfree(&bh);
			} else {
				gins(AMOVW, &lo1, &al);
				gins(AMOVW, &hi1, &ah);
			}
			break;
		}

		regalloc(&bl, types[TUINT32], N);
		regalloc(&s, types[TUINT32], N);
		regalloc(&creg, types[TUINT32], N);

		p1 = P;
		p2 = P;
		if (is64(r->type)) {
			if(!n->bounded) {
				gins(AMOVW, &hi2, &s);
				p1 = zbranch(ABNE, &s, -1);
			}
			gins(AMOVW, &lo2, &s);
		} else
			gmove(r, &s);

		if(!n->bounded) {
			gins3(ASGTU, ncon(64), &s, &creg);
			p2 = zbranch(ABEQ, &creg, -1);
		}

		gins3(AAND, ncon(32), &s, &creg);
		p3 = zbranch(ABEQ, &creg, 0);

		gins(AMOVW, &lo1, &ah);
		gins(AMOVW, ncon(0), &al);
		gins3(ASLL, &s, &ah, &ah);
		p4 = gjmp(P);

		p5 = P;
		if(!n->bounded) {
			if(p1)
				patch(p1, pc);
			patch(p2, pc);
			gins(AMOVW, ncon(0), &al);
			gins(AMOVW, ncon(0), &ah);
			p5 = gjmp(P);
		}

		patch(p3, pc);
		gins(AMOVW, &lo1, &al);
		gins(AMOVW, &hi1, &ah);
		gins3(ASRL, ncon(1), &al, &creg);
		gins3(ANOR, &s, ncon(0), &bl);
		gins3(ASRL, &bl, &creg, &creg);
		gins3(ASLL, &s, &ah, &ah);
		gins3(ASLL, &s, &al, &al);
		gins3(AOR, &creg, &ah, &ah);

		patch(p4, pc);
		if(p5)
			patch(p5, pc);

		regfree(&bl);
		regfree(&s);
		regfree(&creg);
		break;

	case ORSH:
		nodreg(&n1, types[TINT32], 0);
		se = hi1.type->etype == TINT32;
		
		if(r->op == OLITERAL) {
			v = mpgetfix(r->val.u.xval);

			if(v >= 64) {
				if(se) {
					gins(AMOVW, &hi1, &ah);
					gins3(ASRA, ncon(31), &ah, &al);
					gins3(ASRA, ncon(31), &ah, &ah);
				} else {
					gins(AMOVW, ncon(0), &al);
					gins(AMOVW, ncon(0), &ah);
				}
			} else if(v > 32) {
				gins(AMOVW, &hi1, &al);
				if(se) {
					gins3(ASRA, ncon(31), &al, &ah);
					gins3(ASRA, ncon(v-32), &al, &al);
				} else {
					gins(AMOVW, ncon(0), &ah);
					gins3(ASRL, ncon(v-32), &al, &al);
				}
			} else if(v == 32) {
				gins(AMOVW, &hi1, &al);
				if(se)
					gins3(ASRA, ncon(31), &al, &ah);
				else
					gins(AMOVW, ncon(0), &ah);
			} else if(v > 0) {
				regalloc(&creg, types[TINT32], N);
				gins(AMOVW, &lo1, &al);
				gins(AMOVW, &hi1, &ah);
				gins3(ASRL, ncon(v), &al, &al);
				gins3(ASLL, ncon(32-v), &ah, &creg);
				if(se)
					gins3(ASRA, ncon(v), &ah, &ah);
				else
					gins3(ASRL, ncon(v), &ah, &ah);
				gins3(AOR, &creg, &al, &al);
				regfree(&creg);
			} else {
				gins(AMOVW, &lo1, &al);
				gins(AMOVW, &hi1, &ah);
			}
			break;
		}

		regalloc(&bl, types[TUINT32], N);
		regalloc(&s, types[TUINT32], N);
		regalloc(&creg, types[TUINT32], N);

		p1 = P;
		p2 = P;
		if(is64(r->type)) {
			if(!n->bounded) {
				gins(AMOVW, &hi2, &s);
				p1 = zbranch(ABNE, &s, -1);
			}
			gins(AMOVW, &lo2, &s);
		} else
			gmove(r, &s);

		if(!n->bounded) {
			gins3(ASGTU, ncon(64), &s, &creg);
			p2 = zbranch(ABEQ, &creg, -1);
		}

		gins3(AAND, ncon(32), &s, &creg);
		p3 = zbranch(ABEQ, &creg, 0);

		gins(AMOVW, &hi1, &al);
		if(se) {
			gins3(ASRA, ncon(31), &al, &ah);
			gins3(ASRA, &s, &al, &al);
		} else {
			gins(AMOVW, ncon(0), &ah);
			gins3(ASRL, &s, &al, &al);
		}
		p4 = gjmp(P);

		p5 = P;
		if(!n->bounded) {
			if(p1)
				patch(p1, pc);
			patch(p2, pc);
			if(se) {
				gins(AMOVW, &hi1, &al);
				gins3(ASRA, ncon(31), &al, &ah);
				gins3(ASRA, ncon(31), &al, &al);
			} else {
				gins(AMOVW, ncon(0), &al);
				gins(AMOVW, ncon(0), &ah);
			}
			p5 = gjmp(P);
		}

		patch(p3, pc);
		gins(AMOVW, &lo1, &al);
		gins(AMOVW, &hi1, &ah);
		gins3(ASLL, ncon(1), &ah, &creg);
		gins3(ANOR, &s, ncon(0), &bl);
		gins3(ASLL, &bl, &creg, &creg);
		gins3(ASRL, &s, &al, &al);
		if(se)
			gins3(ASRA, &s, &ah, &ah);
		else
			gins3(ASRL, &s, &ah, &ah);
		gins3(AOR, &creg, &al, &al);

		patch(p4, pc);
		if(p5)
			patch(p5, pc);

		regfree(&bl);
		regfree(&s);
		regfree(&creg);
		break;
	}

	if(is64(r->type))
		splitclean();
	splitclean();

	split64(res, &lo1, &hi1);
	gins(AMOVW, &al, &lo1);
	gins(AMOVW, &ah, &hi1);
	splitclean();

	regfree(&al);
	regfree(&ah);
}

void
cmp64(Node *nl, Node *nr, int op, int likely, Prog *to)
{
	Node lo1, hi1, lo2, hi2, al, ah, bh, tmp;
	Prog *p1;
	int se, a;

	split64(nl, &lo1, &hi1);
	se = hi1.type->etype == TINT32;
	
	regalloc(&al, lo1.type, N);
	regalloc(&ah, hi1.type, N);

	if(nr->op == OLITERAL)
	if(mpgetfix(nr->val.u.xval) == 0) {
		gins(AMOVW, &hi1, &ah);

		switch(op) {
		case OEQ:
		case ONE:
			gins(AMOVW, &lo1, &al);
			gins(AOR, &al, &ah);
			if(op == OEQ)
				a = ABEQ;
			else
				a = ABNE;
			patch(zbranch(a, &ah, likely), to);
			break;

		case OLT:
			if(se)
				patch(zbranch(ABLTZ, &ah, likely), to);
			else {
				;
			}
			break;

		case OGT:
			if(se) {
				p1 = zbranch(ABLTZ, &ah, -likely);
				gins(AMOVW, &lo1, &al);
				gins(AOR, &al, &ah);
				patch(zbranch(ABNE, &ah, likely), to);
				patch(p1, pc);
			} else {
				gins(AMOVW, &lo1, &al);
				gins(AOR, &al, &ah);
				patch(zbranch(ABNE, &ah, likely), to);
			}
			break;

		case OGE:
			if(se) {
				p1 = zbranch(ABLTZ, &ah, -likely);
				gjmp(to);
				patch(p1, pc);
				break;
			} else
				gjmp(to);
			break;

		case OLE:
			if(se)
				patch(zbranch(ABLTZ, &ah, likely), to);
			gins(AMOVW, &lo1, &al);
			gins(AOR, &al, &ah);
			patch(zbranch(ABEQ, &ah, likely), to);
			break;

		default:
			fatal("cmp64: %O, %d, %d", op, op, OLE);
		}

		splitclean();
		regfree(&al);
		regfree(&ah);
		return;
	}

	split64(nr, &lo2, &hi2);
	regalloc(&bh, hi2.type, N);


	gins(AMOVW, &hi1, &ah);
	gins(AMOVW, &hi2, &bh);

	switch(op) {
	case OEQ:
		p1 = tbranch(ABNE, &ah, &bh, -likely);
		gins(AMOVW, &lo1, &ah);
		gins(AMOVW, &lo2, &bh);
		patch(tbranch(ABEQ, &ah, &bh, likely), to);
		patch(p1, pc);
		break;
	case ONE:
		patch(tbranch(ABNE, &ah, &bh, likely), to);
		gins(AMOVW, &lo1, &ah);
		gins(AMOVW, &lo2, &bh);
		patch(tbranch(ABNE, &ah, &bh, likely), to);
		break;

	case OLT:
	case OGE:
	case OGT:
	case OLE:
		if(op == OLT || op == OLE) {
			tmp = ah;
			ah = bh;
			bh = tmp;
		}
		a = ASGTU;
		if(se)
			a = ASGT;
		gins3(a, &ah, &bh, &al);
		patch(zbranch(ABNE, &al, likely), to);
		p1 = tbranch(ABNE, &ah, &bh, -likely);

		if(op == OLT || op == OLE) {
			gins(AMOVW, &lo2, &ah);
			gins(AMOVW, &lo1, &bh);
		} else {
			gins(AMOVW, &lo1, &ah);
			gins(AMOVW, &lo2, &bh);
		}
		if(op == OGT || op == OLT) {
			gins3(ASGTU, &ah, &bh, &al);
			patch(zbranch(ABNE, &al, likely), to);
		} else {
			gins3(ASGTU, &bh, &ah, &al);
			patch(zbranch(ABEQ, &al, likely), to);
		}
		patch(p1, pc);

		break;

	default:
		fatal("cmp64: %O, %d, %d", op, op, OLE);
	}

	regfree(&al);
	regfree(&ah);
	regfree(&bh);

	splitclean();
	splitclean();
}
