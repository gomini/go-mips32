#include	"l.h"
#include	"../ld/lib.h"
#include	"../ld/elf.h"
#include	"../ld/dwarf.h"

char	linuxdynld[] = "/lib/ld-linux.so.2";
char	freebsddynld[] = "XXX";
char	openbsddynld[] = "XXX";
char	netbsddynld[] = "XXX";
char	dragonflydynld[] = "XXX";
char	solarisdynld[] = "XXX";

static int
needlib(char *name)
{
	char *p;
	LSym *s;

	if(*name == '\0')
		return 0;
	p = smprint(".dynlib.%s", name);
	s = linklookup(ctxt, p, 0);
	free(p);
	if(s->type == 0) {
		s->type = 100;
		return 1;
	}
	return 0;
}

int	nelfsym = 1;

void
adddynrela(LSym *rel, LSym *s, Reloc *r)
{
	USED(rel); USED(s); USED(r);
}

void
adddynrel(LSym *s, Reloc *r)
{
	LSym *targ;

	targ = r->sym;
	ctxt->cursym = s;
	ctxt->diag("unsupported relocation for dynamic symbol %s (type=%d stype=%d)", targ->name, r->type, targ->type);
}

int
elfreloc1(Reloc *r, vlong sectoff)
{
	USED(r); USED(sectoff);
	return -1;
}

void
elfsetupplt(void)
{
	return;
}

int
machoreloc1(Reloc *r, vlong sectoff)
{
	USED(r);
	USED(sectoff);

	return -1;
}

#define	OP(x,y)		(((x)<<3)|((y)<<0))
#define	SP(x,y)		(((x)<<29)|((y)<<26))

static int
suitdelayslot(uint32 o)
{
	if((o & SP(7,7)) == 0)
		switch(o & OP(7,7)) {
		case 0:case 2:case 3:case 4:case 6:
		case 7:case 10:case 11:case 20:case 22:
		case 23:case 32:case 33:case 34:case 35:
		case 36:case 37:case 38:case 39:case 42:
		case 43:case 44:case 45:case 46:case 47:
		case 56:case 58:case 59:case 60:case 62:
		case 63:
			return 1;
		}
	if((o & SP(7,7)) == 28)
		switch(o & OP(7,7)) {
		case 32:
		case 33:
			return 1;
		}
	return 0;
}

int
archreloc(Reloc *r, LSym *s, vlong *val)
{
	uint32 o1, o2;
	int32 t;
	uchar *px, *q;
	int i;

	if(linkmode == LinkExternal) {
		return -1;
	}
	switch(r->type) {
	case R_CONST:
		*val = r->add;
		return 0;
	case R_GOTOFF:
		*val = symaddr(r->sym) + r->add - symaddr(linklookup(ctxt, ".got", 0));
		return 0;
	case R_ADDRMIPS:
		o1 = r->add >> 32;
		o2 = r->add;
		t = symaddr(r->sym);
		if(t < 0)
			ctxt->diag("relocation for %s is too big (>=2G): %lld", s->name, symaddr(r->sym));
		t += ((o1 & 0xffff) << 16) + ((int32)o2 << 16 >> 16);
		if(t & 0x8000)
			t += 0x10000;
		o1 = (o1 & 0xffff0000) | ((t >> 16) & 0xffff);
		o2 = (o2 & 0xffff0000) | (t & 0xffff);
		if(ctxt->arch->endian == BigEndian)
			*val = ((vlong)o1 << 32) | o2;
		else
			*val = ((vlong)o2 << 32) | o1;
		return 0;
	case R_CALLMIPS:
		o1 = (uint32)r->add & 0x03ffffffful;
		o1 += (symaddr(r->sym) >> 2);
		o1 = ((uint32)r->add & 0xfc000000ul) | (o1 & 0x03fffffful);

		*val = o1;

		px = (uchar*)&o2;
		for(q=s->p+r->off+4, i=0; i < 4; i++)
			px[i] = q[inuxi4[i]];
		if(1 || o2 != 0)
			return 0;

		for(q=r->sym->p, i=0; i < 4; i++)
			px[i] = q[inuxi4[i]];
		if(!suitdelayslot(o2))
			return 0;

		o1 += 1;
		for(q=s->p+r->off+4, i=0; i<4; i++)
			q[i] = px[inuxi4[i]];
		*val = o1;

		return 0;
	}
	return -1;
}

void
adddynsym(Link *ctxt, LSym *s)
{
	USED(ctxt);
	USED(s);
	return;
}

void
adddynlib(char *lib)
{
	LSym *s;

	if(!needlib(lib))
		return;
	if(iself) {
		s = linklookup(ctxt, ".dynstr", 0);
		if(s->size == 0)
			addstring(s, "");
		elfwritedynent(linklookup(ctxt, ".dynamic", 0), DT_NEEDED, addstring(s, lib));
	} else {
		ctxt->diag("adddynlib: unsupported binary format");
	}
}

void
asmb(void)
{
	uint32 symo;
	Section *sect;
	LSym *sym;
	int i;

	if(debug['v'])
		Bprint(&bso, "%5.2f asmb\n", cputime());
	Bflush(&bso);

	if(iself)
		asmbelfsetup();

	sect = segtext.sect;
	cseek(sect->vaddr - segtext.vaddr + segtext.fileoff);
	codeblk(sect->vaddr, sect->len);
	for(sect = sect->next; sect != nil; sect = sect->next) {
		cseek(sect->vaddr - segtext.vaddr + segtext.fileoff);
		datblk(sect->vaddr, sect->len);
	}

	if(segrodata.filelen > 0) {
		if(debug['v'])
			Bprint(&bso, "%5.2f rodatblk\n", cputime());
		Bflush(&bso);

		cseek(segrodata.fileoff);
		datblk(segrodata.vaddr, segrodata.filelen);
	}

	if(debug['v'])
		Bprint(&bso, "%5.2f datblk\n", cputime());
	Bflush(&bso);

	cseek(segdata.fileoff);
	datblk(segdata.vaddr, segdata.filelen);

	symsize = 0;
	lcsize = 0;
	symo = 0;
	if(!debug['s']) {
		if(debug['v'])
			Bprint(&bso, "%5.2f sym\n", cputime());
		Bflush(&bso);
		switch(HEADTYPE) {
		default:
			if(iself)
				goto ElfSym;
		case Hplan9:
			symo = segdata.fileoff+segdata.filelen;
			break;
		ElfSym:
			symo = segdata.fileoff+segdata.filelen;
			symo = rnd(symo, INITRND);
			break;
		}
		cseek(symo);
		switch(HEADTYPE) {
		default:
			if(iself) {
				if(debug['v'])
					Bprint(&bso, "%5.2f elfsym\n", cputime());
				asmelfsym();
				cflush();
				cwrite(elfstrdat, elfstrsize);
	
				if(debug['v'])
					Bprint(&bso, "%5.2f dwarf\n", cputime());
				dwarfemitdebugsections();
				
				if(linkmode == LinkExternal)
					elfemitreloc();
			}
			break;
		case Hplan9:
			asmplan9sym();
			cflush();

			sym = linklookup(ctxt, "pclntab", 0);
			if(sym != nil) {
				lcsize = sym->np;
				for(i=0; i < lcsize; i++)
					cput(sym->p[i]);

				cflush();
			}
			break;
		}
	}

	ctxt->cursym = nil;
	if(debug['v'])
		Bprint(&bso, "%5.2f header\n", cputime());
	Bflush(&bso);
	cseek(0L);
	switch(HEADTYPE) {
	default:
	case Hplan9:
		LPUT(0x647);
		LPUT(segtext.filelen);
		LPUT(segdata.filelen);
		LPUT(segdata.len - segdata.filelen);
		LPUT(symsize);
		LPUT(entryvalue());
		LPUT(0L);
		LPUT(lcsize);
		break;
	case Hlinux:
	case Hfreebsd:
	case Hnetbsd:
	case Hopenbsd:
	case Hnacl:
		asmbelf(symo);
		break;
	}
	cflush();
	if(debug['c']){
		print("textsize=%ulld\n", segtext.filelen);
		print("datsize=%ulld\n", segdata.filelen);
		print("bsssize=%ulld\n", segdata.len - segdata.filelen);
		print("symsize=%d\n", symsize);
		print("lcsize=%d\n", lcsize);
		print("total=%lld\n", segtext.filelen+segdata.len+symsize+lcsize);
	}
}

int32
rnd(int32 v, int32 r)
{
	int32 c;

	if(r <= 0)
		return v;
	v += r - 1;
	c = v % r;
	if(c < 0)
		c += r;
	v -= c;
	return v;
}
