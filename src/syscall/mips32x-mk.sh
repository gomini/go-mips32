#!/bin/sh

S="/home/ren/opt/openwrt/staging_dir"
export STAGING_DIR="$S"

# errors
GOOS=linux GOARCH=mips32 ./mips32x-err.sh -b32 | gofmt > zerrors_linux_mips32.go
GOOS=linux GOARCH=mips32le ./mips32x-err.sh -l32 | gofmt > zerrors_linux_mips32le.go

# syscall
GOOS=linux GOARCH=mips32 ./mksyscall.pl -b32 -mips32 syscall_linux.go syscall_linux_mips32x.go |gofmt >zsyscall_linux_mips32.go
GOOS=linux GOARCH=mips32le ./mksyscall.pl -l32 -mips32 syscall_linux.go syscall_linux_mips32x.go |gofmt >zsyscall_linux_mips32le.go

# sysnum
./mips32x-num.sh -b32 | gofmt > zsysnum_linux_mips32.go
./mips32x-num.sh -l32 | gofmt > zsysnum_linux_mips32le.go

# types
CC="$S/toolchain-mips_34kc_gcc-4.6-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-gcc"
GOOS=linux GOARCH=mips32 CC=$CC go tool cgo -godefs types_linux.go | gofmt > ztypes_linux_mips32.go

CC="$S/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mipsel-openwrt-linux-gcc"
GOOS=linux GOARCH=mips32le CC=$CC go tool cgo -godefs types_linux.go | gofmt > ztypes_linux_mips32le.go

