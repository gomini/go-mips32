
#include <u.h>
#include <libc.h>
#include "gg.h"
#include "opt.h"

enum
{
	RightRdwr = RightRead | RightWrite,
};

static ProgInfo progtable[ALAST] = {
	[ATYPE]=	{Pseudo | Skip},
	[ATEXT]=	{Pseudo},
	[AFUNCDATA]=	{Pseudo},
	[APCDATA]=	{Pseudo},
	[AUNDEF]=	{Break},
	[AUSEFIELD]=	{OK},
	[ACHECKNIL]=	{LeftRead},
	[AVARDEF]=	{Pseudo | RightWrite},
	[AVARKILL]=	{Pseudo | RightWrite},

	[ANOP]=		{LeftRead | RightWrite},
	
	[AADDU]=	{SizeL | LeftRead | RegRead | RightWrite},
	[ASUBU]=	{SizeL | LeftRead | RegRead | RightWrite},
	[AMUL]=		{SizeL | LeftRead | RegRead},
	[AMULU]=	{SizeL | LeftRead | RegRead},
	[ADIV]=		{SizeL | LeftRead | RegRead},
	[ADIVU]=	{SizeL | LeftRead | RegRead},
	[AREMU]=	{SizeL | LeftRead | RegRead},
	[AREM]=		{SizeL | LeftRead | RegRead},
	[AAND]=		{SizeL | LeftRead | RegRead | RightWrite},
	[AOR]=		{SizeL | LeftRead | RegRead | RightWrite},
	[ANOR]=		{SizeL | LeftRead | RegRead | RightWrite},
	[AXOR]=		{SizeL | LeftRead | RegRead | RightWrite},
	[ASLL]=		{SizeL | LeftRead | RegRead | RightWrite},
	[ASRA]=		{SizeL | LeftRead | RegRead | RightWrite},
	[ASRL]=		{SizeL | LeftRead | RegRead | RightWrite},
	[ASGT]=		{SizeL | LeftRead | RegRead | RightWrite},
	[ASGTU]=	{SizeL | LeftRead | RegRead | RightWrite},

	[AADDD]=	{SizeD | LeftRead | RegRead | RightWrite},
	[AADDF]=	{SizeF | LeftRead | RegRead | RightWrite},
	[ASUBD]=	{SizeD | LeftRead | RegRead | RightWrite},
	[ASUBF]=	{SizeF | LeftRead | RegRead | RightWrite},
	[AMULD]=	{SizeD | LeftRead | RegRead | RightWrite},
	[AMULF]=	{SizeF | LeftRead | RegRead | RightWrite},
	[ADIVD]=	{SizeD | LeftRead | RegRead | RightWrite},
	[ADIVF]=	{SizeF | LeftRead | RegRead | RightWrite},
	[ANEGF]=	{SizeF | LeftRead | RightWrite},
	[ANEGD]=	{SizeF | LeftRead | RightWrite},
	[ACMPEQD]=	{SizeD | LeftRead | RegRead},
	[ACMPEQF]=	{SizeF | LeftRead | RegRead},
	[ACMPGED]=	{SizeD | LeftRead | RegRead},
	[ACMPGEF]=	{SizeF | LeftRead | RegRead},
	[ACMPGTD]=	{SizeD | LeftRead | RegRead},
	[ACMPGTF]=	{SizeF | LeftRead | RegRead},

	[AMOVWD]=		{SizeD | LeftRead | RightWrite | Conv},
	[AMOVWF]=		{SizeF | LeftRead | RightWrite | Conv},
	[AMOVDF]=		{SizeF | LeftRead | RightWrite | Conv},
	[AMOVDW]=		{SizeL | LeftRead | RightWrite | Conv},
	[AMOVFD]=		{SizeD | LeftRead | RightWrite | Conv},
	[AMOVFW]=		{SizeL | LeftRead | RightWrite | Conv},

	[AMOVW]=		{SizeL | LeftRead | RightWrite | Move},
	[AMOVD]=		{SizeD | LeftRead | RightWrite | Move},
	[AMOVF]=		{SizeF | LeftRead | RightWrite | Move},

	[AMOVB]=		{SizeB | LeftRead | RightWrite | Move | Conv},
	[AMOVBU]=		{SizeB | LeftRead | RightWrite | Conv | Move},
	[AMOVH]=		{SizeW | LeftRead | RightWrite | Move | Conv},
	[AMOVHU]=		{SizeW | LeftRead | RightWrite | Conv | Move},
	
	[ADUFFZERO]=		{Call},
	[ADUFFCOPY]=		{Call},

	[AJMP]=		{Jump | Break},
	[AJAL]=		{Call},
	[ABEQ]=		{Cjmp},
	[ABNE]=		{Cjmp},
	[ABGEZ]=	{Cjmp},
	[ABGTZ]=	{Cjmp},
	[ABLTZ]=	{Cjmp},
	[ABLEZ]=	{Cjmp},
	[ABFPF]=	{Cjmp},
	[ABFPT]=	{Cjmp},
	[ABGEZAL]=	{Call},
	[ABLTZAL]=	{Call},
	[ABREAK]=	{Break},
	[ARET]=		{Break},
};

void
proginfo(ProgInfo *info, Prog *p)
{
	*info = progtable[p->as];
	if(info->flags == 0)
		fatal("unknown instruction %P", p);

	if(p->from.type == D_CONST && p->from.sym != nil && (info->flags & LeftRead)) {
		info->flags &= ~LeftRead;
		info->flags |= LeftAddr;
	}

	if((info->flags & RegRead) && p->reg == NREG) {
		info->flags &= ~RegRead;
		info->flags |= CanRegRead | RightRead;
	}
}
