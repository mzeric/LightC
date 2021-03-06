#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "lexer.h"
#include "anode.h"
#include "parse.hh"
#include "dfa.h"
#include "lower.h"
#include "script.h"
#include "code.h"
#include "code_legacy.h"
extern int yydebug;
extern char *yytext;
extern int yyleng;
extern void build_ssa_legacy(basic_block_t *p);
int main(){
	int i;	
	/*
	while((i = yylex())){
		if (i == CONSTANT_INT || i == CONSTANT)
			printf("yylex c[%s]:%d\n",yytext, yyleng);

	}
	*/


	//_init_guile();

	yydebug = 0;
        c_parse_init();
	yyparse();
	fflush(stdin);
	printf("after all\n");
	//dump_block_list(top_ast_node, 0);
	build_func_cfg(top_ast_node);//, ENTRY_BLOCK_PTR, ENTRY_BLOCK_PTR, "start");
	//_call_guile_cfg(ENTRY_BLOCK_PTR->next);
	simplify_bb(ENTRY_BLOCK_PTR->next);
	//make_edge(get_bb(ENTRY_BLOCK_PTR, 8), get_bb(ENTRY_BLOCK_PTR, 6), 0);
	lower_bb(ENTRY_BLOCK_PTR->next);

	_call_lua_cfg(ENTRY_BLOCK_PTR);
	//build_ssa_legacy(ENTRY_BLOCK_PTR);
	//code_gen(ENTRY_BLOCK_PTR);
	code_gen_legacy(ENTRY_BLOCK_PTR);
	//build_ssa(ENTRY_BLOCK_PTR);
	//_call_guile_ssa(ENTRY_BLOCK_PTR->next);
	//dfa_handle(ENTRY_BLOCK_PTR->next);

	//dump_bb(ENTRY_BLOCK_PTR);


}
