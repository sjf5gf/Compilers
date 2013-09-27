
all: turtle-sjf5gf eins.ps

eins.ps: eins.tlt turtle-sjf5gf
	./turtle-sjf5gf < eins.tlt > eins.ps

lex.yy.o:  lex.yy.c turtle-sjf5gf.tab.h symtab.h
	gcc -c lex.yy.c

lex.yy.c: turtle.l
	flex turtle.l
	
turtle-sjf5gf.tab.h: turtle-sjf5gf.y
	bison -d turtle-sjf5gf.y
	
turtle-sjf5gf.tab.c: turtle-sjf5gf.y
	bison -d turtle-sjf5gf.y
	
turtle-sjf5gf.tab.o: turtle-sjf5gf.tab.c symtab.h
	gcc -c turtle-sjf5gf.tab.c

turtle-sjf5gf: turtle-sjf5gf.tab.o lex.yy.o symtab.c
	gcc lex.yy.o turtle-sjf5gf.tab.o symtab.c -lfl -o turtle-sjf5gf

clean:
	rm -f turtle-sjf5gf lex.* turtle-sjf5gf.tab.* *.o eins.ps

