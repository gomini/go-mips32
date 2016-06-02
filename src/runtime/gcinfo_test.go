// Copyright 2014 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package runtime_test

import (
	"bytes"
	"runtime"
	"testing"
)

// TestGCInfo tests that various objects in heap, data and bss receive correct GC pointer type info.
func TestGCInfo(t *testing.T) {
	verifyGCInfo(t, "bss ScalarPtr", &bssScalarPtr, nonStackInfo(infoScalarPtr))
	verifyGCInfo(t, "bss PtrScalar", &bssPtrScalar, nonStackInfo(infoPtrScalar))
	verifyGCInfo(t, "bss BigStruct", &bssBigStruct, nonStackInfo(infoBigStruct()))
	verifyGCInfo(t, "bss string", &bssString, nonStackInfo(infoString))
	verifyGCInfo(t, "bss slice", &bssSlice, nonStackInfo(infoSlice))
	verifyGCInfo(t, "bss eface", &bssEface, nonStackInfo(infoEface))
	verifyGCInfo(t, "bss iface", &bssIface, nonStackInfo(infoIface))

	verifyGCInfo(t, "data ScalarPtr", &dataScalarPtr, nonStackInfo(infoScalarPtr))
	verifyGCInfo(t, "data PtrScalar", &dataPtrScalar, nonStackInfo(infoPtrScalar))
	verifyGCInfo(t, "data BigStruct", &dataBigStruct, nonStackInfo(infoBigStruct()))
	verifyGCInfo(t, "data string", &dataString, nonStackInfo(infoString))
	verifyGCInfo(t, "data slice", &dataSlice, nonStackInfo(infoSlice))
	verifyGCInfo(t, "data eface", &dataEface, nonStackInfo(infoEface))
	verifyGCInfo(t, "data iface", &dataIface, nonStackInfo(infoIface))

	verifyGCInfo(t, "stack ScalarPtr", new(ScalarPtr), infoScalarPtr)
	verifyGCInfo(t, "stack PtrScalar", new(PtrScalar), infoPtrScalar)
	verifyGCInfo(t, "stack BigStruct", new(BigStruct), infoBigStruct())
	verifyGCInfo(t, "stack string", new(string), infoString)
	verifyGCInfo(t, "stack slice", new([]string), infoSlice)
	verifyGCInfo(t, "stack eface", new(interface{}), infoEface)
	verifyGCInfo(t, "stack iface", new(Iface), infoIface)

	for i := 0; i < 10; i++ {
		verifyGCInfo(t, "heap ScalarPtr", escape(new(ScalarPtr)), nonStackInfo(infoScalarPtr))
		verifyGCInfo(t, "heap PtrScalar", escape(new(PtrScalar)), nonStackInfo(infoPtrScalar))
		verifyGCInfo(t, "heap BigStruct", escape(new(BigStruct)), nonStackInfo(infoBigStruct()))
		verifyGCInfo(t, "heap string", escape(new(string)), nonStackInfo(infoString))
		verifyGCInfo(t, "heap eface", escape(new(interface{})), nonStackInfo(infoEface))
		verifyGCInfo(t, "heap iface", escape(new(Iface)), nonStackInfo(infoIface))
	}

}

func verifyGCInfo(t *testing.T, name string, p interface{}, mask0 []byte) {
	mask := runtime.GCMask(p)
	if len(mask) > len(mask0) {
		mask0 = append(mask0, BitsDead)
		mask = mask[:len(mask0)]
	}
	if bytes.Compare(mask, mask0) != 0 {
		t.Errorf("bad GC program for %v:\nwant %+v\ngot  %+v", name, mask0, mask)
		return
	}
}

func nonStackInfo(mask []byte) []byte {
	// BitsDead is replaced with BitsScalar everywhere except stacks.
	mask1 := make([]byte, len(mask))
	mw := false
	for i, v := range mask {
		if !mw && v == BitsDead {
			v = BitsScalar
		}
		mw = !mw && v == BitsMultiWord
		mask1[i] = v
	}
	return mask1
}

var gcinfoSink interface{}

func escape(p interface{}) interface{} {
	gcinfoSink = p
	return p
}

const (
	BitsDead = iota
	BitsScalar
	BitsPointer
	BitsMultiWord
)

const (
	BitsString = iota // unused
	BitsSlice         // unused
	BitsIface
	BitsEface
)

type ScalarPtr struct {
	q int
	w *int
	e int
	r *int
	t int
	y *int
}

var infoScalarPtr = []byte{BitsScalar, BitsPointer, BitsScalar, BitsPointer, BitsScalar, BitsPointer}

type PtrScalar struct {
	q *int
	w int
	e *int
	r int
	t *int
	y int
}

var infoPtrScalar = []byte{BitsPointer, BitsScalar, BitsPointer, BitsScalar, BitsPointer, BitsScalar}

type BigStruct struct {
	q *int
	w byte
	e [17]byte
	r []byte
	t int
	y uint16
	u uint64
	i string
}

func infoBigStruct() []byte {
	switch runtime.GOARCH {
	case "386", "arm", "mips32", "mips32le":
		return []byte{
			BitsPointer,                                                // q *int
			BitsScalar, BitsScalar, BitsScalar, BitsScalar, BitsScalar, // w byte; e [17]byte
			BitsPointer, BitsDead, BitsDead, // r []byte
			BitsScalar, BitsScalar, BitsScalar, BitsScalar, // t int; y uint16; u uint64
			BitsPointer, BitsDead, // i string
		}
	case "amd64":
		return []byte{
			BitsPointer,                        // q *int
			BitsScalar, BitsScalar, BitsScalar, // w byte; e [17]byte
			BitsPointer, BitsDead, BitsDead, // r []byte
			BitsScalar, BitsScalar, BitsScalar, // t int; y uint16; u uint64
			BitsPointer, BitsDead, // i string
		}
	case "amd64p32":
		return []byte{
			BitsPointer,                                                // q *int
			BitsScalar, BitsScalar, BitsScalar, BitsScalar, BitsScalar, // w byte; e [17]byte
			BitsPointer, BitsDead, BitsDead, // r []byte
			BitsScalar, BitsScalar, BitsDead, BitsScalar, BitsScalar, // t int; y uint16; u uint64
			BitsPointer, BitsDead, // i string
		}
	default:
		panic("unknown arch")
	}
}

type Iface interface {
	f()
}

type IfaceImpl int

func (IfaceImpl) f() {
}

var (
	// BSS
	bssScalarPtr ScalarPtr
	bssPtrScalar PtrScalar
	bssBigStruct BigStruct
	bssString    string
	bssSlice     []string
	bssEface     interface{}
	bssIface     Iface

	// DATA
	dataScalarPtr             = ScalarPtr{q: 1}
	dataPtrScalar             = PtrScalar{w: 1}
	dataBigStruct             = BigStruct{w: 1}
	dataString                = "foo"
	dataSlice                 = []string{"foo"}
	dataEface     interface{} = 42
	dataIface     Iface       = IfaceImpl(42)

	infoString = []byte{BitsPointer, BitsDead}
	infoSlice  = []byte{BitsPointer, BitsDead, BitsDead}
	infoEface  = []byte{BitsMultiWord, BitsEface}
	infoIface  = []byte{BitsMultiWord, BitsIface}
)
