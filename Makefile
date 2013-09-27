LLVM_ROOT=/opt/llvm/
CLANG_INC=/Users/walker/work/compiler/llvm-3.3.src/tools/clang/include
BOOST_INC=/Users/walker/boost_src/

LLVM_VERSION=$(shell ${LLVM_ROOT}/bin/llvm-config --version|sed 's/\.//')

LLVM_CXXFLAGS=$(shell ${LLVM_ROOT}/bin/llvm-config --cppflags ) \
	-D LLVM_VERSION=${LLVM_VERSION}
LLVM_CXXFLAGS += -I${CLANG_INC} \
				-I${BOOST_INC}

LLVM_LDFLAGS=$(shell ${LLVM_ROOT}/bin/llvm-config --cppflags --ldflags --libs ) \
	-D LLVM_VERSION=${LLVM_VERSION}
LLVM_LDFLAGS += -I${CLANG_INC}

CC=clang++
objs:= parser.o lexer.o ir.o ast.o type.o main.o
all: parse ${objs} link
parse: token.l parse.y
	flex -o lexer.c token.l
	bison -d -o parser.c parse.y
%.o:%.c 
	@echo CC $<
	@${CC} -c ${LLVM_CXXFLAGS} $< 
%.o:%.cpp
	@echo CC $<
	@${CC} -c ${LLVM_CXXFLAGS} $< 

link:
	@echo Linking ${objs}
	@${CC} -o lc  ${LLVM_LDFLAGS} ${objs}
clean:
	@rm *.o lexer.c parser.c parser.h 2&>/dev/null
