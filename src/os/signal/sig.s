// Copyright 2012 The Go Authors.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Assembly to get into package runtime without using exported symbols.

// +build amd64 amd64p32 arm 386 mips32 mips32le

#include "textflag.h"

#ifdef GOARCH_arm
#define JMP B
#endif

TEXT ·signal_disable(SB),NOSPLIT,$0
	JMP runtime·signal_disable(SB)

TEXT ·signal_enable(SB),NOSPLIT,$0
	JMP runtime·signal_enable(SB)

TEXT ·signal_recv(SB),NOSPLIT,$0
	JMP runtime·signal_recv(SB)

