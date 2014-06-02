
OBJS=token.o anode.o main.o decl.o ssa_cons.o parse.o dfa.o
Headers=  anode.h
CFLAGS=-g -std=gnu99
CXXFLAGS=-g
bison=bison
all: lc
lc: ${OBJS} ${Headers}
	${CXX} ${OBJS} -g -o lc 
.l.cpp:
parse.cc : parse.y
	${bison} -d -o parse.cc parse.y
token.cpp : token.l
	${bison} -d -o parse.cc parse.y
	flex -t token.l > token.cpp
%.o:%.c ${Headers}

clean:
	rm *.o parse.cc  token.cpp
