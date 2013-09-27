#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include "common.h"
#include "ast.h"
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
extern "C" void check(const char*str,...){

	printf(" [");
	printf(str);
	printf("] ");

}
////////////////////////////////////////////////////////////////////////////////////
LLVMContext *llvm_context;
Module *TheModule;
IRBuilder<> Builder(getGlobalContext());

typedef std::map<std::string, Node*> SymTable;
SymTable NamedValues;//函数内命名空间
std::map<std::string, Node*> NamedUnValues;//函数内命名空间

struct IRContext ir_context;

class TopContext{
private:
	TopContext(){}
public:
	SymTable* get(){
		return sk.top();
	}
	void push(SymTable *s){
		sk.push(s);
	}
	void pop(){
		if(!sk.empty())
			sk.pop();
	}
private:
	std::stack<SymTable*> sk;
};
	
Fvalue * A_error(const char *str){fprintf(stderr,"Error: %s\n",str);return NULL;}
void A_fatal(const char *str,int e){
	fprintf(stderr,"Error: %s\n",str);
	exit (e);
}
    
////////////////////////////////////////////////////////////////////////////
/////////////////// AST_declare->AST_local_var


/*!
	Read/Need value of the AST_var node
*/
void CodegenVisitor::visit(AST_var* p){
	debug_visit("ast_var");
	SymbolInfo *st = context->Lookup(p->decl_id);
	if(!st){

		std::cout<<"undefine symbol: "<<p->decl_id<<std::endl;
		assert("undefine symbol ");
	}
	if(st->value){
		p->ir = st->value;
		return;
	}
	/* evaluate the var, may be a parameter */
	std::cout << "var.isNull:" << p->decl_id << std::endl;
	if(st->node == NULL){
		/* un-init variable or func-parameter */
		
	}
	st->node->accept(this);
	p->ir = st->node->ir;


}
void CodegenVisitor::visit(AST_integer *p){
	if(p->ir)
		return;
   	p->ir = ConstantInt::get(*llvm_context,APInt(32,p->var_value));
    p->ir_type = p->ir->getType();
    std::cout << "visit int " <<p->ir->getType()->isIntegerTy()<<std::endl;
}
void CodegenVisitor::visit(AST_literal *p){
	/* alloc mem for const-string aka string_literal */
}
void CodegenVisitor::visit(AST_assignment *p){
	debug_visit("AST_assignment");
		if(p->ir)
			return;
		if( !(p->RHS->ir))
			p->RHS->accept(this);
		Fvalue *vc = p->RHS->ir;
		printf("assign :%X\n",vc);

		SymbolInfo *si = context->Lookup(p->lhs);
		si->value = p->RHS->ir;
		
		vc->setName(p->lhs);
		p->ir = vc;	
}
void CodegenVisitor::visit(AST_bin *p){
	debug_visit("AST_bin");
		Fvalue *lhs, *rhs;


		if(p->ir)
			return;
		if(!(p->LHS->ir))
			p->LHS->accept(this);
		if(!(p->RHS->ir))
			p->RHS->accept(this);
		lhs = p->LHS->ir;
		rhs = p->RHS->ir;

		printf("AST_bin:code(op:%c,%X,%X)\n",p->op,lhs,rhs);

		if(lhs == NULL||rhs == NULL)
			p->ir = NULL;
		switch (p->op){
		case '+':p->ir = Builder.CreateAdd(lhs, rhs, "add");
					break;
		case '-':p->ir = Builder.CreateSub(lhs, rhs, "sub");break;
		case '*':p->ir = Builder.CreateMul(lhs, rhs, "mul");break;
		case '/':p->ir = Builder.CreateSDiv(lhs, rhs, "div");break;
		case '<':
			p->ir = Builder.CreateICmpULT(lhs,rhs,"cmp");
			//return Builder.CreateUITOInt(lhs,
			//Type::getInt32Type(*llvm_context), "bool");
			break;
		default:
			A_error("无效的操作符");

		}
}
void CodegenVisitor::visit(AST_args *p){
std::cout << "visit AST_args " << p << std::endl;
}
void CodegenVisitor::visit(AST_call *p){
	debug_visit("visit AST_call");
	Function *func = TheModule->getFunction(p->name);
	if(func == NULL)
		A_error("未知的函数\n");
	if(func->arg_size() != p->args.size())
			A_error("参数错误\n");
	std::vector<Fvalue *> Argsv; //传给createcall的func参数列表

	for(unsigned int i = 0,e = p->args.size(); i != e; ++i){
		if(!(p->args[i]->ir))
			p->args[i]->accept(this);
    	Argsv.push_back(p->args[i]->ir);
		if ( Argsv.back() == NULL)
			return;
		}
	p->ir = Builder.CreateCall(func,Argsv,"call");
}
void CodegenVisitor::visit(AST_decl_var *p){
	std::cout << "visit AST_decl_var " << p << std::endl;
	// need deal with :
	// int i = j + 1;
}
void CodegenVisitor::visit(Declaration *p){
	std::cout << "visit Declaration " << p->declarator<< std::endl;

}
void CodegenVisitor::visit(AST_proto *p){
//		debug_visit("proto");
		std::cout << "visit proto now push" << std::endl;

		std::vector<Type*> int_args(p->args.size(),
					Type::getInt32Ty(*llvm_context));	//参数类型
		llvm::FunctionType * func_type = 
			llvm::FunctionType::get(Type::getInt32Ty(*llvm_context),//返回值类型
                              int_args,false);
		Function  *func = Function::Create(func_type,Function::ExternalLinkage,p->name,TheModule);
		if(func){
            printf("proto.ok\n");
        }
            //检查重名
		if(func->getName() != p->name){
			func->eraseFromParent();
			func = TheModule->getFunction(p->name);
			if(!func->empty()){
				A_error("重复定义");
				return;
			}
			if(func->arg_size()!= p->args.size()){
				A_error("重复定义，参数表不同");
			}
		}
              
            //保存参数名
		int i =0;
		Function::arg_iterator AI;
		for(AI = func->arg_begin();i<func->arg_size();AI++,i++){
			AI->setName(p->args[i].first);
			SymbolTable *st = p->context->Local();// == context->Current();
			st->info[p->args[i].first].value = AI;
			std::cout << "set args."<<p->args[i].first<<":"<< p->args[i].second.value<<std::endl;
//			AST_var* var = new AST_var(p->args[i].first);
//			var->ir = AI;
//			NamedValues[p->args[i].first] = var;
		}
	//	printf("proto:%x\n",func);
		p->ir = func;

}
/*!
	AST_func.body 含有{ }的context

*/
void CodegenVisitor::visit(AST_func *p){
//	p->code();
	int ret = 0;
	std::cout << "visit Func " << p->proto << std::endl;

    std::cout << "proto.push_namesapce" << std::endl;
	context->Push(p->proto->context);
	//dumpAllContext(context);

    if(p->proto->ir == NULL){
    	p->proto->accept(this);
    }
	std::cout << "visit Func " << p->proto << std::endl;
    Function* func = (Function*)(p->proto->ir);
    if(func == NULL){
        debug_visit("func.NULL");
        return;
    }
	debug_visit("begin body code\n");
    BasicBlock *func_block = BasicBlock::Create(*llvm_context,"entry",func);
    Builder.SetInsertPoint(func_block);
    Fvalue *ret_expr=ConstantInt::get(getGlobalContext(), APInt(32, 0));
   
    /*!
		Seriously , Push the {->sym_table, Now
		
    */
	if(p->body == NULL){// empyt func body
		ret = 0;

		goto empty_exit;

	}
	if(p->body->ir == NULL){
		context->Push(p->body->context);
		std::cout << "now in {" << std::endl;
		//dumpAllContext(context);
		p->body->accept(this);
	}
	debug_visit(p->body->ir);

    if (p->body->ir == NULL){
        //定义失败
        //FIXME
        func->eraseFromParent();
    	p->ir = NULL;
    	goto clean_exit;

    }

    if(p->body->ir)
    	ret_expr = p->body->ir;

    // ONLY non-empty function Pop here
	// Pop body symbol-table, ret_expr don't need this;
	context->Pop();

empty_exit:
    Builder.CreateRet(ret_expr);

    debug_visit("hello");
    verifyFunction(*func);

clean_exit:

	context->Pop();
	std::cout << "pop }.namespace & proto namesapce" << std::endl;

	func_block->end();
    p->ir = func;
    return;
    
}
