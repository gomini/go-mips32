#!/usr/bin/env bash

unset LANG
export LC_ALL=C
export LC_CTYPE=C

cc=${CC:-gcc}
S="/home/ren/opt/openwrt/staging_dir"

case "$1" in
-l32)
	CC="$S/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mipsel-openwrt-linux-gcc"
	shift
	;;
-b32)
	CC="$S/toolchain-mips_34kc_gcc-4.6-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-gcc"
	shift
	;;
*)
	echo "$0 -b32|-l32 [cflags]"
	exit 1
esac
export STAGING_DIR="$S"

#$CC -v || exit 1

uname=$(uname)

echo "#include <stdio.h>" > _n.c
echo "#include <stdlib.h>" >> _n.c
echo "#include <ctype.h>" >> _n.c
echo "#include <string.h>" >> _n.c
echo >> _n.c
echo "#include <asm/unistd.h>" | $CC -x c - -E -dM | grep __NR_Linux | grep __NR_Linux_syscalls -v >> _n.c

defs=$(
	echo "#include <asm/unistd.h>" | $CC -x c - -E -dM |
	grep __NR_Linux | grep __NR_Linux_syscalls -v |
	awk '$1=="#define" { print $2 }' |
	sort
)

(
	echo -E "

typedef struct T {
	int	i;
	char*	s;
}
T;

enum { A = 'A', Z = 'Z', a = 'a', z = 'z' }; // avoid need for single quotes below

T defs[] = {
"
	for i in $defs
	do
		echo -E '	'\{$i,\"$i\"\},
	done

	echo -E '
};

#define nelem(x) (sizeof(x)/sizeof((x)[0]))

static int
intcmp(const void *a, const void *b)
{
	return *(int*)a - *(int*)b;
}

int
main(void)
{
	int i, j;
	char buf[1024];

	qsort(defs, nelem(defs), sizeof defs[0], intcmp);

	printf("package syscall\n");
	printf("const (\n");
	for(i=0; i<nelem(defs); i++) {
		if(strcmp(defs[i].s, "__NR_Linux")==0)
			continue;
		// j=5 to skip __NR_
		for(j=5; defs[i].s[j]; j++) {
			buf[j-5] = defs[i].s[j];
			if(buf[j-5] >= a && buf[j-5] <= z)
				buf[j-5] = buf[j-5] - (a - A);
		}
		buf[j-5] = 0;
		printf("\tSYS_%s = %d\n", buf, defs[i].i);
	}
	printf(")\n");

	return 0;
}

'
) >> _n.c

$cc -o _n _n.c && ./_n && rm -f _n.c _n
