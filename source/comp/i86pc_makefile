CFLAGS = -O1  -g -DALPHA -DOUTLINE -DRASTER
#CFLAGS = -g -DM68000 -DOUTLINE -DRASTER
LFLAGS = -DM68000 -DOUTLINE -DRASTER
CC = gcc
OFILES = csc.o io.o cgen.o stype.o syntax1.o syntax2.o
LFILES = csc.c io.c cgen.c stype.c syntax1.c syntax2.c

csc:	i86pc $(OFILES)
#	cc ${CFLAGS} -o sc $(OFILES)
	gcc ${CFLAGS} -g -o sc $(OFILES)

LINT:
	lint ${LFLAGS} $(LFILES)

io.o:	defines.h functions.h
cgen.o stype.o:	defines.h stype.h functions.h
syntax1.o syntax2.o: defines.h stype.h symbols.h cgen.h lex.h functions.h
csc.o:	defines.h externs.h stype.h mktype.h functions.h

install: csc
	mv sc ../../distribution/lib/exec/i86pc_sc

i86pc:
	make clean
	echo > 186pc
clean:
	rm -f sc *.o sun3 sun4 alpha i86pc
