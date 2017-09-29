#ifndef __H_AST__
#define __H_AST__
#include <inttypes.h>
enum node_type{
        NT_NONE= 0, NT_POINTER, NT_FUNCTION, NT_DECLARATOR, NT_SPECIFIER,
        NT_LITERAL_ID,
};

struct node{
        int kind;
        int opator;
  /*      struct list_head link; */
};

/******************************
        Statement Node
*******************************/
extern struct specifier_node*  declaration_specifier();
extern struct decl_node *declaration();
extern int is_specifier();

/******************************
        Declaration Node
*******************************/
struct specifier_node {
        struct node super;
        int type;
};
struct declarator_node {
        struct node super;
        struct declarator_node *_declar;
        char *id;

};
struct pointer_node {
        struct declarator_node super;
        struct node qual_type;
};
struct decl_node{
        struct node     super;
        struct specifier_node   *spec;
        struct list_head        init_list;
};
struct initializer_node {

};
struct init_decl_node {
        struct declarator_node  *declar;
        struct initializer_node *init;
        struct list_head        list;
};

struct parameter_decl_node{
        struct specifier_node   *spec;
        struct declarator_node  *declar;
        struct list_head        list;
};
struct proto_declarator_node{
        struct declarator_node  super;
        struct parameter_decl_node   *param;
};
struct function_node{
        struct node super;
        struct specifier_node           *spec;
        struct declarator_node          *declar;
        struct proto_declarator_node    *real_declar;
        struct compound_statement_node           *stmt;
};


#define KIND(x)    ((struct node*)(x))->kind
#define DECLAR(x)  ((struct declarator_node*)(x))->_declar
#define NEW_NODE(type) (type*)malloc(sizeof(type))

inline struct node *AST_NODE(int kind, int value)
{
        return NEW_NODE(struct node);
}

static inline struct decl_node *AST_DECL_NODE()
{
        struct decl_node *ptr = (struct decl_node*)malloc(sizeof(struct decl_node));
        INIT_LIST_HEAD(&(ptr->init_list));
        return ptr;
}

static inline struct specifier_node *AST_SPECIFIER_NODE(int kind, int value)
{       
        struct specifier_node* spec = 
                (struct specifier_node*) malloc(sizeof(struct specifier_node));
        spec->type = value;
        return spec;
        /* data */
}

#endif
