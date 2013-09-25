#include "ast.h"


ASTContextImpl ContextImpl;
ASTContext *current_ast_context;
ASTContext* getCurrentContext(){
	assert(current_ast_context && " Please Init ast_context First! ");
	return current_ast_context;
}
