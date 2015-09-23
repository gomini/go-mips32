

#include	"l.h"
#include	"../ld/lib.h"
#include	"../ld/elf.h"
#include	"../ld/dwarf.h"
#include	<ar.h>

char *thestring = "mips32";
LinkArch *thelinkarch;

void
linkarchinit(void)
{
	thestring = getgoarch();
	if(strcmp(thestring, "mips32le") == 0)
		thelinkarch = &linkmips32le;
	else
		thelinkarch = &linkmips32;
}

void
archinit(void)
{
	if(linkmode == LinkAuto && strcmp(getgoextlinkenabled(), "0") == 0)
		linkmode = LinkInternal;

	switch(HEADTYPE) {
	default:
		if(linkmode == LinkAuto)
			linkmode = LinkInternal;
		if(linkmode == LinkExternal && strcmp(getgoextlinkenabled(), "1") != 0)
			sysfatal("cannot use -linkmode=external with -H %s", headstr(HEADTYPE));
		break;
	}

	switch(HEADTYPE) {
	default:
		diag("unknown -H option");
		errorexit();
	case Hplan9:
		HEADR = 32L;
		if(INITTEXT == -1)
			INITTEXT = 4128;
		if(INITDAT == -1)
			INITDAT = 0;
		if(INITRND == -1)
			INITRND = 4096;
		break;
	case Hlinux:
		debug['d'] = 1;
		elfinit();
		HEADR = ELFRESERVE;
		if(INITTEXT == -1)
			INITTEXT = 0x10000 + HEADR;
		if(INITDAT == -1)
			INITDAT = 0;
		if(INITRND == -1)
			INITRND = 0x10000;
		break;
	case Hnacl:
		elfinit();
		HEADR = 0x10000;
		funcalign = 16;
		if(INITTEXT == -1)
			INITTEXT = 0x20000;
		if(INITDAT == -1)
			INITDAT = 0;
		if(INITRND == -1)
			INITRND = 0x10000;
		break;
	}
	if(INITDAT != 0 && INITRND != 0)
		print("warning: -D0x%ux is ignored because of -R0x%ux\n",
			INITDAT, INITRND);

}
