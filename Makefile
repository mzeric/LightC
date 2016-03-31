
OBJS=token.o anode.o main.o decl.o ssa_cons2.o parse.o dfa.o ra.o lower.o \
     lua.o ssa_cons.o code.o
Headers=  anode.h
GUILE_INC=`pkg-config --static --cflags guile-2.0`
GUILE_LD=-lguile-2.0
LUA_INC=`pkg-config --cflags lua5.2`
LUA_LD=`pkg-config lua5.2 --libs`
CFLAGS=-g -std=c++0x ${GUILE_INC} ${LUA_INC}
CXXFLAGS=-g -std=c++0x ${LUA_INC}
LDFLAGS=${GUILE_LD} ${LUA_LD}
bison=bison
all: lc
lc: ${OBJS} ${Headers}
	${CXX} ${CXXFLAGS} ${OBJS} ${LDFLAGS} -g -o lc
.l.cpp:
parse.cc : parse.y
	${bison} -d -o parse.cc parse.y
token.cpp : token.l
	${bison} -d -o parse.cc parse.y
	flex -t token.l > token.cpp
%.o:%.c ${Headers}

lua.o : lua.cc
	${CXX} ${CXXFLAGS} -c lua.cc -o lua.o --static
guile.o : guile.cc
	${CXX} ${CXXFLAGS} -c ${GUILE_INC} guile.cc -o guile.o --static
clean:
	rm *.o parse.cc  token.cpp
.PHONY: all clean
