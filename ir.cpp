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

AST_decl_var::AST_decl_var(AST_decl *s, Node *v ){
		decl_id = s->decl_id;
		if(v)
		init_value = v->ir;

		if(v == NULL){
			NamedUnValues[s->decl_id] =  NULL;

		}{
			NamedValues[s->decl_id] = v;

		}

		printf("[AST_local_var::add_v]");
}

void CodegenVisitor::visit(AST_var* p){
	debug_visit("ast_var");
	Node *V = NamedValues[p->decl_id];
	p->ir = V?V->ir:NULL;
	//p->context->Lookup(p->var_id);

}
void CodegenVisitor::visit(AST_integer *p){

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
		NamedValues[p->lhs]=p->RHS;
		
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
		std::cout << "visit proto" << std::endl;
		std::vector<Type*> int_args(p->args.size(),
					Type::getInt32Ty(*llvm_context));	//参数类型
		llvm::FunctionType * func_type = 
			llvm::FunctionType::get(Type::getInt32Ty(*llvm_context),//返回值类型
                              int_args,false);
		Function	* func = Function::Create(func_type,Function::ExternalLinkage,p->name,TheModule);
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
			AI->setName(p->args[i]);
			AST_var* var = new AST_var(p->args[i]);
			var->ir = AI;
			NamedValues[p->args[i]] = var;
		}
	//	printf("proto:%x\n",func);
		p->ir = func;

}

void CodegenVisitor::visit(AST_func *p){
//	p->code();
	std::cout << "visit Func " << p->proto << std::endl;
    NamedValues.clear();//清除名字空间(符号表)
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
    Fvalue *ret_expr=NULL;
    
    if(p->ret_value){
    

        ret_expr = p->ret_value;
    }else{
        
    	if(p->body->ir == NULL)
    		p->body->accept(this);
    	debug_visit(p->body->ir);
        ret_expr=p->body->ir;
    }
    if(ret_expr ){
        Builder.CreateRet(ret_expr);
        verifyFunction(*func);
        func_block->end();
        p->ir = func;

        return;
    }
        //定义失败
    func->eraseFromParent();
    p->ir = NULL;
    return;
}
