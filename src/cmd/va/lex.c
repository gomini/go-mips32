#define	EXTERN
#include <u.h>
#include <libc.h>
#include "a.h"
#include "y.tab.h"

enum
{
	Plan9	= 1<<0,
	Unix	= 1<<1,
	Windows	= 1<<2,
};

int
systemtype(int sys)
{
#ifdef _WIN32
	return sys&Windows;
#else
	return sys&Plan9;
#endif
}

int
Lconv(Fmt *fp)
{
	return linklinefmt(ctxt, fp);
}

void
dodef(char *p)
{
	if(nDlist%8 == 0)
		Dlist = allocn(Dlist, nDlist*sizeof(char*), 8*sizeof(char*));
	Dlist[nDlist++] = p;
}

LinkArch*	thelinkarch = &linkmips32;

void
usage(void)
{
	print("usage: %ca [options] file.c...\n", thechar);
	flagprint(1);
	errorexit();
}

void
main(int argc, char *argv[])
{
	char *p;

	thechar = 'v';
	thestring = "mips32";

	p = getgoarch();
	if(strncmp(p, thestring, strlen(thestring)) != 0)
		sysfatal("cannot use %cc with GOARCH=%s", thechar, p);
	if(strcmp(p, "mips32le") == 0)
		thelinkarch = &linkmips32le;

	ctxt = linknew(thelinkarch);
	ctxt->diag = yyerror;
	ctxt->bso = &bstdout;
	ctxt->enforce_data_order = 1;
	Binit(&bstdout, 1, OWRITE);
	listinitv();
	fmtinstall('L', Lconv);

	ensuresymb(NSYMB);
	memset(debug, 0, sizeof(debug));
	cinit();
	outfile = 0;
	setinclude(".");

	flagfn1("D", "name[=value]: add #define", dodef);
	flagfn1("I", "dir: add dir to include path", setinclude);
	flagcount("S", "print assembly and machine code", &debug['S']);
	flagcount("m", "debug preprocessor macros", &debug['m']);
	flagstr("o", "file: set output file", &outfile);
	flagstr("trimpath", "prefix: remove prefix from recorded source file paths", &ctxt->trimpath);

	flagparse(&argc, &argv, usage);
	ctxt->debugasm = debug['S'];

	if(argc < 1)
		usage();
	if(argc > 1){
		print("can't assemble multiple files\n");
		errorexit();
	}

	if(assemble(argv[0]))
		errorexit();
	Bflush(&bstdout);
	exits(0);
}

int
assemble(char *file)
{
	char *ofile, *p;
	int i, of;

	ofile = alloc(strlen(file) + 3);
	strcpy(ofile, file);
	p = utfrrune(ofile, '/');
	if(p) {
		include[0] = ofile;
		*p++ = 0;
	} else
		p = ofile;
	if(outfile == 0) {
		outfile = p;
		if(outfile){
			p = utfrrune(outfile, '.');
			if(p)
				if(p[1] == 's' && p[2] == 0)
					p[0] = 0;
			p = utfrune(outfile, 0);
			p[0] = '.';
			p[1] = thechar;
			p[2] = 0;
		} else
			outfile = "/dev/null";
	}

	of = create(outfile, OWRITE, 0644);
	if(of < 0){
		yyerror("%ca: cannot create %s", thechar, outfile);
		errorexit();
	}
	Binit(&obuf, of, OWRITE);
	Bprint(&obuf, "go object %s %s %s\n", getgoos(), getgoarch(), getgoversion());
	Bprint(&obuf, "!\n");

	for(pass = 1; pass <= 2; pass++) {
		nosched = 0;
		pinit(file);
		for(i=0; i<nDlist; i++)
			dodefine(Dlist[i]);
		yyparse();
		cclean();
		if(nerrors)
			return nerrors;
	}

	writeobj(ctxt, &obuf);
	Bflush(&obuf);
	return 0;
}

struct
{
	char	*name;
	ushort	type;
	ushort	value;
} itab[] =
{
	"SP",		LSP,	D_AUTO,
	"SB",		LSB,	D_EXTERN,
	"FP",		LFP,	D_PARAM,
	"PC",		LPC,	D_BRANCH,
	"HI",		LHI,	D_HI,
	"LO",		LLO,	D_LO,

	"R",		LR,	0,
	"R0",		LREG,	0,
	"R1",		LREG,	1,
	"R2",		LREG,	2,
	"R3",		LREG,	3,
	"R4",		LREG,	4,
	"R5",		LREG,	5,
	"R6",		LREG,	6,
	"R7",		LREG,	7,
	"R8",		LREG,	8,
	"R9",		LREG,	9,
	"R10",		LREG,	10,
	"R11",		LREG,	11,
	"R12",		LREG,	12,
	"R13",		LREG,	13,
	"R14",		LREG,	14,
	"R15",		LREG,	15,
	"R16",		LREG,	16,
	"R17",		LREG,	17,
	"R18",		LREG,	18,
	"R19",		LREG,	19,
	"R20",		LREG,	20,
	"R21",		LREG,	21,
	"R22",		LREG,	22,
	"R23",		LREG,	23,
	"R24",		LREG,	24,
	"R25",		LREG,	25,
	"R26",		LREG,	26,
	"R27",		LREG,	27,
	"R28",		LREG,	28,
	"R29",		LREG,	29,
	"R30",		LREG,	30,
	"R31",		LREG,	31,

	"M",		LM,	0,
	"M0",		LMREG,	0,
	"M1",		LMREG,	1,
	"M2",		LMREG,	2,
	"M3",		LMREG,	3,
	"M4",		LMREG,	4,
	"M5",		LMREG,	5,
	"M6",		LMREG,	6,
	"M7",		LMREG,	7,
	"M8",		LMREG,	8,
	"M9",		LMREG,	9,
	"M10",		LMREG,	10,
	"M11",		LMREG,	11,
	"M12",		LMREG,	12,
	"M13",		LMREG,	13,
	"M14",		LMREG,	14,
	"M15",		LMREG,	15,
	"M16",		LMREG,	16,
	"M17",		LMREG,	17,
	"M18",		LMREG,	18,
	"M19",		LMREG,	19,
	"M20",		LMREG,	20,
	"M21",		LMREG,	21,
	"M22",		LMREG,	22,
	"M23",		LMREG,	23,
	"M24",		LMREG,	24,
	"M25",		LMREG,	25,
	"M26",		LMREG,	26,
	"M27",		LMREG,	27,
	"M28",		LMREG,	28,
	"M29",		LMREG,	29,
	"M30",		LMREG,	30,
	"M31",		LMREG,	31,

	"F",		LF,	0,
	"F0",		LFREG,	0,
	"F1",		LFREG,	1,
	"F2",		LFREG,	2,
	"F3",		LFREG,	3,
	"F4",		LFREG,	4,
	"F5",		LFREG,	5,
	"F6",		LFREG,	6,
	"F7",		LFREG,	7,
	"F8",		LFREG,	8,
	"F9",		LFREG,	9,
	"F10",		LFREG,	10,
	"F11",		LFREG,	11,
	"F12",		LFREG,	12,
	"F13",		LFREG,	13,
	"F14",		LFREG,	14,
	"F15",		LFREG,	15,
	"F16",		LFREG,	16,
	"F17",		LFREG,	17,
	"F18",		LFREG,	18,
	"F19",		LFREG,	19,
	"F20",		LFREG,	20,
	"F21",		LFREG,	21,
	"F22",		LFREG,	22,
	"F23",		LFREG,	23,
	"F24",		LFREG,	24,
	"F25",		LFREG,	25,
	"F26",		LFREG,	26,
	"F27",		LFREG,	27,
	"F28",		LFREG,	28,
	"F29",		LFREG,	29,
	"F30",		LFREG,	30,
	"F31",		LFREG,	31,

	"FCR",		LFCR,	0,
	"FCR0",		LFCREG,	0,
	"FCR1",		LFCREG,	1,
	"FCR2",		LFCREG,	2,
	"FCR3",		LFCREG,	3,
	"FCR4",		LFCREG,	4,
	"FCR5",		LFCREG,	5,
	"FCR6",		LFCREG,	6,
	"FCR7",		LFCREG,	7,
	"FCR8",		LFCREG,	8,
	"FCR9",		LFCREG,	9,
	"FCR10",	LFCREG,	10,
	"FCR11",	LFCREG,	11,
	"FCR12",	LFCREG,	12,
	"FCR13",	LFCREG,	13,
	"FCR14",	LFCREG,	14,
	"FCR15",	LFCREG,	15,
	"FCR16",	LFCREG,	16,
	"FCR17",	LFCREG,	17,
	"FCR18",	LFCREG,	18,
	"FCR19",	LFCREG,	19,
	"FCR20",	LFCREG,	20,
	"FCR21",	LFCREG,	21,
	"FCR22",	LFCREG,	22,
	"FCR23",	LFCREG,	23,
	"FCR24",	LFCREG,	24,
	"FCR25",	LFCREG,	25,
	"FCR26",	LFCREG,	26,
	"FCR27",	LFCREG,	27,
	"FCR28",	LFCREG,	28,
	"FCR29",	LFCREG,	29,
	"FCR30",	LFCREG,	30,
	"FCR31",	LFCREG,	31,

	"ADD",		LTYPE1, AADD,
	"ADDU",		LTYPE1, AADDU,
	"SUB",		LTYPE1, ASUB,
	"SUBU",		LTYPE1, ASUBU,
	"SGT",		LTYPE1, ASGT,
	"SGTU",		LTYPE1, ASGTU,
	"AND",		LTYPE1, AAND,
	"OR",		LTYPE1, AOR,
	"XOR",		LTYPE1, AXOR,
	"SLL",		LTYPE1, ASLL,
	"SRL",		LTYPE1, ASRL,
	"SRA",		LTYPE1, ASRA,

	"NOR",		LTYPE2, ANOR,

	"MOVB",		LTYPE3, AMOVB,
	"MOVBU",	LTYPE3, AMOVBU,
	"MOVH",		LTYPE3, AMOVH,
	"MOVHU",	LTYPE3, AMOVHU,
	"MOVWL",	LTYPE3, AMOVWL,
	"MOVWR",	LTYPE3, AMOVWR,

	"BREAK",	LTYPEJ, ABREAK,
	"END",		LTYPE4, AEND,
	"REM",		LTYPE6, AREM,
	"REMU",		LTYPE6, AREMU,
	"RET",		LTYPE4, ARET,
	"SYSCALL",	LTYPE4, ASYSCALL,
	"TLBP",		LTYPE4, ATLBP,
	"TLBR",		LTYPE4, ATLBR,
	"TLBWI",	LTYPE4, ATLBWI,
	"TLBWR",	LTYPE4, ATLBWR,

	"MOVW",		LTYPE5, AMOVW,
	"MOVD",		LTYPE5, AMOVD,
	"MOVF",		LTYPE5, AMOVF,

	"DIV",		LTYPE6, ADIV,
	"DIVU",		LTYPE6, ADIVU,
	"MUL",		LTYPE6, AMUL,
	"MULU",		LTYPE6, AMULU,

	"JMP",		LTYPE7, AJMP,

	"JAL",		LTYPE8, AJAL,

	"BEQ",		LTYPE9, ABEQ,
	"BNE",		LTYPE9, ABNE,

	"BGEZ",		LTYPEA, ABGEZ,
	"BGEZAL",	LTYPEA, ABGEZAL,
	"BGTZ",		LTYPEA, ABGTZ,
	"BLEZ",		LTYPEA, ABLEZ,
	"BLTZ",		LTYPEA, ABLTZ,
	"BLTZAL",	LTYPEA, ABLTZAL,

	"TEXT",		LTYPEB, ATEXT,
	"GLOBL",	LTYPEB, AGLOBL,

	"DATA",		LTYPEC, ADATA,

	"MOVDF",	LTYPE5, AMOVDF,
	"MOVDW",	LTYPE5, AMOVDW,
	"MOVFD",	LTYPE5, AMOVFD,
	"MOVFW",	LTYPE5, AMOVFW,
	"MOVWD",	LTYPE5, AMOVWD,
	"MOVWF",	LTYPE5, AMOVWF,

	"ABSD",		LTYPED, AABSD,
	"ABSF",		LTYPED, AABSF,
	"NEGD",		LTYPED, ANEGD,
	"NEGF",		LTYPED, ANEGF,

	"CMPEQD",	LTYPEF, ACMPEQD,
	"CMPEQF",	LTYPEF, ACMPEQF,
	"CMPGED",	LTYPEF, ACMPGED,
	"CMPGEF",	LTYPEF, ACMPGEF,
	"CMPGTD",	LTYPEF, ACMPGTD,
	"CMPGTF",	LTYPEF, ACMPGTF,

	"ADDD",		LTYPEE, AADDD,
	"ADDF",		LTYPEE, AADDF,
	"DIVD",		LTYPEE, ADIVD,
	"DIVF",		LTYPEE, ADIVF,
	"MULD",		LTYPEE, AMULD,
	"MULF",		LTYPEE, AMULF,
	"SUBD",		LTYPEE, ASUBD,
	"SUBF",		LTYPEE, ASUBF,

	"BFPT",		LTYPEG, ABFPT,
	"BFPF",		LTYPEG, ABFPF,


	"ROTR",		LTYPE1, AROTR,
	"MAD",		LTYPE6, AMAD,
	"MADU",		LTYPE6, AMADU,
	"MSUB",		LTYPE6, AMSUB,
	"MSUBU",	LTYPE6, AMSUBU,

	"MUL32",	LMUL32, AMUL32,
	"MOVZ",		LMOVN, AMOVZ,
	"MOVN",		LMOVN, AMOVN,

	"CLZ",		LBITOP,	ACLZ,
	"CLO",		LBITOP, ACLO,
	"SEB",		LBITOP, ASEB,
	"SEH",		LBITOP,	ASEH,
	"WSBH",		LBITOP,	AWSBH,

	"PREF", 	LPREF, APREF,
	"LL",		LLL, ALL,
	"SC",		LSC, ASC,

	"WORD",		LTYPEH, AWORD,
	"NOP",		LTYPEI, ANOP,
	"SCHED",	LSCHED, 0,
	"NOSCHED",	LSCHED, 0x80,

	"UNDEF",	LTYPE4,	AUNDEF,
	"PCDATA",	LPCDAT,	APCDATA,
	"FUNCDATA",	LFUNCDAT, AFUNCDATA,

	0
};

void
cinit(void)
{
	Sym *s;
	int i;

	nullgen.type = D_NONE;
	nullgen.name = D_NONE;
	nullgen.reg = NREG;
	nullgen.scale = NREG;

	nerrors = 0;
	iostack = I;
	iofree = I;
	peekc = IGN;
	nhunk = 0;
	for(i=0; i<NHASH; i++)
		hash[i] = S;
	for(i=0; itab[i].name; i++) {
		s = slookup(itab[i].name);
		s->type = itab[i].type;
		s->value = itab[i].value;
	}
}

void
syminit(Sym *s)
{

	s->type = LNAME;
	s->value = 0;
}

void
cclean(void)
{

	outcode(AEND, &nullgen, NREG, &nullgen);
	Bflush(&obuf);
}

static Prog *lastpc;

void
outcode(int a, Addr *g1, int reg, Addr *g2)
{
	Prog *p;
	Plist *pl;

	if(pass == 1)
		goto out;

	p = ctxt->arch->prg();
	memset(p, 0, sizeof *p);
	p->as = a;
	p->lineno = lineno;
	if(nosched)
		p->mark |= NOSCHED;
	p->from = *g1;
	p->reg = reg;
	p->to = *g2;
	p->pc = pc;

	if(lastpc == nil) {
		pl = linknewplist(ctxt);
		pl->firstpc = p;
	} else
		lastpc->link = p;
	lastpc = p;

out:
	if(a != AGLOBL && a != ADATA)
		pc++;
}

LSym *thetext;

void
settext(LSym *s)
{
	thetext = s;
}

Sym*
labellookup(Sym *s)
{
	char *p;
	Sym *lab;

	if(thetext == nil) {
		s->labelname = s->name;
		return s;
	}
	p = smprint("%s.%s", thetext->name, s->name);
	lab = slookup(p);
	free(p);
	lab->labelname = s->name;
	return lab;
}

#include "../cc/lexbody"
#include "../cc/macbody"
