
OBJS=token.o anode.o main.o decl.o ssa_cons.o parse.o dfa.o ra.o lower.o \
     guile.o lua.o
Headers=  anode.h
GUILE_INC=`pkg-config --static --cflags guile-2.0`
GUILE_LD=-lguile-2.0
CFLAGS=-g -std=gnu99 ${GUILE_INC}
CXXFLAGS=-g
LDFLAGS=${GUILE_LD}
bison=bison
all: lc
lc: ${OBJS} ${Headers}
	${CXX} ${OBJS} ${LDFLAGS} -g -o lc
.l.cpp:
parse.cc : parse.y
	${bison} -d -o parse.cc parse.y
token.cpp : token.l
	${bison} -d -o parse.cc parse.y
	flex -t token.l > token.cpp
%.o:%.c ${Headers}

lua.o : lua.cc
	${CXX} -c lua.cc -o lua.o --static
guile.o : guile.cc
	${CXX} -c ${GUILE_INC} guile.cc -o guile.o --static
clean:
	rm *.o parse.cc  token.cpp
.PHONY: all clean
