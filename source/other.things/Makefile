#.SUFFIXES:	.out .S
#.S.out:
#	../../distribution/lib/exec/$(ARCH)_sc $<

SC=../../distribution/lib/exec/$(ARCH)_sc

all: rasters reals sops elide

install: all
	cp -r  S.stand ../../distribution/lib
	cp -r  S.prelude ../../distribution/lib
	mv Sraster.out  ../../distribution/lib/runtime/$(ARCH)_Sraster.out
	mv realio.out  ../../distribution/lib/runtime/$(ARCH)_realio.out

clean:
	rm -f Sraster.out realio.out sops.out elide.out

rasters: 
	$(SC) Sraster.S
reals:
	$(SC) realio.S
sops:
	$(SC) sops.S
elide:
	$(SC) elide.S
