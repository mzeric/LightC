
OBJS=token.o anode.o main.o decl.o ssa_cons.o parse.o
Headers=  anode.h
CFLAGS=-g -std=gnu99
CXXFLAGS=-g
bison=/usr/local/bin/bison
all: lc
lc: ${OBJS} ${Headers}
	${CXX} ${OBJS} -g -ll -o lc 
.l.cpp:
parse.cc : parse.y
	${bison} -d -o parse.cc parse.y
token.cpp : token.l
	${bison} -d -o parse.cc parse.y
	lex -t token.l > token.cpp
%.o:%.c ${Headers}

clean:
	rm *.o parse.cc  token.cpp
