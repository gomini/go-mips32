
// +build mips32 mips32le

#include "runtime.h"
#include "defs_GOOS_GOARCH.h"
#include "os_GOOS.h"
#include "textflag.h"

#define AT_NULL		0
#define AT_RANDOM	25 // introduced in 2.6.29

static uint32 runtime·randomNumber;

#pragma textflag NOSPLIT
void
runtime·setup_auxv(int32 argc, byte **argv)
{
	byte **envp;
	byte *rnd;
	uint32 *auxv;

	for(envp = &argv[argc+1]; *envp != nil; envp++)
		;
	envp++;
	
	for(auxv=(uint32*)envp; auxv[0] != AT_NULL; auxv += 2) {
		switch(auxv[0]) {
		case AT_RANDOM:
			if(auxv[1]) {
				rnd = (byte*)auxv[1];
				runtime·randomNumber = rnd[4] | rnd[5]<<8 | rnd[6]<<16 | rnd[7]<<24;
			}
			break;
		}
	}
}


#pragma textflag NOSPLIT
int64
runtime·cputicks(void)
{
	return runtime·nanotime() + runtime·randomNumber;
}
