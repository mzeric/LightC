#include "ast.h"


ASTContextImpl ContextImpl;
ASTContext *current_ast_context;
ASTContext* getCurrentContext(){
	assert(current_ast_context && " Please Init ast_context First! ");
	return current_ast_context;
}
//


void dumpSymbolTable(SymbolTable *st){
	SymbolTable::Iterator iter_t = st->info.begin();
	for(; iter_t != st->info.end(); ++iter_t){
		std::cout << "Symbol["<<iter_t->first<< "]" << iter_t->second.value << std::endl;
	}
}
void dumpAllContext(){
	ASTContext *c = current_ast_context;
	ASTContext::Iterator iter_c = c->begin();
	for(;iter_c != c->end(); ++iter_c)
		dumpSymbolTable(*iter_c);

}
void dumpAllContext(ASTContext *c){
	ASTContext::Iterator iter_c = c->begin();
	for(;iter_c != c->end(); ++iter_c)
		dumpSymbolTable(*iter_c);
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
