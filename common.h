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

class AST_expr{
  public:
    AST_expr(){}
    virtual ~AST_expr(){};
    virtual Fvalue *code()=0;
};
class AST_integer: public AST_expr{
  public:
    
    int var_value;
    
  AST_integer(int val):var_value(val){}
    virtual Fvalue *code();
};

class AST_var	: public AST_expr{
  public:
    Fstring name;
  AST_var(const Fstring &var_name):name(var_name){}
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
    Fstring name;
    std::vector<AST_expr*> args;
  AST_call(const Fstring &func, std::vector<AST_expr*> args):name(func), args(args){}
    virtual Fvalue *code();
};
class AST_declare //所有声明的基类
{
public:
	Fstring id;
	AST_declare(){}
	~AST_declare(){}
	virtual Fvalue *code(){};
    	virtual void	set_name(Fstring str){
    		id = str;
    	}
	virtual Fstring get_name(){return id;}    
};

class AST_args  : public AST_declare{
public:
	std::vector <Fstring> args;
    	AST_args(){}
    	void add_args(std::string s){
    	{
    	using namespace std;
    		cout<<"add_"<<s<<"_";	
	}
	args.push_back(s);
    	}
    	std::vector<Fstring> get_args(){
    		return args;
    	}
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
