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
LLVMContext *llvm_context;
extern "C" int yyparse(void);
typedef Value Fvalue;
typedef std::string Fstring;
static Module *TheModule;
static IRBuilder<> Builder(getGlobalContext());
static std::map<std::string, Value*> NamedValues;

class AST_expr{
public:
AST_expr(){}
virtual ~AST_expr(){};
virtual Fvalue *code()=0;
};
Fvalue * A_error(const char *str){fprintf(stderr,"Error: %s\n",str);return NULL;}
class AST_integer: public AST_expr{
public:
	
	int var_value;
	
	AST_integer(int val):var_value(val){}
	virtual Fvalue *code();
};
Fvalue *AST_integer::code(){
	return ConstantInt::get(*llvm_context,APInt(32,var_value));


}
class AST_var	: public AST_expr{
public:
	Fstring var_name;
AST_var(const Fstring &name):var_name(name){}
virtual Fvalue *code();

};
Fvalue *AST_var::code(){

	Fvalue *V = NamedValues[var_name];
	return V?V:NULL;

}
class AST_bin   : public AST_expr{
public:
	char op;
	AST_expr *LHS, *RHS;
	AST_bin(char op, AST_expr *lhs, AST_expr* rhs):op(op),LHS(lhs),RHS(rhs){}
virtual Fvalue *code();

};
Fvalue *AST_bin::code(){
	Fvalue *lhs = LHS->code();
	Fvalue *rhs = RHS->code();

	if(lhs == NULL||rhs == NULL)
		return NULL;
		switch (op){
		case '+':return Builder.CreateAdd(lhs,rhs,"add");break;
		case '-':return Builder.CreateSub(lhs,rhs,"sub");break;
		case '*':return Builder.CreateMul(lhs,rhs,"mul");break;
		case '<':
		return Builder.CreateICmpULT(lhs,rhs,"cmp");
		//return Builder.CreateUITOInt(lhs,
		//Type::getInt32Type(*llvm_context), "bool");
			break;
		default:
			return A_error("无效的操作符");


		}
	


}
class AST_call  : public AST_expr{
public:
	Fstring name;
	std::vector<AST_expr*> args;
	AST_call(const Fstring &func, std::vector<AST_expr*> &args):name(func), args(args){}
virtual Fvalue *code();
};
Fvalue *AST_call::code(){
  Function *func = TheModule->getFunction(name);
	if(func == NULL)
		return A_error("未知的函数\n");
	if(func->arg_size() != args.size())
		return A_error("参数错误\n");
	std::vector<Fvalue *> Argsv; //传给createcall的func参数列表

	for(unsigned int i = 0,e = args.size(); i != e; ++i){

		Argsv.push_back(args[i]->code());
		if ( Argsv.back() == NULL) return 0;

	}
	return Builder.CreateCall(func,Argsv.begin(),Argsv.end(),"call");
}
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
  llvm_context = &Context;
  TheModule = new Module("cool jit",*llvm_context);

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
