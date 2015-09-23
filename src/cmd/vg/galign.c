
#include <u.h>
#include <libc.h>
#include "gg.h"

int	thechar	= 'v';
char*	thestring	= "mips32";
LinkArch*	thelinkarch = &linkmips32;

void
linkarchinit(void)
{
	isbigendian = 1;
	if(strcmp(getgoarch(), "mips32le") == 0) {
		thelinkarch = &linkmips32le;
		isbigendian = 0;
	}
}

vlong MAXWIDTH = (1LL<<32) - 1;

Typedef	typedefs[] =
{
	{"int",		TINT,		TINT32},
	{"uint",	TUINT,		TUINT32},
	{"uintptr",	TUINTPTR,	TUINT32},
	{0}
};

void
betypeinit(void)
{
	widthptr = 4;
	widthint = 4;
	widthreg = 4;

	zprog.link = P;
	zprog.as = AGOK;
	zprog.reg = NREG;
	zprog.from.type = D_NONE;
	zprog.from.name = D_NONE;
	zprog.from.reg = NREG;
	zprog.to = zprog.from;

	listinitv();
}
