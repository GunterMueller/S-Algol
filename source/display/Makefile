include $(ARCH)_Makefile

ROPHS= fropTBRL.h fropTBLR.h fropBTLR.h
ROPS= frop.o
PROPS= frop.p.o
GROPS= frop.g.o

all: $(ARCH)_ofiles $(ARCH)_libdisplay.a $(ARCH)_libdisplay_p.a $(ARCH)_libdisplay_g.a $(ARCH)_tester

$(ARCH)_ofiles:
	@- mv *.o `cat *_ofiles`/
	(make clean)
	@- mkdir $(ARCH)
	@- mv $(ARCH)/*.o .
	csh -fc 'echo -n $(ARCH) > $(ARCH)_ofiles'

clean:
	rm -f *_ofiles $(ARCH)_libdisplay.a $(ARCH)_libdisplay_p.a $(ARCH)_libdisplay_g.a *.o $(ARCH)_tester

$(ARCH)_libdisplay.a: display.o $(ROPS) rop_lines.o
	rm -f $(ARCH)_libdisplay.a
	ar rc $(ARCH)_libdisplay.a display.o $(ROPS) rop_lines.o
	ranlib $(ARCH)_libdisplay.a

$(ARCH)_libdisplay_p.a: display.p.o $(PROPS) rop_lines.p.o
	rm -f $(ARCH)_libdisplay_p.a
	ar rc $(ARCH)_libdisplay_p.a display.p.o $(PROPS) rop_lines.p.o
	ranlib $(ARCH)_libdisplay_p.a

$(ARCH)_libdisplay_g.a: display.g.o $(GROPS) rop_lines.g.o
	rm -f $(ARCH)_libdisplay_g.a
	ar rc $(ARCH)_libdisplay_g.a display.g.o $(GROPS) rop_lines.g.o
	ranlib $(ARCH)_libdisplay_g.a

$(ARCH)_tester: tester.g.o $(ARCH)_libdisplay_g.a
	$(CC) -o $(ARCH)_tester tester.g.o $(ARCH)_libdisplay_g.a -lX11

display.g.o display.p.o display.o: libdisplay.h display.h icon.h

$(GROPS) $(PROPS) $(ROPS): $(ROPHS)
