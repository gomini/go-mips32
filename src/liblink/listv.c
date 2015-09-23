#include <u.h>
#include <libc.h>
#include <bio.h>
#include <link.h>
#include "../cmd/vl/v.out.h"
#include "../runtime/funcdata.h"

enum {
	STRINGSZ = 1000,
};

static int	Aconv(Fmt*);
static int	Dconv(Fmt*);
static int	Pconv(Fmt*);
static int	Rconv(Fmt*);
static int	DSconv(Fmt*);
static int	Mconv(Fmt*);
static int	DRconv(Fmt*);

static Prog*	bigP;


#pragma	varargck	type	"$"	char*
#pragma	varargck	type	"M"	Addr*

void
listinitv(void)
{
	fmtinstall('A', Aconv);
	fmtinstall('D', Dconv);
	fmtinstall('P', Pconv);
	fmtinstall('R', Rconv);

	fmtinstall('^', DRconv);

	fmtinstall('$', DSconv);
	fmtinstall('M', Mconv);
}

static int
Aconv(Fmt *fp)
{
	char *s;
	int a;

	a = va_arg(fp->args, int);
	s = "???";
	if(a >= AXXX && a < ALAST)
		s = anamesv[a];
	return fmtstrcpy(fp, s);
}

static int
Dconv(Fmt *fp)
{
	char str[STRINGSZ];
	Addr *a;
	int32 v;

	a = va_arg(fp->args, Addr*);

	if(fp->flags & FmtLong) {
		if(a->type==D_CONST2) {
			if(a->offset2==ArgsSizeUnknown)
				sprint(str, "$%d-unknown", (int32)a->offset);
			else
				sprint(str, "$%d-%d", (int32)a->offset, a->offset2);
		} else
			sprint(str, "!!%D", a);
		return fmtstrcpy(fp, str);
	}

	switch(a->type) {
	default:
		sprint(str, "GOK-type(%d)", a->type);
		break;

	case D_NONE:
		str[0] = 0;
		if(a->name != D_NONE || a->reg != NREG || a->sym != nil)
			sprint(str, "%M(R%d)(NONE)", a, a->reg);
		break;

	case D_CONST:
		if(a->reg != NREG)
			sprint(str, "$%M(R%d)", a, a->reg);
		else
			sprint(str, "$%M", a);
		break;

	case D_CONST2:
		if(a->offset2==ArgsSizeUnknown)
			sprint(str, "$%lld-unknown", a->offset);
		else
			sprint(str, "$%lld-%d", a->offset, a->offset2);
		break;

	case D_OCONST:
		if(a->reg != NREG)
			sprint(str, "%M(R%d)", a, a->reg);
		else
			sprint(str, "$*$%M", a);
		break;

	case D_OREG:
		if(a->reg != NREG)
			sprint(str, "%M(R%d)", a, a->reg);
		else
			sprint(str, "%M", a);
		break;

	case D_REG:
		sprint(str, "R%d", a->reg);
		if(a->name != D_NONE || a->sym != nil)
			sprint(str, "%M(R%d)(REG)", a, a->reg);
		break;

	case D_MREG:
		sprint(str, "M%d", a->reg);
		if(a->name != D_NONE || a->sym != nil)
			sprint(str, "%M(R%d)(REG)", a, a->reg);
		break;

	case D_FREG:
		sprint(str, "F%d", a->reg);
		if(a->name != D_NONE || a->sym != nil)
			sprint(str, "%M(R%d)(REG)", a, a->reg);
		break;

	case D_FCREG:
		sprint(str, "FC%d", a->reg);
		if(a->name != D_NONE || a->sym != nil)
			sprint(str, "%M(R%d)(REG)", a, a->reg);
		break;

	case D_LO:
		sprint(str, "LO");
		if(a->name != D_NONE || a->sym != nil)
			sprint(str, "%M(LO)(REG)", a);
		break;

	case D_HI:
		sprint(str, "HI");
		if(a->name != D_NONE || a->sym != nil)
			sprint(str, "%M(HI)(REG)", a);
		break;

	case D_BRANCH:
		if(bigP->pcond != nil) {
			v = bigP->pcond->pc;
			if(a->sym != nil)
				sprint(str, "%s+%.5lux(BRANCH)", a->sym->name, v);
			else
				sprint(str, "%.5lux(BRANCH)", v);
		} else if(a->u.branch != nil)
			sprint(str, "%lld", a->u.branch->pc);
		else if(a->sym != nil)
			sprint(str, "%s+%ld(APC)", a->sym->name, a->offset);
		else
			sprint(str, "%ld(APC)", a->offset);
		break;

	case D_FCONST:
		sprint(str, "$%.17g", a->u.dval);
		break;

	case D_SCONST:
		sprint(str, "$\"%$\"", a->u.sval);
		break;
	}
	return fmtstrcpy(fp, str);
}

static int
Pconv(Fmt *fp)
{
	char str[STRINGSZ], *s;
	Prog *p;
	int a;

	p = va_arg(fp->args, Prog*);
	bigP = p;
	a = p->as;

	if(a == ADATA || a == ADYNT_ || a == AINIT_)
		sprint(str, "%.5lld (%L)	%A	%D/%d/%D", p->pc, p->lineno, a, &p->from, p->reg, &p->to);
	else if(a == ATEXT) {
		if(p->reg != 0)
			sprint(str, "%.5lld (%L)	%A	%D,%d,%lD", p->pc, p->lineno, a, &p->from, p->reg, &p->to);
		else
			sprint(str, "%.5lld (%L)	%A	%D,%lD", p->pc, p->lineno, a, &p->from, &p->to);
	} else if(a == AGLOBL) {
		if(p->reg != 0)
			sprint(str, "%.5lld (%L)	%A	%D,%d,%D", p->pc, p->lineno, a, &p->from, p->reg, &p->to);
		else
			sprint(str, "%.5lld (%L)	%A	%D,%D", p->pc, p->lineno, a, &p->from, &p->to);
	} else {
		s = str;
		if(p->mark & NOSCHED)
			s += sprint(s, "*");
		if(isnopv(p))
			sprint(s, "%.5lld (%L)	nop", p->pc, p->lineno);
		else if(p->reg == NREG)
			sprint(s, "%.5lld (%L)	%A	%D,%D", p->pc, p->lineno, a, &p->from, &p->to);
		else {
			s += sprint(s, "%.5lld (%L)	%A	%D", p->pc, p->lineno, a, &p->from);
			s += sprint(s, ",%c%d", p->from.type==D_FREG?'F':'R', p->reg);
			sprint(s, ",%D", &p->to);
		}
		if(p->spadj != 0)
			return fmtprint(fp, "%s # spadj=%d", str, p->spadj);
	}
	return fmtstrcpy(fp, str);
}

static int
Rconv(Fmt *fp)
{
	char str[STRINGSZ];
	int r;

	r = va_arg(fp->args, int);
	if(r < NREG)
		sprint(str, "r%d", r);
	else
		sprint(str, "f%d", r-NREG);
	return fmtstrcpy(fp, str);
}

static int
DSconv(Fmt *fp)
{
	int i, c;
	char str[STRINGSZ], *p, *a;

	a = va_arg(fp->args, char*);
	p = str;
	for(i=0; i<sizeof(int32); i++) {
		c = a[i] & 0xff;
		if(c >= 'a' && c <= 'z' ||
		   c >= 'A' && c <= 'Z' ||
		   c >= '0' && c <= '9' ||
		   c == ' ' || c == '%') {
			*p++ = c;
			continue;
		}
		*p++ = '\\';
		switch(c) {
		case 0:
			*p++ = 'z';
			continue;
		case '\\':
		case '"':
			*p++ = c;
			continue;
		case '\n':
			*p++ = 'n';
			continue;
		case '\t':
			*p++ = 't';
			continue;
		}
		*p++ = (c>>6) + '0';
		*p++ = ((c>>3) & 7) + '0';
		*p++ = (c & 7) + '0';
	}
	*p = 0;
	return fmtstrcpy(fp, str);
}

static int
DRconv(Fmt *fp)
{
	char *s;
	int a;

	a = va_arg(fp->args, int);
	s = "C_??";
	if(a >= C_NONE && a <= C_NCLASS)
		s = cnamesv[a];
	return fmtstrcpy(fp, s);
}

static int
Mconv(Fmt *fp)
{
	char str[STRINGSZ];
	Addr *a;
	LSym *s;

	a = va_arg(fp->args, Addr*);
	s = a->sym;
	switch(a->name) {
	default:
		sprint(str, "GOK-name(%d)", a->name);
		break;

	case D_NONE:
		sprint(str, "%lld", a->offset);
		break;

	case D_EXTERN:
		if(s == nil)
			sprint(str, "%lld(SB)", a->offset);
		else {
			if(a->offset != 0)
				sprint(str, "%s+%lld(SB)", s->name, a->offset);
			else
				sprint(str, "%s(SB)", s->name);
		}
		break;

	case D_STATIC:
		if(s == nil)
			sprint(str, "<>+%lld(SB)", a->offset);
		else
			sprint(str, "%s<>+%lld(SB)", s->name, a->offset);
		break;

	case D_AUTO:
		if(s == nil)
			sprint(str, "%lld(SP)", -a->offset);
		else
			sprint(str, "%s-%lld(SP)", s->name, -a->offset);
		break;

	case D_PARAM:
		if(s == nil)
			sprint(str, "%lld(FP)", a->offset);
		else
			sprint(str, "%s+%lld(FP)", s->name, a->offset);
		break;
	}

	return fmtstrcpy(fp, str);
}
