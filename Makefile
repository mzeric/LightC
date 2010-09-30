LLVM_FLAGS=$(shell /opt/llvm/bin/llvm-config --cppflags --ldflags --libs core)
CC=g++
all:
	exit 
	flex  2.l
	bison 2.y -d
	g++ lex.yy.c -c ${LLVM_FLAGS}
	g++ 2.tab.c -c ${LLVM_FLAGS}
	g++ -o 2 main.cpp ${LLVM_FLAGS} 2.tab.o lex.yy.o
clean:
	@rm *.o lex.yy.c 2.tab.c 2.tab.h 2 &>/dev/null
