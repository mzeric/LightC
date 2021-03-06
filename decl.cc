#include "decl.h"

anode bb_node = NULL;


void print_decl(anode node){
        if(!node)
                printf("type: null\tname: null\n");
        else if (anode_code(node) == IDENTIFIER_NODE){
                printf("ssa define: %s\n",
                        IDENTIFIER_POINTER(node));

        }else{
                printf("type: %s\t%s\tname: %s\n",
                        anode_code_name(anode_code(ANODE_DECL_TYPE(node))),
                        anode_code_name(anode_code(node)),
                        IDENTIFIER_POINTER(decl_name(node)));

        }
}

void dump_block_list(anode l, int level){
        anode  t;
        for(t = l; t; t = ANODE_CHAIN(t)){
                if (anode_code_class(anode_code(t)) == 'd'){
                        print_decl(t);

                }else if (anode_code(t) == COMPOUND_STMT){
                        printf("compound_stmt {\n");
                        dump_block_list(((anode_expr*)t)->operands[0], level+1);
                        printf("}\n");
                }else if (anode_code(t) == IDENTIFIER_NODE){
                        printf("(identifier %s)", IDENTIFIER_POINTER(t));
                }else if (anode_code(t) == INTEGER_CST){
                        printf("(integer_cst %d)", ((anode_int_cst*)t)->int_cst);
                }else if (anode_code(t) == COMPOUND_STMT){

                }else{
                        printf("node : %c (%s ",anode_code_class(anode_code(t)),
                         anode_code_name(anode_code(t)));
                        for(int i = 0; i < anode_code_length(anode_code(t)); i++)
                        dump_block_list(((anode_expr*)t)->operands[i], level+1);
                        printf(")\n");
        
                }
        }

}
void dump_stmt(anode stmt){

}

void push_namespace(void){
        declspace_stack = anode_cons(NULL, current_declspaces, declspace_stack);
}
void pop_namespace(void){
        current_declspaces = ANODE_VALUE(declspace_stack);
        declspace_stack = ANODE_CHAIN(declspace_stack);

}
void push_decl(anode decl){
        /* only need to move the current_decl */
        anode_decl *d = ANODE_(ANODE_CLASS_CHECK(decl, 'd'), anode_decl);
        current_declspaces = anode_cons(NULL, d, current_declspaces);
}
void push_block_note(){
        anode_list *l = new anode_list();
        ANODE_VALUE(l) = bb_node;
        current_declspaces = anode_cons(NULL, l, current_declspaces);
}
void push_ssa_decl(anode id, anode value){

      ANODE_CHECK(id, IDENTIFIER_NODE);
      current_declspaces = anode_cons(value, id, current_declspaces);


}

anode decl_declar(anode node){
        return ANODE_CLASS_CHECK(ANODE_(node, anode_decl), 'd')->name;
}
anode get_inner_declar(anode node){
        anode name, t;
        char c = anode_code_class(anode_code(node));
        
        assert(c == 'd');
        t = node;
        while(anode_code(t) != VAR_DECL){
            t = decl_declar(t);
        }
        return t;
}
anode decl_name(anode node){
        if(anode_code(node) == IDENTIFIER_NODE)
                return node;
        anode_decl *t = ANODE_(get_inner_declar(node), anode_decl);
        return ANODE_CLASS_CHECK(t, 'd')->name;
}

anode build_var_decl(anode speci, anode decl){

        ANODE_CLASS_CHECK(decl, 'd');
        ANODE_TYPE(decl) = speci;
        return decl;
}
anode build_parm_decl(anode speci, anode parm){
        anode t = new anode_decl();
        t->code = PARM_DECL;
        ANODE_TYPE(t) = speci;

}
anode build_func_decl(anode name, anode parms ){
        anode_decl *t = new anode_decl();
        t->code = FUNCTION_DECL;
        t->name = name;
        return t;
}
anode build_if_else(anode cond, anode if_true, anode if_else){

}
anode build_stmt(int code, ...){
        anode_expr    *t;
        va_list       vp;
        int           length;
        int           i;
        va_start(vp, code);
        t = new anode_expr(code);
        length = anode_code_length(code);

        for (i = 0; i < length; i++){
            ANODE_OPERAND (t, i) = va_arg(vp, anode);
        }

        va_end(vp);
	t->decl_outer = current_declspaces;
        return t;
}
/*
 *
 *  called by lower 
 *
 */
anode build_ir_len(int code, int len, ...){
        anode_expr    *t;
        va_list       vp;
        int           length;
        int           i;
        va_start(vp, len);
        t = new anode_expr(code);

        for (i = 0; i < len; i++){
            ANODE_OPERAND (t, i) = va_arg(vp, anode);
        }

        va_end(vp);

        return t;
}


anode get_last_stmt(anode tree){
    return chain_last(tree);
}
anode add_stmt(anode *dst, anode src){
    anode last = get_last_stmt(*dst);
    ANODE_CHAIN(last) = src;
      return NULL;
}
