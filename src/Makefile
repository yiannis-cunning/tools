all: phex catn readPE converter

phex: phex.c
	gcc phex.c -o ../bin/phex

catn: catn.c
	gcc catn.c -o ../bin/catn


readPE: readPE.c
	gcc readPE.c -o ../bin/readPE

converter: converter.c
	gcc converter.c -o ../bin/converter


writePE: writePE.c
	gcc writePE.c -o ../bin/writePE

new_pe: writePE
	../bin/writePE CREATED_EXE
	chmod ugo+wrx CREATED_EXE

