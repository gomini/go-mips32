
// +build mips32le

package runtime

import "unsafe"

//go:nosplit
func atomicor8(addr *uint8, v uint8) {
	uaddr := uintptr(unsafe.Pointer(addr))
	addr32 := (*uint32)(unsafe.Pointer(uaddr &^ 3))
	word := uint32(v) << ((uaddr & 3) * 8)
	for {
		old := *addr32
		if cas(addr32, old, old|word) {
			return
		}
	}
}
