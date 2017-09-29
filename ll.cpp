#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "anode.h"
#include "lexer.h"
#include "utils.h"
#include "expr.h"
#include "ast.h"
#include "tree.h"


static int look_ahead = -1;
typedef enum c_id_kind {
    C_ID_ID, /* normal id */
    C_ID_TYPENAME, /*typedef */
    C_ID_NONE
}c_id_kind;
struct c_token{
    int     type;
    int     id_kind;
    anode   value;
};
typedef struct c_parser {
    c_token     tokens[2];
    short       token_avail;
}c_parser;
static c_parser *the_parser;

void one_token_from_lex(c_parser *parser, c_token *token){
    token->type = yylex();
    token->id_kind = C_ID_NONE;

    switch(token->type){
        case IDENTIFIER:

        break;
    }
}

int next(){
        return (look_ahead = yylex());
}

int current(){
        return look_ahead;
}

int peek() {
        if(look_ahead >= 0){
                return look_ahead;
        }
        return next();
}
/* depred
int match(int c) {
        if(look_ahead >= 0){
                int a = look_ahead;
                look_ahead = -1;
                return a;
        }
        return next();
}
*/
/**

 */

static struct node *external_declaration();
int translation_unit(){
        struct node *list_unit;
        while(next()){

                list_unit = external_declaration();
                printf("get one grammar\n");
        }

        return 0;
}
/**
        declaration or func-defination
        | declar-specifier ;
        | declar-specifier declarator|declarator = xx, ... ;
        
          declar-specifier declarator decl-list compound_statement
        |

 */
enum declarator_category 
        {DC_ABSTRACT=0x1, DC_CONCRETE=0x2, DC_EITHER=0x3};


static struct decl_node* declaratoion_or_funcdefinition();
struct specifier_node*  declaration_specifier();

static struct declarator_node *declarator(int tc);
static struct init_decl_node *init_declarator();

static struct proto_declarator_node *is_prototype(struct decl_node *decl);

static struct declarator_node *direct_declarator(int tc);
static struct parameter_decl_node *parameter_type_list();
static struct parameter_decl_node *parameter_list();
static struct parameter_decl_node *parameter_declaration();
/**
    will try to Parse 


 */
static struct declarator_node *get_decl_declar(struct decl_node *decl) {
        struct list_head *head = &decl->init_list;
        return list_entry(head->next, struct init_decl_node, list)->declar;
}

static struct node *external_declaration() {
        struct decl_node *df;
        struct proto_declarator_node *ptr;
       
        RESET_PRINT_LEVEL;
        df = declaratoion_or_funcdefinition();
        ptr = is_prototype(df);
        if (ptr) {
                printf("get func proto declar [0x%X]\n", (unsigned int)(uintptr_t)ptr);
                if (current() == ';') { /* function declaration */
                        next();
                        RETURN((struct node*)df);
                }

                /* here function definition */
                assert(current() == '{');
                struct function_node * fptr = NEW_NODE(struct function_node);
                fptr->spec = df->spec;
                fptr->declar = get_decl_declar(df);
                fptr->real_declar = ptr;

                if (ptr->param) {
                        struct list_head *pos, *head;
                        head = ptr->param->list.prev;
                        list_for_each(pos, head){

                        }
                }


                fptr->stmt = compound_statement();

                RETURN((struct node*)fptr);
        

        }else {

                printf("current :%c\n", current());
                assert(current() == ';');
        }
        
        RETURN((struct node*)df);
}
/**
    @return the innermost function-declarator
        the outer is return type , innermost is the real prototype
        skip the declarator that part of function specifier

        int (*fool(char*))() --> fool(char*) is returned otherwise NULL

 */
static struct proto_declarator_node *is_prototype(struct decl_node *decl) {
        struct init_decl_node       *init_ptr;
        struct declarator_node      *declar;
        
        if (!decl)
                return NULL;

        /* prototype never has ',' */
        if (list_length(&decl->init_list) != 1)
                return NULL;
        
        declar = get_decl_declar(decl);
        
        /* can't be a function while missing declarator */
        if (!declar->_declar)
                RETURN(NULL);
        
        while(declar && 
            (KIND(declar) != NT_FUNCTION || KIND(DECLAR(declar)) != NT_LITERAL_ID)){
                declar = declar->_declar;
                //printf("is_prototype:\t%d,%d, %X\n", KIND(declar), KIND(declar->_declar),declar);
        }
        printf("prototype is:\t[%s],%d\n", DECLAR(declar)->id, KIND(declar->_declar));
        
        if (!declar)
                RETURN(NULL);
        if (KIND(declar) == NT_FUNCTION) {
                if (declar->_declar)
                        RETURN( (struct proto_declarator_node*)(declar) );
        }
        
        RETURN(NULL);
}

/**
        everyting before ';'' of  one declaration
 */
static struct decl_node *declaratoion_or_funcdefinition() {
        struct specifier_node *spec;
        struct decl_node *decl;

        decl = AST_DECL_NODE();

        decl->spec = declaration_specifier();

        if(current() == ';')
                return decl;

        struct init_decl_node* tmp = init_declarator();
        assert(tmp && "init_declarator error");
        list_add(&(decl->init_list), &(tmp->list));

        while(current() == ','){
                next();
                init_declarator();
        }
        RETURN(decl);
}
int is_storage_specifier() {
        RETURN(TYPEDEF <= current() && current() <= REGISTER);
}
int is_type_specifier() {
        RETURN(CHAR <= current() && current() <= ENUM);
}
int is_type_qualifier() {
        RETURN(CONSTANT <= current() && current() <= VOLATILE);
}
int is_function_specifier() {
        RETURN(current() == INLINE);
}
int is_specifier() {
        RETURN(is_storage_specifier() || is_type_specifier()
                || is_type_qualifier() || is_function_specifier());
}
struct specifier_node *declaration_specifier() {
        int t = current();
        if (is_specifier()){
                next();
                return AST_SPECIFIER_NODE(NT_SPECIFIER, t);
        }
        assert(!"not valid type-specifier");
        RETURN(NULL);
}
/**
    declaratiion before ','
 */
static struct init_decl_node *init_declarator() {
        struct init_decl_node *ptr;
        struct declarator_node *dptr;

        ptr = (struct init_decl_node*)malloc(sizeof(struct init_decl_node));
        ptr->declar = declarator(DC_CONCRETE);
        if(current() == ASSIGN)
            assert(!"not support init decl right now");

        return ptr;
}
/**



 */


static struct declarator_node *declarator(int allow_abstract) {
        struct declarator_node *ptr;

        if (current() == '*'){
                next();
                struct pointer_node *pptr = NEW_NODE(struct pointer_node);
                pptr->super._declar = direct_declarator(allow_abstract);
                KIND(pptr) = NT_POINTER;
                ptr = (struct declarator_node*)pptr;

               // printf("get pointer of kind:%d\n", KIND(DECLAR(pptr)));
                //assert(!"pointer declar not support right now");
        }else{

                ptr = direct_declarator(allow_abstract);
                printf("get node's kind:%d, %d\n", KIND(ptr), NT_FUNCTION);

        }
        RETURN(ptr);
}
/**
        id
        (id)
        ... [] array declaration
        ... () func declarator

 */


static struct declarator_node* direct_declarator(int allow_abstract) {
        struct declarator_node *dptr;
        /*  bury the seeds */
        /* mark the identifier or abstract with literal_id flag */

        if (current() == IDENTIFIER) {
                assert(allow_abstract != DC_ABSTRACT);
                PRINT_LEVEL;
                printf("(IDENTIFIER %s)\n", (char*)token_value);
                dptr = NEW_NODE(struct declarator_node);
                dptr->id = strdup((char*)token_value);
                KIND(dptr) = NT_LITERAL_ID;

                next();

        }else if (current() == '(') {
                /* (*func_point_declaraton) depends on allow_abstract */
                next();
                dptr = declarator(allow_abstract);
                assert(current() == ')');

                next();

        }else {
                assert (allow_abstract != DC_CONCRETE);
                dptr = NEW_NODE(struct declarator_node);
                KIND(dptr) = NT_LITERAL_ID;
                dptr->id = NULL;

        }

        while(1) {
                if (current() == '('){
                        printf("make proto_declar\n");
                        struct proto_declarator_node *pdptr =
                            NEW_NODE(struct proto_declarator_node);
                        DECLAR(pdptr) = dptr;
                        KIND(pdptr) = NT_FUNCTION;
                        /* function declarator */
                        next();
                        if (current() == ')'){
                                pdptr->param = NULL;
                                printf("empty parameter_list func decl\n");
                        }else{
                                pdptr->param = parameter_type_list();

                                assert(current() == ')');

                        }
                        /*  inside proto is the real proto, outer is return type */
                        dptr = (struct declarator_node*)pdptr;
                        next();
                }else if (current() == '[') {
                        /* array declarator */
                }else{
                        printf("direct_declarator finished %d:%c\n", dptr->super.kind,current());
                        RETURN(dptr);
                }
        }


}

static struct parameter_decl_node *parameter_type_list() {

        return parameter_list();

}
static struct parameter_decl_node *parameter_list() {
        struct parameter_decl_node *dptr_first, *dptr;

        dptr_first = parameter_declaration();
        while(current() == ',' ){
                if (next() == ELLIPSIS) {
                        assert("not right now");
                        break;
                }
                dptr = parameter_declaration();
                list_add(&dptr_first->list, &dptr->list);
        }

        RETURN(dptr_first);
}

static struct parameter_decl_node *parameter_declaration() {
        struct parameter_decl_node *dptr = NEW_NODE(struct parameter_decl_node);
        
        INIT_LIST_HEAD(&dptr->list);
        declaration_specifier();
        declarator(DC_EITHER);

        RETURN(dptr);
}
struct decl_node *declaration() {
        struct decl_node *ptr;
        ptr = declaratoion_or_funcdefinition();
        assert(current() == ';');
        next();
        return ptr;
        //next();
}
/*
    looklike we need prediction for every grammar

*/
int main(int argc, char**argv){
 
        if (argc > 1){
                if ((yyin = fopen(argv[1], "r")) == 0){
                        perror(argv[1]);
                        exit(1);
                }
        }
        
        translation_unit();

//        tree a = alloc_tree(sizeof(struct tree_common));
//        TREE_SET_CODE(a, RETURN_STMT);
 //       DECL_NAME(a);

        if(argc > 1){
                fclose(yyin);
        }

}
