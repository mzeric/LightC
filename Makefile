LLVM_ROOT=/opt/llvm/
LLVM_VERSION=$(shell ${LLVM_ROOT}/bin/llvm-config --version|sed 's/\.//')

LLVM_CXXFLAGS=$(shell ${LLVM_ROOT}/bin/llvm-config --cppflags ) \
	-D LLVM_VERSION=${LLVM_VERSION}

LLVM_LDFLAGS=$(shell ${LLVM_ROOT}/bin/llvm-config --cppflags --ldflags --libs core) \
	-D LLVM_VERSION=${LLVM_VERSION}

CC=g++
all:
	flex  token.l
	bison parse.y -d
	${CC} lex.yy.c -c ${LLVM_CXXFLAGS}  
	${CC} parse.tab.c -c ${LLVM_CXXFLAGS} 
	${CC} -o 2 main.cpp ${LLVM_LDFLAGS} parse.tab.o lex.yy.o 
clean:
	@rm *.o lex.yy.c parse.tab.c parse.tab.h 2 &>/dev/null
