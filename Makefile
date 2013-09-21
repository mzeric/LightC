LLVM_ROOT=/opt/llvm/
CLANG_INC=/Users/walker/work/compiler/llvm-3.3.src/tools/clang/include

LLVM_VERSION=$(shell ${LLVM_ROOT}/bin/llvm-config --version|sed 's/\.//')

LLVM_CXXFLAGS=$(shell ${LLVM_ROOT}/bin/llvm-config --cppflags ) \
	-D LLVM_VERSION=${LLVM_VERSION}
LLVM_CXXFLAGS += -I${CLANG_INC}

LLVM_LDFLAGS=$(shell ${LLVM_ROOT}/bin/llvm-config --cppflags --ldflags --libs ) \
	-D LLVM_VERSION=${LLVM_VERSION}
LLVM_LDFLAGS += -I${CLANG_INC}

CC=clang++
objs:= parse.tab.o lex.yy.o ir.o type.o main.o
all: parse ${objs} link
parse: token.l parse.y
	flex  token.l
	bison parse.y -d
%.o:%.c 
	@echo CC $<
	@${CC} -c ${LLVM_CXXFLAGS} $< 
%.o:%.cpp
	@echo CC $<
	@${CC} -c ${LLVM_CXXFLAGS} $< 

link:
	@echo Linking ${objs}
	@${CC} -o 2  ${LLVM_LDFLAGS} ${objs}
clean:
	@rm *.o lex.yy.c parse.tab.c parse.tab.h 2 &>/dev/null
