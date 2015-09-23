# Copyright 2012 The Go Authors.  All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

include ../../Make.dist

inst:	y.tab.h
	dist install -v && cp ../../../pkg/tool/linux_amd64/va /home/ren/bin

install: y.tab.h

y.tab.h: a.y
	LANG=C LANGUAGE=en_US.UTF8 bison -d -y a.y
