#ifndef ANODE_H
#define ANODE_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>



#define DEFTREECODE(sym, name, type, len) sym,
enum anode_code{
	#include "c-tree.def"
};
#undef DEFTREECODE

class anode_node;
class anode_list;
typedef anode_node* anode;
typedef anode_list* anode_seq;

extern anode top_ast_node;
extern anode anode_null;
typedef std::set<anode> Set;

#define HOST_WIDTH_INT  int

extern int anode_code_length(int node);
#define ANODE_CODE(t) (t)->code
extern int anode_code(anode node);
extern int anode_code_class(int code);
extern const char* anode_code_name(int code);

extern anode global_space;
extern anode current_declspaces;
extern anode declspace_stack;

void anode_class_check_failed(const anode node, int cl, const char *file,
                int line, const char* function);
void anode_check_failed(const anode node, int  code, const char*file,
		int line, const char* function);
void anode_operand_check_failed (int idx, int  code, const char *file,
                                int line, const char *function);
#define ANODE_CLASS_CHECK(T, CLASS) __extension__                        \
({  __typeof(T) __t = (T);                                               \
    if (anode_code_class (anode_code(__t)) != (CLASS))                    \
      anode_class_check_failed (__t, (CLASS), __FILE__, __LINE__,        \
                               __FUNCTION__);                           \
    __t; })

#define ANODE_CHECK(T, CODE) __extension__                               \
({  __typeof(T) __t = (T);                                               \
    if (anode_code(__t) != (CODE))                                      \
      anode_check_failed (__t, (CODE), __FILE__, __LINE__, __FUNCTION__); \
    __t; })

#define IS_EXPR_CODE_CLASS(CLASS) (strchr("<12ers", (CLASS)) != 0)
#define EXPR_P(Node) IS_EXPR_CODE_CLASS(anode_code_class(anode_code(Node)))

#define EXPR_CHECK(T) __extension__                                     \
({  anode __t = (T);                                              \
    if (!EXPR_P(__t))                                                   \
            anode_class_check_failed(__t, 'e', __FILE__, __LINE__,      \
                __FUNCTION__);                                          \
    __t;})

#define ANODE_OPERAND_CHECK(T, I) __extension__                         \
(*({ anode_expr *__t = (anode_expr*)(T);                                             \
        const int __i = (I);                                            \
        if (__i < 0 || __i >= anode_code_length(anode_code(T)))         \
            anode_operand_check_failed (__i, anode_code(__t),           \
                __FILE__, __LINE__, __FUNCTION__);                      \
        &__t->operands[__i];}))

#define ANODE_OPERAND_P_CHECK(T, I) __extension__                         \
({ anode_expr *__t = (anode_expr*)(T);                                             \
        const int __i = (I);                                            \
        if (__i < 0 || __i >= anode_code_length(anode_code(T)))         \
            anode_operand_check_failed (__i, anode_code(__t),           \
                __FILE__, __LINE__, __FUNCTION__);                      \
        &__t->operands[__i];})

#define ANODE_OPERAND_CHECK_CODE(T, CODE, I) __extension__              \
(*({ __typeof(T) __t = (T);                                             \
     const int   __i = (I);                                             \
    if (anode_code(__t) != CODE)                                        \
        anode_check_failed(__t, CODE, __FILE__, __LINE__,__FUNCTION__); \
    if (__i < 0 || __i >= anode_code_length(CODE))                      \
        anode_operand_check_failed (__i, (CODE), __FILE__, __LINE__,    \
            __FUNCTION__);                                              \
    &__t->operands[__i];}))

struct basic_block_s;

class anode_node{
public:
        anode           chain;
        unsigned        code;
        anode           type; /* each has a type info */
        unsigned short  stack_index;
        struct basic_block_s *basic_block;

        unsigned side_effects_flag : 1;
        unsigned constant_flag : 1;
        unsigned addressable_flag : 1;
        unsigned volatile_flag : 1;
        unsigned readonly_flag : 1;
        unsigned unsigned_flag : 1;
        unsigned asm_written_flag: 1;
        unsigned unused_0 : 1;

        unsigned used_flag : 1;
        unsigned nothrow_flag : 1;
        unsigned static_flag : 1;
        unsigned public_flag : 1;
        unsigned private_flag : 1;
        unsigned protected_flag : 1;
        unsigned deprecated_flag : 1;
        unsigned unused_1 : 1;

        unsigned lang_flag_0 : 1;
        unsigned lang_flag_1 : 1;
        unsigned lang_flag_2 : 1;
        unsigned lang_flag_3 : 1;
        unsigned lang_flag_4 : 1;
        unsigned lang_flag_5 : 1;
        unsigned lang_flag_6 : 1;
        unsigned unused_2 : 1;

        unsigned int version;
        anode        decl_outer;
public:
	virtual int length(){
		return anode_code_length(this->code);
	}
    anode_node(){
        chain = NULL;
        type  = NULL;
        version = 0;
        stack_index = 0;
        users = new std::set<anode*>;
        
        is_reg = false;
        addr = 0;
        hint = NULL;
    }
	virtual ~anode_node(){
	}
    virtual void add_user(anode *u){}
    virtual void cat(anode p){
        if (this->chain)
            this->chain->cat(p);
        else
            this->chain = p;
    }

    std::set<anode*> *users;/* list */
    Set     def, use;

    bool is_reg;
    int addr;
    anode hint;
};

typedef struct location_s{
        const char *file;
        int line;
}location_t;

class anode_identifier : public anode_node{
public:
	anode_identifier(char * s){
		code = IDENTIFIER_NODE;
		pointer = strdup(s);
	}
	~anode_identifier(){
		printf("free\n");
		free(pointer);
	}
	char *pointer;
};
class anode_int_cst : public anode_node {
public:
	anode_int_cst(size_t cst):int_cst(cst){
		code = INTEGER_CST;
	}
	size_t 	int_cst;
    size_t cst(void){
        return int_cst;
    }
};
class anode_string_cst : public anode_node{
public:
	anode_string_cst(char *str){
		code = STRING_CST;
		pointer = strdup(str);
		length = strlen(str);
	}
	~anode_string_cst(){
		free((void*)pointer);
	}
public:
        int length;
        const char *pointer;
};
class anode_list : public anode_node{
public:
    anode_list():value(NULL),purpose(NULL){
        chain = NULL;
        code = ANODE_LIST;
    }
public:
        anode purpose;
        anode value;
};
/*
    MUST SINGLTON
*/
class anode_type : public anode_node {
public:
    //static type_table[]
public:
	anode_type(int c){
		code = c;
	}

public:
        anode values;
        anode size;
        anode size_unit;
        anode attributes;
        unsigned int uid; 

        unsigned int precision : 9; 
        unsigned int mode : 7; 

        unsigned string_flag : 1; 
        unsigned no_force_blk_flag : 1; 
        unsigned needs_constructing_flag : 1; 
        unsigned transparent_union_flag : 1; 
        unsigned packed_flag : 1; 
        unsigned restrict_flag : 1; 
        unsigned spare : 2; 

        unsigned lang_flag_0 : 1; 
        unsigned lang_flag_1 : 1; 
        unsigned lang_flag_2 : 1; 
        unsigned lang_flag_3 : 1; 
        unsigned lang_flag_4 : 1; 
        unsigned lang_flag_5 : 1; 
        unsigned lang_flag_6 : 1; 
        unsigned user_align  : 1;
        unsigned int align;
        anode pointer_to;
        anode reference_to;
        union anode_type_symtab {
                int address;
                char *pointer;
               // struct die_struct *die;
        } symtab;
        anode name;
        anode minval;
        anode maxval;
        anode next_variant;
        anode main_variant;
        anode binfo;
        anode context;
        int alias_set;
};
//
class anode_decl : public anode_node{
public:
public:
        unsigned uid;
        anode name;


        anode           size;
        unsigned char mode : 8;

        unsigned external_flag : 1;
        unsigned nonlocal_flag : 1;
        unsigned regdecl_flag : 1;
        unsigned inline_flag : 1;
        unsigned bit_field_flag : 1;
        unsigned virtual_flag : 1;
        unsigned ignored_flag : 1;
        unsigned abstract_flag : 1;

        unsigned in_system_header_flag : 1;
        unsigned common_flag : 1;
        unsigned defer_output : 1;
        unsigned transparent_union : 1;
        unsigned static_ctor_flag : 1;
        unsigned static_dtor_flag : 1;
        unsigned artificial_flag : 1;
        unsigned weak_flag : 1;

        unsigned non_addr_const_p : 1;
        unsigned no_instrument_function_entry_exit : 1;
        unsigned comdat_flag : 1;
        unsigned malloc_flag : 1;
        unsigned no_limit_stack : 1;
        unsigned built_in_class : 2;
        unsigned pure_flag : 1;

        unsigned non_addressable : 1;
        unsigned user_align : 1;
        unsigned uninlinable : 1;
        unsigned thread_local_flag : 1;
        unsigned declared_inline_flag : 1;
        unsigned visibility : 2;
        unsigned unused : 1;



        unsigned lang_flag_0 : 1;
        unsigned lang_flag_1 : 1;
        unsigned lang_flag_2 : 1;
        unsigned lang_flag_3 : 1;
        unsigned lang_flag_4 : 1;
        unsigned lang_flag_5 : 1;
        unsigned lang_flag_6 : 1;
        unsigned lang_flag_7 : 1;

        union anode_decl_u1{
        struct anode_decl_u1_a {
                unsigned int align : 24;
                unsigned int off_align : 8;
        } a;
        }u1;

        anode size_unit;
        anode arguments;
        anode result;
        anode initial;
        anode abstract_origin;
        anode assembler_name;
        anode section_name;
        anode attributes;

        anode saved_func; /* for FUNCTION_DECL , is DECL_SAVED_TREE */

        int pointer_alias_set;

        bool is_tmp;

        anode_decl():is_tmp(false){}

};


class anode_expr : public anode_node {
public:
        anode_expr(int c){
            int len = anode_code_length(c);
            operands = (anode*)malloc(sizeof(anode) * len);
            memset(operands, 0, sizeof(anode) * len);

            code = c;
            //compound_decl_list = NULL;
        }
        ~anode_expr(){
            free(operands);
        }
public:
        int comp;
        anode *operands;
        anode compound_decl_list_inter;
        anode compound_decl_list_outer;

};
class anode_block : public anode_node {
public:
        unsigned handler_block_flag : 1;
        unsigned abstract_flag : 1;
        unsigned block_num : 30;

        anode vars;
        anode subblocks;
        anode supercontext;
        anode abstract_origin;
        anode fragment_origin;
        anode fragment_chain;
};
typedef struct edge_s *edge;
class  use_t{
public:
    use_t():expr(NULL),op_index(-1){}
    use_t(anode u, int op, bool isp):expr(u), op_index(op), is_phi(isp){}
    anode expr; /* expr or phi-expr(anode_phi*) */
    int  op_index;/* index of the expr's operand */
    anode *tmp; /* tmp for normal expr */
    bool is_phi;
    struct basic_block_s *pred_block; /* block of pred for phi */
    struct basic_block_s *block;
};


class anode_ssa_name : public anode_node {
public:
        anode_ssa_name(){code = IR_SSA_NAME;}
        anode_ssa_name(anode id){
            var     = id;
            code    = IR_SSA_NAME;
            phi     = NULL;
            def_stmt = NULL;

        }
        void set_phi(anode p){
            phi = p;
        }

        bool is_phi(){return phi;}
        void add_user(anode u, int op, bool isp){ulist.push_back(use_t(u, op, isp));}
        void add_user(use_t t){ulist.push_back(t);}

        anode var; /* NULL means phi node */
        anode def_stmt;
        std::list<use_t> ulist;/* 所有用到这个stmt的 stmt*/
        anode phi;

};
struct ssa_name_comparator{
    bool operator()(const anode &l, const anode &r)const{
        anode_ssa_name *a = (anode_ssa_name*)l;
        anode_ssa_name *b = (anode_ssa_name*)r;

        if (a->var == b->var){
            if (a->version == b->version)
                return false;
            return a->version < b->version;
        }else
            return a->var < b->var;
    }
};
typedef std::map<anode_ssa_name*, anode, ssa_name_comparator> ssa_table_t;

#define ANODE_(Node, Type) __extension__                \
({  ((Type*)(Node)); })
#define ANODE_CHAIN(t)       ((t)->chain)
#define ANODE_LIST_CHECK
#define ANODE_VALUE(Node)       ANODE_(ANODE_CHECK((Node), ANODE_LIST),anode_list)->value
#define ANODE_PURPOSE(Node)       ANODE_(ANODE_CHECK((Node), ANODE_LIST),anode_list)->purpose
/* Every node has one type */
#define ANODE_TYPE(Node)        ((Node)->type)
#define TYPE_SIZE(t)           ANODE_(ANODE_CLASS_CHECK((t), 't'), anode_type)->size
#define ANODE_DECL_TYPE(Node)   (ANODE_CLASS_CHECK((Node),'d')->type)
#define ANODE_DECL_NAME(Node)   (ANODE_CLASS_CHECK((Node),'d')->name)

#define IDENTIFIER_POINTER(Node) ANODE_CHECK(ANODE_(Node, anode_identifier), IDENTIFIER_NODE)->pointer

#define ANODE_OPERAND(NODE, I)      ANODE_OPERAND_CHECK(NODE, I)
#define ANODE_OPERAND_P(NODE, I)    ANODE_OPERAND_P_CHECK(NODE, I)
#define IF_STMT_CHECK(NODE)     ANODE_CHECK(NODE, IF_STMT)
#define WHILE_STMT_CHECK(NODE)     ANODE_CHECK(NODE, WHILE_STMT)

#define IF_COND(NODE)           ANODE_OPERAND (IF_STMT_CHECK (NODE), 0)
#define WHILE_COND(NODE)           ANODE_OPERAND (WHILE_STMT_CHECK (NODE), 0)
#define WHILE_CLAUSE(NODE)           ANODE_OPERAND (WHILE_STMT_CHECK (NODE), 1)

#define THEN_CLAUSE(NODE)       ANODE_OPERAND (IF_STMT_CHECK (NODE), 1)
#define ELSE_CLAUSE(NODE)       ANODE_OPERAND (IF_STMT_CHECK (NODE), 2)

#define COMPOUND_STMT_CHECK(t)  ANODE_CHECK(t, COMPOUND_STMT)
#define COMPOUND_BODY(t)        ANODE_OPERAND(COMPOUND_STMT_CHECK(t), 0)
#define COMPOUND_DS_INTER(t)    ANODE_CHECK(t, COMPOUND_STMT)->compound_decl_list_inter
#define COMPOUND_DS_OUTER(t)    ANODE_CHECK(t, COMPOUND_STMT)->compound_decl_list_outer

anode decl_name(anode node);

inline anode decl_initial(anode node){
    anode_decl *t = (anode_decl*)node;
    return ANODE_CLASS_CHECK(t, 'd')->initial;
}

anode build_list(anode root, anode node);
anode chain_cat(anode a, anode b);
anode chain_last(anode a);
anode build_var_decl(anode specifier, anode declarator);
anode anode_cons(anode purpose, anode node, anode chain);
void c_parse_init(void);
anode build_stmt(int code, ...);
anode build_ir_len(int code, int len, ...);
void    push_namespace (void);
void    pop_namespace (void);
void    push_decl(anode decl);

#define FOR_EACH_DECL(T)        \
    for (anode T = current_declspaces; T; T = ANODE_CHAIN(T))

int     is_branch(anode t);

/*
    about Basic Block Construction
*/
struct edge_s;
enum edge_flag{
    EDGE_PASSTHOUGH,
    EDGE_NO_MERGE,
    EDGE_TRUE,
    EDGE_FALSE,
};
#include "dfa.h"
class anode_label: public anode_node{
public:
    anode_label(const char *name){
        {
            char *buf =NULL;
            if(name)
                asprintf(&buf, "%s%d", name,++num);
            else
                asprintf(&buf, "label_%d", ++num);
            
            label = strdup(buf);
            free(buf);
        }
        code = IR_LABEL;
        num++;
    }
    char *label;
    static int num;
};
class anode_ins: public anode_expr{
public:
    anode_ins(int c):anode_expr(c){

    }
};
typedef std::map<anode, edge> phi_edge_t;
typedef std::map<anode, live_set_t> stmt_live_t;
typedef struct basic_block_s{
        unsigned                index; /* used for goto expr */
        struct basic_block_s    *prev, *next; /* the chain */
        anode                   list;		/* use list to link stmt inside bb instead modify The AST */
        anode                   outer_loop; /* outer loop for break */
        anode                   entry, exit; /* first & last node of the block */
        anode                   lower_ir;
        struct edge_s           *pred, *succ; /* edges in / out of the block */
        anode                   decl_context; /* store the current_declspaces */
        anode                   decl_current;
        char                    *comment;
        ssa_table_t             *ssa_table;
        int                     status;

        anode                   phi;   /* all phi instructs linked by ->chain */
        phi_edge_t              *phi_edge;
        struct live_ness_t      *live;
        stmt_live_t             *stmt_live;

        std::list<anode_ins*>        *ins;

}basic_block_t,bb_t, *bb;

#define BB_STMT_BEGIN(b) (b)->entry
#define BB_STMT_END(b) (b)->exit;
#define BB_STMT_NEXT(s) ANODE_CHAIN((s))

#define FOR_EACH_STMT_OF_BB(s, b)  for (anode t = BB_STMT_BEGIN(b); t,ANODE_VALUE(t); t = BB_STMT_NEXT((s)))

#define _FOR_EACH_BB(b, start, end) for (basic_block_t *(b) = (start); (b) != (end); (b) = (b)->next)
#define FOR_EACH_BB(b, start) _FOR_EACH_BB(b, start, EXIT_BLOCK_PTR)

typedef struct edge_s{

        struct edge_s           *pred_next, *succ_next;
        struct basic_block_s    *src, *dst;
        int                     flag;

}edge_t, *edge;

class anode_phi : public anode_node {
public:
        anode_phi(basic_block_t *b):block(b){

            this->chain = b->phi;
            b->phi = (anode)this;
            code = IR_PHI;

        }
        

        void replace_by(anode new_v){
            std::set<anode*>::iterator iter;
            for (iter = users->begin(); iter != users->end(); ++iter){
                    **iter = new_v;
            }
        }
        void add_user(anode* u){
            users->insert(u);
            
        }
public:

        basic_block_t   *block;
        anode_ssa_name  *def_name;
        vector<anode>    targets;/* anode_list for NULL phi operand before rename */

};
inline void add_phi_user(anode phi, anode* u){
        if(anode_code(phi) != IR_PHI)
            return;
        ((anode_phi*)phi)->add_user(u);
}
extern struct basic_block_s entry_exit_blocks[2];

#define ENTRY_BLOCK_PTR (&entry_exit_blocks[0])
#define EXIT_BLOCK_PTR (&entry_exit_blocks[1])

edge make_edge(bb src, bb dst, int flag);
edge clone_edge(edge e);
edge get_edge(bb src, bb dst);
edge replace_edge(bb_t *src, bb_t *dst, edge n);

void remove_edge(edge e);
anode lookup_name_current_bb(const char *name);

void push_ssa_decl(anode id, anode value);
anode chain_last(anode l);

extern struct basic_block_s *current_bb;
void dump_block_list(anode l, int le);
void print_decl(anode l);
anode build_func_decl(anode declar, anode params);
anode build_decl(enum anode_code, anode speci, anode declar);
anode build_parm_decl(anode a, anode b);
anode build_addr_expr(anode expr);



basic_block_t *build_func_cfg(anode t);
void simplify_bb(basic_block_t *b);
void dump_bb(basic_block_t* t);
void dump_stmt(anode s);
void dump_edges(basic_block_t *e);
void fill_bb(basic_block_t *b);
void build_ssa(basic_block_t *b);
inline basic_block_t* last_bb(basic_block_t *b){
    basic_block_t* t = b;
    while(t->next)
        t = t->next;
    return t;

}
inline bb get_bb(bb b, int index){
    for(bb t = b; t; t = t->next){
        if (t->index == index)
            return t;
    }
    return NULL;
}

extern anode get_def(anode id, bb b);
extern anode read_variable(anode id, bb b);

/*
inline anode get_use(use_t *u){
    if(!u || !u->expr)
        return NULL;

    if(anode_code(u->expr) == IR_PHI)
        return ((anode_phi*)u->expr)->targets[u->op_index];
    if(EXPR_P(u->expr)){
        return ANODE_OPERAND(u->expr, u->op_index);
    }
    char cc = anode_code_class(anode_code(u->expr));
    if( cc == 'c' || cc == 'd'){
        return u->expr;
    }

    return NULL;
}
inline void rewrite_use(use_t *u, anode value){
    if(!u || !u->expr)
        return;
    if(anode_code(u->expr) == IR_PHI){

    }
    if(EXPR_P(u->expr)){
        ANODE_OPERAND(u->expr, u->op_index) = value;
        return;
    }

}
*/

#endif
