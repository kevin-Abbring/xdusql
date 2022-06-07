y: sqlyacc.y
	bison -d sqlyacc.y

f: sqlflex.l
	flex sqlflex.l

cg: dbfunction.c sqlyacc.tab.c  lex.yy.c
	gcc -c -g dbfunction.c sqlyacc.tab.c  lex.yy.c

o:	dbfunction.o lex.yy.o sqlyacc.tab.o
	gcc dbfunction.o lex.yy.o sqlyacc.tab.o -o  xdusql

run: y f cg o
clean:
	rm lex.yy.c sqlyacc.tab.c sqlyacc.tab.h dbfunction.o lex.yy.o sqlyacc.tab.o xdusql
