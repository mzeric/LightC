#include "common.h"
#include "ast.h"
#include "type.h"
using namespace llvm;
int main(int argc, char * const *argv){
        
		LLVMContext &Context = getGlobalContext();
		llvm_context = &Context;


		current_ast_context = new ASTContext();// The default ContextImpl
		current_ast_context->Push();// Gloable Symbol Table;

		TheModule = new Module("cool jit",*llvm_context);
 /*		       
		std::vector<const Type*> Doubles(2, Type::getDoubleTy(getGlobalContext()));

		FunctionType *FT = FunctionType::get(Type::getDoubleTy(getGlobalContext()),
                                             Doubles, false);
        
            //Function *F = Function::Create(FT, Function::ExternalLinkage, "main", TheModule);
		Function *F = A_FUNC_proto(FT, "main", TheModule);

        
		BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
		Builder.SetInsertPoint(BB);
        
        
		Function::arg_iterator AI = F->arg_begin();
		AI->setName("he");
		NamedValues["he"] = AI;
        
		Value *va = ConstantFP::get(getGlobalContext(),APFloat((float)3));
		Value *vb = NULL;
		vb = Builder.CreateFAdd(NamedValues["he"],NamedValues["he"],"he");
		vb->setName("hea");
		Builder.CreateRet(vb);

		std::vector<Fstring> my_args;
		my_args.push_back("first");
		my_args.push_back("second");
		AST_integer *vint = new AST_integer(456);
		AST_var *my_var   = new AST_var("first");
		AST_proto* my_proto = new AST_proto("my_func",my_args);



		AST_func * my_func  = new AST_func (my_proto,my_var);
		std::vector<AST_expr*> my_call_args;
		my_call_args.push_back(new AST_integer(456));
		my_call_args.push_back(new AST_integer(789));
		AST_call * my_call = new AST_call("my_func",my_call_args);
		my_func->code();
  */
  		yyparse();
		//my_call->code();
		
    /*    
		TheModule->dump();
		SMDiagnostic diag;
    	//Builder.SetInsertPoint(func_block);
    	llvm::BumpPtrAllocator bp;
		ASTType * ta = (ASTType*)bp.Allocate(sizeof(ASTType),4);
    	typedef llvm::PointerIntPair<ASTType*, 3> PU;
    	PU vl(ta,5);
    	std::cout << ta <<" restore : " << vl.getInt() << " "<<vl.getPointer() << std::endl;
    	#include <llvm/Support/Alignof.h>

    	std::cout << "alignof  " << alignOf<Type*>()<< std::endl;
    	BuiltinType *t = new BuiltinType();
    	//QualType *t = dyn_cast<QualType>(d);
    	if (dyn_cast<BuiltinType>(t))
    		std::cout << "get dyn_cast" << std::endl;
		//ModuleTest->dump();

	*/
		return 0;
        
        
    
}
