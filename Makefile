LLVM_FLAGS=$(shell /opt/llvm/bin/llvm-config --cppflags --ldflags --libs core)
CC=g++
all:
	flex  token.l
	bison parse.y -d
	g++ lex.yy.c -c ${LLVM_FLAGS} -lfl
	g++ parse.tab.c -c ${LLVM_FLAGS}
	g++ -o 2 main.cpp ${LLVM_FLAGS} parse.tab.o lex.yy.o
clean:
	@rm *.o lex.yy.c parse.tab.c parse.tab.h 2 &>/dev/null
