/*
        author : tianxiao
        e-mail : tianxiao.xtx@gmail.com
        description:
                Data Flow Analysis, compute the def-use,liveness, etc

    two space : syntax lex space, cfg flow space, later matter more here
*/
#include <stdio.h>
#include <stdlib.h>

#include "dfa.h"
#include "anode.h"

using namespace std;

bool is_const(anode n){
        switch(anode_code(n)){
                case INTEGER_CST:
                case STRING_CST:
                        return true;
                default:
                        return false;
        }
}
bool is_variable(anode n){
        switch(anode_code(n)){
                case VAR_DECL:
                case PARM_DECL:
                case RESULT_DECL:
              //  case SSA_NAME:
                        return true;
                default:
                        return false;
        }
}
bool is_id(anode n){
        return (is_variable(n) || anode_code(n) == FUNCTION_DECL
                || anode_code(n) == LABEL_DECL );
}
/* r-value */
bool is_val(anode n){
        return is_variable(n);
}

/*
        we need the def-use chain, have to modify ssa name valuing
        every-basic_block has a def_use table
*/
anode get_ssa_def(anode e, bb b){
    return read_variable(get_def(e, b), b);
}
void get_all_var(anode expr, bb b, live_set_t &var, live_set_t &r_def){
        /* some expr in cfg left NULL intendely as ir_branch(cond, NULL, NULL) */
        if (!expr)
            return;

        if (expr == &undefine_variable){
            printf("\tundefine_variable\n");
            return;
        }
        printf("\t%s\n", anode_code_name(anode_code(expr)));
        if (anode_code(expr) == IDENTIFIER_NODE){
            printf("id:%s\n",IDENTIFIER_POINTER(decl_name(expr)));
            
        }

        int len = anode_code_length(anode_code(expr));

           
        if (anode_code_class(anode_code(expr)) == 'c'){
            return;
        }

        if (anode_code(expr) == IR_SSA_NAME || anode_code(expr) == IR_PHI){
            var.insert(expr);
            return;
        }
        if (len == 0)
            return;
        if (anode_code(expr) == MODIFY_EXPR){
            printf("%s\n", anode_code_name(anode_code(ANODE_OPERAND(expr, 0))));
            r_def.insert(ANODE_OPERAND(expr, 0));
            get_all_var(ANODE_OPERAND(expr, 1), b, var, r_def);

        }else if (EXPR_P(expr)){
            for (int i = 0; i < len; ++i){
                get_all_var(ANODE_OPERAND(expr, i), b, var, r_def);
            }

        }


}
void get_gen(anode stmt, basic_block_t *b, set<anode> &v){
        if (anode_code(stmt) == MODIFY_EXPR){
            anode a = ANODE_OPERAND(stmt, 0);
            a = get_def(a, b);

        }
}
void get_kill(anode stmt, basic_block_t *b, set<anode> &v){

}
void build_bb_gen(basic_block_t *b) {
        /* for ssa it's defs also, in ssa_table */
        ssa_table_t::iterator iter;
        for (iter = b->ssa_table->begin(); iter != b->ssa_table->end(); ++iter){
            if (anode_code(iter->second) != IR_PHI)
                b->live->gen.insert(iter->first);
        }
}
void build_bb_kill(basic_block_t *b) {

}
void build_bb_use_def(basic_block_t *b) {


    for (ssa_table_t::iterator iter = b->ssa_table->begin(); iter != b->ssa_table->end(); ++iter){

        assert(iter->first);
        if (anode_code(iter->second) == IR_PHI){
            printf("SUDDEN PHI\n");
            anode_phi *phi = (anode_phi*)iter->second;
            for (anode t = phi->targets; t; t = ANODE_CHAIN(t))
                b->live->use.insert(ANODE_VALUE(t));
        }
        b->live->def.insert(iter->first);
    }

    for (anode stmt_list = b->entry; stmt_list; stmt_list = ANODE_CHAIN(stmt_list)){
        anode stmt = ANODE_VALUE(stmt_list);
        printf("dfa %d epxr: %s\n", b->index, anode_code_name(anode_code(stmt)));
        live_ness_t *l = new live_ness_t();
        ANODE_PURPOSE(stmt_list) = (anode)l;

        live_set_t l_use, l_def;

        get_all_var(stmt, b, l_use, l_def);
        /* phi is nor var */

        for (live_set_t::iterator iter = l_use.begin(); iter != l_use.end(); ++iter){
                if (b->live->def.find(*iter) == b->live->def.end())
                    b->live->use.insert(*iter);
        }
        b->live->def.insert(l_def.begin(), l_def.end());

    }
}
void build_bb_def(basic_block_t *b) {

}
/* calc the define-rechable in/out of bb */

/* cals the live-variable info of bb */

/*
    Def-Use from use backward to it's Latest Define

    once have the Defs-DAG, we got Liveness at each stmt, if it on path of some var's Def-use path

*/
template<typename T>
void build_edge(T src, T dst, int flag){

}
void t_test_df(){
    build_edge<int>(1, 2, 3);
}
bool same_expr(anode e_a, anode e_b) {
    if (anode_code(e_a) == IDENTIFIER_NODE && 
        anode_code(e_b) == IDENTIFIER_NODE){
            if (strcmp(IDENTIFIER_POINTER(decl_name(e_a)), IDENTIFIER_POINTER(decl_name(e_b))) == 0)
                return true;
            else
                return false;
        }
    if (anode_code(e_a) == IDENTIFIER_NODE || anode_code(e_b) == IDENTIFIER_NODE)
        return false;
    if (anode_code_length(anode_code(e_a)) != anode_code_length(anode_code(e_b)))
        return false;
    if (anode_code(e_a) != anode_code(e_b))
        return false;
    int length = anode_code_length(anode_code(e_a));
    for (int i = 0; i < length; i++){
        if (!same_expr(ANODE_OPERAND(e_a, i), ANODE_OPERAND(e_b, i)))
            return false;
    }
    return true;

}
void dfa_handle(basic_block_t *b){
    for (basic_block_t *t = b->next; t; t = t->next){
        build_bb_use_def(t);
    }
    //inplace_merge;
}