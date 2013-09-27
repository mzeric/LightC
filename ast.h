#ifndef _H_AST_
#define _H_AST_

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/TypeBuilder.h>

#include <llvm/IRReader/IRReader.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Support/Allocator.h>
#include <llvm/ADT/PointerIntPair.h>
#include <llvm/ADT/PointerUnion.h>



#include <iostream>
#include <string>
#include <list>
#include "type.h"




typedef llvm::Value Fvalue;
typedef std::string Fstring;

#define debug_visit(x) std::cout << "visit " << x << std::endl;
extern llvm::LLVMContext* llvm_context;
extern struct llvm::Module *TheModule;
extern llvm::IRBuilder<> Builder;



class Node;
class AST_func;
class AST_expr_list;
class AST_integer;
class AST_var;
class AST_literal;
class AST_bin;
class AST_assignment;
class AST_call;
class AST_args;
class AST_decl_var;
class Declaration;
class AST_proto;
class AST_func;

class AST_storage;
class NodeType;

class SymbolTable;
class ASTContextImpl;
class ASTContext;

extern ASTContextImpl ContextImpl;
extern ASTContext *current_ast_context;

ASTContext* getCurrentContext();
void dumpSymbolTable(SymbolTable *st);
void dumpAllContext();
void dumpAllContext(ASTContext *c);
class SymbolInfo{
  public:
    SymbolInfo():typeName(NULL),
      value(NULL),
      node(NULL)
      {}
    SymbolInfo(llvm::Value *v):value(v){}
    ASTType     *typeName;
    llvm::Value *value;
    Node        *node;
  };
class SymbolTable{
public:
  
  SymbolTable(){}

  std::map<std::string, SymbolInfo> info;
  typedef std::map<std::string, SymbolInfo>::iterator Iterator;
};
  
class ASTContextImpl{
public:
  std::list<SymbolTable*> list; 
  typedef std::list<SymbolTable*>::reverse_iterator Iterator;
  llvm::BumpPtrAllocator  BumpAlloc;
   
};

class ASTContext{

public:
  ASTContext():sym_table(NULL),impl(&ContextImpl){}
  ASTContext(ASTContextImpl *r):impl(r){}
  typedef ASTContextImpl::Iterator Iterator;
  void* Allocate(unsigned size, unsigned align = 8){
    return impl->BumpAlloc.Allocate(size, align);
  }
  void Push(){
    sym_table = new SymbolTable();
    impl->list.push_back(sym_table);
  }
  void Push(ASTContext *c){
    assert(c != NULL && "Push empty context");
    impl->list.push_back(c->sym_table);
  }
  void Push(Fstring name, SymbolInfo si){
    SymbolTable *st = Current();
    st->info[name] = si;
  }
  void Pop(){
    impl->list.pop_back();
  }
  SymbolTable* Current(){
    return impl->list.back();
  }
  SymbolTable* Local(){
    return sym_table;
  }
  SymbolInfo* Lookup(Fstring str);

  bool hasSymbolTable(){
    return sym_table != NULL;
  }
  Iterator begin(){
    return impl->list.rbegin();
  }
  Iterator end(){
    return impl->list.rend();
  }

  ASTType* GetType(std::string name){
      ListType::reverse_iterator iter = impl->list.rbegin();
      for(; iter != impl->list.rend(); ++iter ){
        if( (*iter)->info[name].typeName){

        }
      }
  }
private:
  SymbolTable *sym_table;
  typedef std::list<SymbolTable*> ListType;
  ASTContextImpl *impl;  
};
//extern ASTContext* GetCurrentContext();


class Node {
public:
    Node():ir(NULL),context(current_ast_context){}
    Node(Fvalue* r):ir(r),context(current_ast_context){}
    class Visitor{
    public:
//        virtual void visit(Node* n) = 0;

        virtual void visit(AST_integer* n)    = 0;
        virtual void visit(AST_var* n)        = 0;
        virtual void visit(AST_literal* n)    = 0;
        virtual void visit(AST_expr_list* n)  = 0;
        virtual void visit(AST_bin *n)        = 0;
        virtual void visit(AST_assignment *n) = 0;
        virtual void visit(AST_args *n)       = 0;
        virtual void visit(AST_call *n)       = 0;
     //   virtual void visit(AST_decl *n)       = 0;
        virtual void visit(AST_decl_var *n)  = 0;
        virtual void visit(Declaration *n)    = 0;
        virtual void visit(AST_func* n)       = 0;
        virtual void visit(AST_proto *n)      = 0;

        virtual ~Visitor() {};

    };
    virtual void accept(Visitor* v) = 0;
    virtual ~Node() {};
public:
    llvm::Value *ir; // 为了使用llmv::IR
    llvm::Type  *ir_type;  // 配合ir生产IR代码
    ASTType     *type;
    ASTContext  *context;

};
/*!
  
  var integer literal, all three kind are Primary-Type expressions
  usualy appear at right of '=' as r-value
  aka postfix-expression
*/
class AST_integer: public Node{
  public:
    
    int var_value;
    
  AST_integer(int val):var_value(val){
    //Fixme: *llvm_context;
    //虽然可以直接使用llvm的type system，但为了尝试另一种方法
    ir_type = llvm::Type::getInt32Ty(llvm::getGlobalContext());
    ir = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(32,var_value));
  }

  void accept(Visitor* v){
    v->visit(this);
  }
};


class AST_literal : public Node{
public:
  Fstring id;
  AST_literal(Fstring *str):id(*str){}

  void accept(Visitor *v){
    v->visit(this);
  }
};

class AST_decl :public Node{
public:
    Fstring  decl_id;
    ASTType *decl_type;
    Node    *decl_node;
    llvm::Value  *init_value;

    AST_decl(){}
    ~AST_decl(){}

    //
    virtual void    set_name(Fstring str){decl_id = str;}
    virtual Fstring get_name(){return decl_id;}  

    void accept(Visitor* v){
      assert(false && "What Fuck Are U Thinking ...");
    }


};
class AST_declarator :public AST_decl{//所有声明的基类
public:

    AST_declarator(){}
  
};
class AST_var   : public AST_declarator{
public:

  AST_var(std::string var_name){
    decl_id = var_name;
    decl_type = NULL;
  }
  void accept(Visitor* v){v->visit(this);}

};
class AST_args  : public AST_decl{
public:
    typedef std::pair<Fstring, SymbolInfo> T;
    std::vector <T> args;
        AST_args(){}
    void add_args(std::string s, SymbolInfo si){
      args.push_back(T(s, si));
    }
    std::vector<T> get_args(){
      return args;
    }
  void accept(Visitor* v){v->visit(this);}

};

class AST_decl_var: public AST_declarator{
public:
    AST_decl_var(AST_decl *s, Node *v=NULL);

    void accept(Visitor* v){
        v->visit(this);
    }
};

class Declaration: public AST_decl{
public:
    AST_declarator  *declarator;
    ASTType         *specifier;
    Declaration(ASTType* s, AST_declarator *d):
      declarator(d),specifier(s){

        // Just creata a SymbolTable for declaration 
        // while in proto-decl the out-level, 
        //    the new context is already created by proto
        //
        SymbolTable* st = context->Current();

        st->info[d->decl_id].typeName = specifier;
        if (d->init_value)
          st->info[d->decl_id].value = d->init_value;
        if (d->decl_node)
          st->info[d->decl_id].node  = d->decl_node;

        dumpAllContext();
    }

    void accept(Visitor *v){v->visit(this);}
    SymbolInfo getInfo(){
        return context->Current()->info[declarator->decl_id];
    }

};
class AST_proto : public AST_declarator{//声明一个函数
public:
    Fstring name;   //函数名
    std::vector<AST_args::T> args;  //参数表

    AST_proto(const Fstring &fname, const std::vector<AST_args::T> &args):name(fname),args(args){}
  void accept(Visitor *v){
      v->visit(this);
  }
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

  void accept(Visitor* v){

    v->visit(this);
  }
};


class AST_bin   : public Node{
  public:
    char op;
    Node *LHS, *RHS;
  AST_bin(char op, Node *lhs, Node *rhs):op(op),LHS(lhs),RHS(rhs){}
  void accept(Visitor* v){
    v->visit(this);
  }        
};
class AST_assignment:public Node{
public:
    Node *RHS;
    Fstring   lhs;
    AST_assignment(Node*v, Node *rhs):RHS(rhs),lhs(((AST_var*)v)->decl_id){}
  void accept(Visitor* v){
    v->visit(this);
  }
};
class AST_call  : public Node{
  public:
    Fstring name;
    std::vector<Node*> args;
  AST_call(const Fstring &func, std::vector<Node*> args):name(func), args(args){}
  void accept(Visitor* v){
    v->visit(this);
  }
};
class AST_func: public Node{
public:
    Node  *proto;
    Node  *body;
    Fvalue    *ret_value;
    AST_func(Node* proto, Node* body): proto(proto),body(body){}
  void accept(Visitor* v){
    v->visit(this);
  }       
       
};

class CodegenVisitor : public Node::Visitor{
public:
//    void visit(Node *n){}// just pass the compile
    ASTContextImpl *CodeGenContextImpl;
    ASTContext *context;

    CodegenVisitor():CodeGenContextImpl(new ASTContextImpl()){
      context = new ASTContext(CodeGenContextImpl);

    }
    ~CodegenVisitor(){
      delete CodeGenContextImpl;
      delete context;
    }

    void visit(AST_expr_list *p){
          debug_visit("expr_list");
      std::vector<AST_expr_list*>::iterator iter = (p->lists).begin();

      assert(p->expr != NULL);
      p->expr->accept(this);
      for( ; iter != (p->lists).end(); ++iter){
        assert((*iter)->expr != NULL);


        if((*iter)->expr->ir == NULL){

            (*iter)->expr->accept(this);

        }
        p->ir = (*iter)->expr->ir;
      
      }
    }
    void visit(AST_var *p);
    void visit(AST_integer *p);
    void visit(AST_literal *p);
    void visit(AST_assignment *p);
    void visit(AST_bin *p);
    void visit(AST_args *p);
    void visit(AST_call *p);
//    void visit(AST_decl *p);
    void visit(AST_decl_var *p);
    void visit(Declaration* p);
    void visit(AST_proto *p);
    void visit(AST_func *p);


};
class DumpASTVisitor: public Node::Visitor{

};

#endif