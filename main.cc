#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "lexer.h"
#include "anode.h"
#include "parse.hh"

extern int yydebug;
extern char *yytext;
extern int yyleng;
int main(){
	int i;	
	/*
	while((i = yylex())){
		if (i == CONSTANT_INT || i == CONSTANT)
			printf("yylex c[%s]:%d\n",yytext, yyleng);

	}
	*/
	yydebug = 0;
        c_parse_init();
	yyparse();
	printf("after all\n");
}
