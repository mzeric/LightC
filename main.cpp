//extern "C"
#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"
#include <cstdio>
#include <string>
#include <map>
#include <vector>
using namespace llvm;
#if 0
#ifdef _cplusplus
extern "C" {
#endif
#include "2.tab.h"
#ifdef _cplusplus
#endif

#endif
#define C_EXPORT extern  "C"
extern "C" int yyparse(void);
typedef Value Fvalue;
typedef std::string Fstring;
static Module *TheModule;
static IRBuilder<> Builder(getGlobalContext());
static std::map<std::string, Value*> NamedValues;

class AST_expr{
public:
	virtual ~AST_expr(){}
	virtual Fvalue *code();
};
class AST_number: public AST_expr{
public:
	
	int ivalue;
	
	
	AST_number(int val):ivalue(val){}
	virtual Value *code();
};
class AST_var	: public AST_expr{
public:
	Fstring name;
AST_var(const Fstring &name):name(name){}
virtual Fvalue *code();

};
class AST_bin   : public AST_expr{
public:
	char op;
	AST_expr *LHS, *RHS;
	AST_bin(char op, AST_expr *lhs, AST_expr* rhs):op(op),LHS(lhs),RHS(rhs){}
virtual Fvalue *code();

};
class AST_call  : public AST_expr{
public:
	Fstring func;
	std::vector<AST_expr*> args;
	AST_call(const Fstring &func, std::vector<AST_expr*> &args):func(func), args(args){}
virtual Fvalue *code();
};
class AST_proto {
public:
	Fstring name;
	std::vector<Fstring> args;
AST_proto(const Fstring &name,	const std::vector<Fstring> &args):name(name),args(args){}
Function *code();
};
class AST_func{
public:
	AST_proto *proto;
	AST_expr  *body;
	AST_func(AST_proto* proto, AST_expr* body): proto(proto),body(body){}
Function *code();


};

C_EXPORT {

Function* A_FUNC_proto(FunctionType* ft, char* name, Module*module){


	Function *F = Function::Create(ft, Function::ExternalLinkage, name, module);

	return F;

}

void A_FUNC_define_begin(Function *){




}



void A_EXP_(int a,int b,char*op){
		
		Value *va = ConstantFP::get(getGlobalContext(),APFloat((float)a));
		printf("A:%x B:%d,%s\n",a,b,op);
		Builder.CreateFAdd(va,va,"addtmp");
		

	}


}

int main(){

  LLVMContext &Context = getGlobalContext();
  TheModule = new Module("cool jit",Context);

//	yyparse();
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
	Value *vb = Builder.CreateFAdd(NamedValues["he"],NamedValues["he"],"addtmp");
	Builder.CreateRet(vb);




  TheModule->dump();
	return 0;


}
