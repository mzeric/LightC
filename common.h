#ifndef _LLVM_COMMON_H
#define _LLVM_COMMON_H


#if (LLVM_VERSION >= 33)
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/IRBuilder.h>
#else
#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Support/IRBuilder.h>
#endif

#include <iostream>

using namespace llvm;
typedef Value Fvalue;
typedef std::string Fstring;

extern LLVMContext* llvm_context;

extern "C" void check(const char*str,...);
void A_fatal(const char *str,int e = 1);
#define debug_visit(x) std::cout << "visit " << x << std::endl;
#define printf 
//static std::map<std::string, Value*> NamedValues;

class AST_func;
class AST_integer;
class AST_expr_list;
class AST_var;
class AST_bin;
class AST_assignment;
class AST_call;
class AST_declare;
class AST_local_var;
class AST_proto;
class AST_func;

class Node {
public:
    Node():ir(NULL){}
    class Visitor{
    public:
//        virtual void visit(Node* n) = 0;
        virtual void visit(AST_func* n) = 0;
        virtual void visit(AST_integer* n) = 0;
        virtual void visit(AST_expr_list* n) = 0;
        virtual void visit(AST_var* n) = 0;
        virtual void visit(AST_bin *n) = 0;
        virtual void visit(AST_assignment *n) = 0;
        virtual void visit(AST_call *n) = 0;
        virtual void visit(AST_declare *n) = 0;
        virtual void visit(AST_local_var *n) = 0;
        virtual void visit(AST_proto *n) = 0;

        void access(Node *n){
          if(!n->ir)
            n->accept(this);
        }
    };
    virtual void accept(Visitor* v) = 0;
    virtual Fvalue *code() = 0;
    Fvalue * ir;
    virtual ~Node() {};
};


class AST_expr_list: public Node{
public:
	Node* expr;
	AST_expr_list* next;
	AST_expr_list(Node*e ):
		expr(e){}
	std::vector<AST_expr_list*> lists;
	void append_expr(AST_expr_list*e){
		next = e;
		lists.push_back(e);
	}
	Fvalue * code();

  void accept(Visitor* v){

    v->visit(this);
  }
};
class AST_integer: public Node{
  public:
    
    int var_value;
    
  AST_integer(int val):var_value(val){}
    virtual Fvalue *code();

  void accept(Visitor* v){
    v->visit(this);
  }
};

class AST_var	: public Node{
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
  void accept(Visitor* v){
    v->visit(this);
  }    
};
class AST_bin   : public Node{
  public:
    char op;
    Node *LHS, *RHS;
  AST_bin(char op, Node *lhs, Node *rhs):op(op),LHS(lhs),RHS(rhs){}
    virtual Fvalue *code();
  void accept(Visitor* v){
    v->visit(this);
  }        
};
class AST_assignment:public Node{
public:
	Node *RHS;
	Fstring   lhs;
AST_assignment(Node*v, Node *rhs):RHS(rhs),lhs(((AST_var*)v)->var_id){}
	virtual Fvalue* code();
  void accept(Visitor* v){
    v->visit(this);
  }
};
class AST_call  : public Node{
  public:
    Fstring name;
    std::vector<Node*> args;
  AST_call(const Fstring &func, std::vector<Node*> args):name(func), args(args){}
    virtual Fvalue *code();
  void accept(Visitor* v){
    v->visit(this);
  }
};
class AST_declare :public Node{//所有声明的基类
public:
	Fstring decl_id;
	AST_declare(){}
	~AST_declare(){}
	virtual Fvalue *code(){};
    	virtual void	set_name(Fstring str){
    		decl_id = str;
    	}
	virtual Fstring get_name(){return decl_id;}  

  void accept(Visitor* v){
    v->visit(this);
  }  
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
  void accept(Visitor* v){
    v->visit(this);
  }
};

class AST_local_var: public AST_declare{
public:
	AST_local_var(AST_declare *s,Fvalue *v=NULL);
	Fvalue *code();
  void accept(Visitor* v){
    v->visit(this);
  }
};
class AST_proto : public AST_declare{//声明一个函数
public:
	Fstring name;	//函数名
	std::vector<Fstring> args;	//参数表
	AST_proto(const Fstring &fname,	const std::vector<Fstring> &args):name(fname),args(args){}
	Function *code();
  void accept(Visitor *v){
      v->visit(this);
  }
};
class AST_func: public Node{
public:
	Node  *proto;
	Node  *body;
	Fvalue	  *ret_value;
	AST_func(Node* proto, Node* body): proto(proto),body(body){}
  Fvalue *code(){
    coder(NULL);
  }
	Function *coder(Fvalue *ret_value = NULL);
  void accept(Visitor* v){
    v->visit(this);
  }       
       
};
class CodegenVisitor : public Node::Visitor{
public:
//    void visit(Node *n){}// just pass the compile

    void visit(AST_integer *p){
        std::cout << "visit int" <<std::endl;
        p->ir = ConstantInt::get(*llvm_context,APInt(32,p->var_value));

    }
    void visit(AST_expr_list *p){
          debug_visit("expr_list");
      std::vector<AST_expr_list*>::iterator iter = (p->lists).begin();

      for( ; iter != (p->lists).end(); ++iter){
        if( (*iter)->expr == NULL){
          A_fatal("expr is NULL!\n");
          break;
        }

        if((*iter)->expr->ir == NULL)
            (*iter)->expr->accept(this);
        p->ir = (*iter)->expr->ir;
      

      }
    }
    void visit(AST_var *p);
    void visit(AST_assignment *p);
    void visit(AST_bin *p);
    void visit(AST_call *p);
    void visit(AST_declare *p);
    void visit(AST_local_var *p);
    void visit(AST_proto *p);
    void visit(AST_func *p);
};



















#endif
