#include "ast.h"


ASTContextImpl ContextImpl;
ASTContext *current_ast_context;
ASTContext* getCurrentContext(){
	assert(current_ast_context && " Please Init ast_context First! ");
	return current_ast_context;
}
//
void dumpAllContext(){
	ASTContext *c = current_ast_context;
	ASTContext::Iterator iter_c = c->begin();
	for(;iter_c != c->end(); ++iter_c){
		SymbolTable::Iterator iter_t = (*iter_c)->info.begin();
		for(;iter_t != (*iter_c)->info.end(); ++iter_t){
			std::cout << "Symbol["<<iter_t->first<< "]" << iter_t->second.value << std::endl;
		}

	}
}
