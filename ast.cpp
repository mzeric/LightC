#include "ast.h"


ASTContextImpl ContextImpl;
ASTContext *current_ast_context;
ASTContext* getCurrentContext(){
	assert(current_ast_context && " Please Init ast_context First! ");
	return current_ast_context;
}
//

AST_decl_var::AST_decl_var(AST_decl *s, Node *v ){
		decl_id = s->decl_id;
		if(v){
			init_value = v->ir;
			decl_node = v;
		}

		printf("[AST_local_var::add_v]");
}

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

SymbolInfo* ASTContext::Lookup(Fstring str){

	ASTContext::Iterator iter_c = begin();
	SymbolTable::Iterator iter_t ;
	for(;iter_c != end(); ++iter_c){
		iter_t = (*iter_c)->info.begin();
		for(;iter_t != (*iter_c)->info.end(); ++iter_t){
			if(iter_t->first == str)
				return &iter_t->second;
		}

	}
	return NULL;
}
