//extern "C"
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include "common.h"
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
	extern "C" int yyparse(void);
	static Module *TheModule;
	IRBuilder<> Builder(getGlobalContext());
	typedef std::map<std::string, Value*> SymTable;
	SymTable NamedValues;//函数内命名空间
	std::map<std::string, Value*> NamedUnValues;//函数内命名空间
    
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
Fvalue * AST_expr_list::code(){
	Fvalue *v = NULL;
//	printf("begin expr_list::code(%X)->%X\n",expr,next?next->expr:NULL);
	for( int i= 0;i<lists.size();i++){
		if(lists[i]->expr == NULL){
			A_fatal("expr is NULL!\n");
			break;
		}

		v = lists[i]->expr->code();

	}
		return v;
	}
    Fvalue *AST_integer::code(){
    		printf("ast_integer::code(%d)\n",var_value);
		return ConstantInt::get(*llvm_context,APInt(32,var_value));
        
        
	}
	
	Fvalue *AST_var::code(){

		printf("ast_var::code(%X)\n",this);
		Fvalue *V = NamedValues[var_id];
		return V?V:NULL;

	}
	Fvalue* AST_assignment::code(){
		Fvalue *vc = RHS->code();
		printf("assign :%X\n",vc);
		NamedValues[lhs]=vc;
		
		vc->setName(lhs);
		return vc;	
	};
	
	Fvalue *AST_bin::code(){
		Fvalue *lhs = LHS->code();
		Fvalue *rhs = RHS->code();
		printf("AST_bin:code(op:%c,%X,%X)\n",op,lhs,rhs);

		if(lhs == NULL||rhs == NULL)
			return NULL;
		switch (op){
		case '+':return Builder.CreateAdd(lhs,rhs,"add");
					break;
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
    
Fvalue *AST_call::code(){//触发一个函数调用
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
		return Builder.CreateCall(func,Argsv,"call");
}
/////////////////// AST_declare->AST_local_var
AST_local_var::AST_local_var(AST_declare *s,Fvalue *v ){
		if(v == NULL){
			NamedUnValues[s->decl_id] =  NULL;

		}{
			NamedValues[s->decl_id] = v;

		}
		printf("[AST_local_var::add_v]");
}
Fvalue* AST_local_var::code(){
		printf("ast_local_var::code(%X)\n",this);	
}
///////////////////
Function* AST_proto::code(){
		std::vector<Type*> int_args(args.size(),Type::getInt32Ty(*llvm_context));	//参数类型
		FunctionType * func_type = FunctionType::get(Type::getInt32Ty(*llvm_context),//返回值类型
                                                     int_args,false);
		Function	* func = Function::Create(func_type,Function::ExternalLinkage,name,TheModule);
		if(func){
            printf("proto.ok\n");
        }
            //检查重名
		if(func->getName() != name){
			func->eraseFromParent();
			func = TheModule->getFunction(name);
			if(!func->empty()){
				A_error("重复定义");
				return NULL;
			}
			if(func->arg_size()!=args.size()){
				A_error("重复定义，参数表不同");
			}
		}
        
        
            //保存参数名
		int i =0;
		for(Function::arg_iterator AI = func->arg_begin();i<func->arg_size();AI++,i++){
			AI->setName(args[i]);
			NamedValues[args[i]] = AI;
		}
	//	printf("proto:%x\n",func);
		return func;
}
////////////////////////////////    
    Function* AST_func::coder(Fvalue *ret_value ){
        NamedValues.clear();//清除名字空间(符号表)
        Function* func = (Function*)(proto->code());
        if(func == NULL){
            printf("func.NULL\n");
            return NULL;
        }
printf("begin body code\n");
        BasicBlock *func_block = BasicBlock::Create(*llvm_context,"entry",func);
        Builder.SetInsertPoint(func_block);
        Fvalue *ret_expr=NULL;
        
        if(ret_value){
        
            ret_expr = ret_value;
            }
        else{
            ret_expr=body->code();
            }
        if(ret_expr ){
            Builder.CreateRet(ret_expr);
            verifyFunction(*func);
            //func_block->end();
            return func;
        }
            //定义失败
        func->eraseFromParent();
        return NULL;
    }
/*
	C_EXPORT {
        
		Function* A_FUNC_proto(FunctionType* ft, char* name, Module*module){
                        
			Function *F = Function::Create(ft, Function::ExternalLinkage, name, module);
            
			return F;
            
		}
        
		void A_FUNC_define_begin(Function *){
            
            
            
            
		}

        
        
		void A_EXP_(int a,int b,char*op){
            
			Value *va = ConstantFP::get(getGlobalContext(),APFloat((float)a));
			Builder.CreateFAdd(va,va,"addtmp");

            
		}
        
        
	}
*/
	int main(){
        
		LLVMContext &Context = getGlobalContext();
		llvm_context = &Context;
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
		
        
		TheModule->dump();
		return 0;
        
        
    
}

void CodegenVisitor::visit(AST_var* p){
	debug_visit("ast_var");
	Fvalue *V = NamedValues[p->var_id];
	p->ir = V?V:NULL;
}
void CodegenVisitor::visit(AST_assignment *p){
	debug_visit("AST_assignment");
		if(p->ir)
			return;
		if( !(p->RHS->ir))
			p->RHS->accept(this);
		Fvalue *vc = p->RHS->ir;
		printf("assign :%X\n",vc);
		NamedValues[p->lhs]=vc;
		
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
		case '+':p->ir = Builder.CreateAdd(lhs,rhs,"add");
					break;
		case '-':p->ir = Builder.CreateSub(lhs,rhs,"sub");break;
		case '*':p->ir = Builder.CreateMul(lhs,rhs,"mul");break;
		case '<':
			p->ir = Builder.CreateICmpULT(lhs,rhs,"cmp");
			//return Builder.CreateUITOInt(lhs,
			//Type::getInt32Type(*llvm_context), "bool");
			break;
		default:
			A_error("无效的操作符");

		}
}
void CodegenVisitor::visit(AST_declare *p){
	debug_visit("AST_declare");
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
void CodegenVisitor::visit(AST_local_var *p){
	std::cout << "AST_local_var " << std::endl;
}
void CodegenVisitor::visit(AST_proto *p){
//		debug_visit("proto");
		std::cout << "visit proto" << std::endl;
		std::vector<Type*> int_args(p->args.size(),
					Type::getInt32Ty(*llvm_context));	//参数类型
		FunctionType * func_type = 
			FunctionType::get(Type::getInt32Ty(*llvm_context),//返回值类型
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
		for(Function::arg_iterator AI = func->arg_begin();i<func->arg_size();AI++,i++){
			AI->setName(p->args[i]);
			NamedValues[p->args[i]] = AI;
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
        //func_block->end();
        p->ir = func;

        return;
    }
        //定义失败
    func->eraseFromParent();
    p->ir = NULL;
    return;
}
