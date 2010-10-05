#ifndef _LLVM_COMMON_H
#define _LLVM_COMMON_H

#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"
#include <iostream>

using namespace llvm;
typedef Value Fvalue;
typedef std::string Fstring;

extern "C" void check(const char*str,...);

#define printf
//static std::map<std::string, Value*> NamedValues;

class AST_expr{
  public:
  //Fstring expr_id;
    AST_expr(){}
    virtual ~AST_expr(){};
    virtual Fvalue *code()=0;
};
class AST_expr_list:public AST_expr{
public:
	AST_expr* expr;
	AST_expr_list* next;
	AST_expr_list(AST_expr*e ):
		expr(e){}
	std::vector<AST_expr_list*> lists;
	void append_expr(AST_expr_list*e){
		next = e;
		lists.push_back(e);
	}
	Fvalue * code();

};
class AST_integer: public AST_expr{
  public:
    
    int var_value;
    
  AST_integer(int val):var_value(val){}
    virtual Fvalue *code();
};

class AST_var	: public AST_expr{
  public:
    Fstring var_id;
  AST_var(std::string var_name){
  	var_id = var_name;
#ifdef LVC_DEBUG
    	{
    	using namespace std;
    		cout<<"var_add_"<<var_name<<"_";	
	}
#endif
  }
    virtual Fvalue *code();
    
};
class AST_bin   : public AST_expr{
  public:
    char op;
    AST_expr *LHS, *RHS;
  AST_bin(char op, AST_expr *lhs, AST_expr* rhs):op(op),LHS(lhs),RHS(rhs){}
    virtual Fvalue *code();
        
};
class AST_assignment:public AST_expr{
public:
	AST_expr *RHS;
	Fstring   lhs;
AST_assignment(AST_var*v,AST_expr *rhs):RHS(rhs),lhs(v->var_id){}
	virtual Fvalue* code();

};
class AST_call  : public AST_expr{
  public:
    Fstring name;
    std::vector<AST_expr*> args;
  AST_call(const Fstring &func, std::vector<AST_expr*> args):name(func), args(args){}
    virtual Fvalue *code();
};
class AST_declare :public AST_expr{//所有声明的基类
public:
	Fstring decl_id;
	AST_declare(){}
	~AST_declare(){}
	virtual Fvalue *code(){};
    	virtual void	set_name(Fstring str){
    		decl_id = str;
    	}
	virtual Fstring get_name(){return decl_id;}    
};

class AST_args  : public AST_declare{
public:
	std::vector <Fstring> args;
    	AST_args(){}
    	void add_args(std::string s){
#ifdef LVC_DEBUG
    	{
    	using namespace std;
    		cout<<"add_"<<s<<"_";	
	}
#endif
	args.push_back(s);
    	}
    	std::vector<Fstring> get_args(){
    		return args;
    	}
};

class AST_local_var: public AST_declare{
public:
	AST_local_var(AST_declare *s,Fvalue *v=NULL);
	Fvalue *code();

};
class AST_proto : public AST_declare{//声明一个函数
public:
	Fstring name;	//函数名
	std::vector<Fstring> args;	//参数表
	AST_proto(const Fstring &fname,	const std::vector<Fstring> &args):name(fname),args(args){}
	Function *code();
};
class AST_func{
public:
	AST_proto *proto;
	AST_expr  *body;
	Fvalue	  *ret_value;
	AST_func(AST_proto* proto, AST_expr* body): proto(proto),body(body){}
	Function *code(Fvalue *ret_value = NULL);
       
       
};

























#endif
