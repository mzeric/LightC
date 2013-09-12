LLVM_ROOT=/opt/llvm/
LLVM_VERSION=$(shell ${LLVM_ROOT}/bin/llvm-config --version|sed 's/\.//')

LLVM_CXXFLAGS=$(shell ${LLVM_ROOT}/bin/llvm-config --cppflags ) \
	-D LLVM_VERSION=${LLVM_VERSION}

LLVM_LDFLAGS=$(shell ${LLVM_ROOT}/bin/llvm-config --cppflags --ldflags --libs core) \
	-D LLVM_VERSION=${LLVM_VERSION}

CC=clang++
objs:= parse.tab.o lex.yy.o main.o
all: parse ${objs} link
parse: token.l parse.y
	flex  token.l
	bison parse.y -d
%.o:%.c 
	${CC} -c ${LLVM_CXXFLAGS} $< 
%.o:%.cpp
	${CC} -c ${LLVM_CXXFLAGS} $< 

link:
	${CC} -o 2  ${LLVM_LDFLAGS} ${objs}
clean:
	@rm *.o lex.yy.c parse.tab.c parse.tab.h 2 &>/dev/null
