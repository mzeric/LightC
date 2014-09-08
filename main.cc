#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "lexer.h"
#include "anode.h"
#include "parse.hh"
#include "dfa.h"
#include "lower.h"
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
	fflush(stdin);
	printf("after all\n");
	dump_block_list(top_ast_node, 0);
	build_func_cfg(top_ast_node);//, ENTRY_BLOCK_PTR, ENTRY_BLOCK_PTR, "start");

	simplify_bb(ENTRY_BLOCK_PTR->next);
	//make_edge(get_bb(ENTRY_BLOCK_PTR, 8), get_bb(ENTRY_BLOCK_PTR, 6), 0);
	lower_bb(ENTRY_BLOCK_PTR->next);
	build_ssa(ENTRY_BLOCK_PTR->next);
	dfa_handle(ENTRY_BLOCK_PTR->next);

	dump_bb(ENTRY_BLOCK_PTR);


}
