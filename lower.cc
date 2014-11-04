#include "anode.h"
#include "lower.h"

static size_t tmp_name_id = 0;
#define tcc_unary '1'
#define tcc_binary '2'
#define tcc_comparison '<'
#define tcc_reference 'r'
#define tcc_constant 'c'
#define tcc_declaration 'd'

#define DEFTREECODE(SYM, STRING, TYPE, NARGS)                               \
  (unsigned char)                                                           \
  ((TYPE) == tcc_unary ? IR_UNARY_RHS                                   \
   : ((TYPE) == tcc_binary                                                  \
      || (TYPE) == tcc_comparison) ? IR_BINARY_RHS                      \
   : ((TYPE) == tcc_constant                                                \
      || (TYPE) == tcc_declaration                                          \
      || (TYPE) == tcc_reference) ? IR_SINGLE_RHS                       \
   : ((SYM) == TRUTH_AND_EXPR                                               \
      || (SYM) == TRUTH_OR_EXPR                                             \
      || (SYM) == TRUTH_XOR_EXPR) ? IR_BINARY_RHS                       \
   : (SYM) == TRUTH_NOT_EXPR ? IR_UNARY_RHS                             \
   : ((SYM) == COND_EXPR                                                    \
        ? IR_TERNARY_RHS                            \
   : (SYM) == ADDR_EXPR                                                 \
      || (SYM) == WITH_SIZE_EXPR                                            \
      || (SYM) == IR_SSA_NAME) ? IR_SINGLE_RHS                             \
   : IR_INVALID_RHS),
#define END_OF_BASE_TREE_CODES (unsigned char) IR_INVALID_RHS,

const unsigned char ir_rhs_table[] = {
#include "c-tree.def"
};
#undef DEFTREECODE
#undef END_OF_BASE_TREE_CODES
int ir_rhs_code (anode t){
        return ir_rhs_table[ANODE_CODE(t)];
}
bool is_ir_variable(anode t)
{
        return (ANODE_CODE(t) == VAR_DECL
                || ANODE_CODE(t) == PARM_DECL
                || ANODE_CODE(t) == RESULT_DECL
                || ANODE_CODE(t) == IR_SSA_NAME);
}
bool is_ir_id(anode t)
{
        return (is_ir_variable(t)
                || ANODE_CODE(t) == FUNCTION_DECL
                || ANODE_CODE(t) == LABEL_DECL
                || ANODE_CODE(t) == STRING_CST /* string  is addressable too */
                || ANODE_CODE(t) == CONST_DECL
                );
}
bool is_ir_addressable(anode t)
{
        return (is_ir_id(t));
}
bool is_ir_lvalue(anode t)
{
        /* only id and *(expr) could is left-value */
        return (is_ir_addressable(t)
                || ANODE_CODE(t) == INDIRECT_REF);
}
bool is_ir_constant(anode t)
{
        switch (ANODE_CODE(t)){
                case INTEGER_CST:
                case VECTOR_CST:
                case STRING_CST:
                        return true;
                default:
                        return false;
        }
}
bool is_ir_live_memory(anode t){
    //if (ANODE_CODE(t) == )
}
bool is_ir_reg(anode t)
{
    if (ANODE_CODE(t) == IR_SSA_NAME)
        return true;

    if (!is_ir_variable(t))
        return false;
    if (is_ir_live_memory(t))
        return false;
    return true;
}
bool is_ir_val(anode t)
{
        /* valitolite is variable but not val */
        return (is_ir_variable(t) || is_ir_constant(t));
}
bool is_ir_stmt(anode t){
  int code = ANODE_CODE (t);

  switch (code)
    {
  //   case BIND_EXPR:
    case COND_EXPR:
      /* These are only valid if they're void.  */
//      return ANODE_TYPE (t) == NULL || VOID_TYPE_P (ANODE_TYPE (t));

    case SWITCH_EXPR:
    case GOTO_EXPR:
    case RETURN_EXPR:
    case LABEL_EXPR:
    case CASE_LABEL:

 //   case STATEMENT_LIST:
        return true;

    case CALL_EXPR:
    case MODIFY_EXPR:
      /* These are valid regardless of their type.  */
      return true;

    default:
      return false;
    }
}
/* ok to assign to some var
*/
bool is_ir_rhs_or_call(anode t){
        int code = ANODE_CODE(t);
        return (ir_rhs_code(t) != IR_INVALID_RHS ||
                code == CALL_EXPR);
}

bool is_ir_xx(anode t)
{

}

/* Push the temporary variable into current binding */
void lower_add_tmp_var(anode tmp){

}
anode create_tmp_var_name(const char *prefix){
        char *name = NULL;

        asprintf(&name, "%s.%lu", prefix ? prefix : "T", tmp_name_id++);
        anode var = new anode_identifier(name);
        free(name);

        return var;
}
anode create_tmp_var_raw(anode type, const char *prefix){
        anode decl;

        decl = build_decl(VAR_DECL, create_tmp_var_name(prefix), type);

        return decl;
}
/*
        before data-flow (ssa), val is anode_identifier
        after is VAR_DECL
*/
anode create_tmp_var(anode val){
        anode type = ANODE_TYPE(val);
        anode var = create_tmp_var_raw(type, IDENTIFIER_POINTER(val));
        return var;
}
anode get_init_tmp_var(anode value, anode_seq *pre_p, anode_seq *post_p){
        anode t,init;

        lower_expr(&value, pre_p, post_p, is_ir_rhs_or_call, how_r);

        t = create_tmp_var(value);
        init = build_stmt(INIT_EXPR, t, value);
        ANODE_TYPE(init) = ANODE_TYPE(t);
        lower_and_add(init, pre_p);
}
anode get_formal_tmp_var(anode_seq expr, anode_seq pre_p)
{

}
enum lower_status lower_ir_assign(anode dst, anode src, anode_seq *seq){

}
void lower_add_ir(anode_seq *dst, anode_seq src){
        anode last;
        assert(ANODE_CODE(*dst) == ANODE_LIST);

        if (src == NULL)
                return;

        last = chain_last(*dst);
        if (ANODE_CODE(src) == ANODE_LIST){
                chain_cat(last, src);
        }else{
                anode n = build_list(NULL, src);
                ANODE_CHAIN(last) = n;
        }
}
void lower_seq_add(anode_seq *dst, anode_seq src){
    if (src == NULL)
        return;
    if (*dst == NULL)
        *dst = new anode_list();

    chain_cat(*dst, src);
}

/*      pre is the side effects must happen before expr is stored
        post is the side effects must happen after expr is stored

*/
typedef anode_list* anode_seq;
enum lower_status lower_expr(anode *expr_p, anode_seq *pre_p, anode_seq *post_p,
                                    if_ir_t ir_test_if, int how_){

        anode old_expr;
        enum lower_status ret;

        anode_seq internal_pre = NULL;
        anode_seq internal_post = NULL;

        anode tmp;



        if (*expr_p == NULL)
                return LS_DONE;

        if (pre_p == NULL)
                pre_p = &internal_pre;
        if (post_p == NULL)
                post_p = &internal_post;

        do {
                old_expr = *expr_p;


                ret = LS_UNHANDLED;

                switch(anode_code(*expr_p)){

                case DECL_STMT:{
                    anode e = ANODE_OPERAND((*expr_p), 0);
                            /* may have multi VAR_DECL or FUNCTION_DECL, XXX_DECL */
                    if (ANODE_CODE(e) == VAR_DECL){
                        while(e){

                            anode_decl *var = (anode_decl*)e;

                            e = e->chain;
                        }
                    }
                }
                break;
                case POSTDECREMENT_EXPR:
                case POSTINCREMENT_EXPR:
                case PREDECREMENT_EXPR:
                case PREINCREMENT_EXPR:
                    ret = lower_self_mod_expr(expr_p, pre_p, post_p, how_none != how_);
                    break;

                case COND_EXPR:
                    ret = lower_cond_expr(expr_p, pre_p, how_);

                    break;

                case INTEGER_CST:
                case STRING_CST:
                    ret = LS_DONE;
                    break;

                default:
                    switch(anode_code_class(ANODE_CODE(*expr_p))){
                        case '1':
                            ret = lower_expr(&ANODE_OPERAND(*expr_p, 0),
                                pre_p, post_p, is_ir_val, how_r);
                            break;
                        case '>':
                            break;
                    
                    }

                    break;

                }

        }while(ret == LS_OK);
        /* check error */

        if (ret == LS_ERROR){
            if (ir_test_if == is_ir_stmt)
                *expr_p = NULL;
            goto out;
        }

        //assert(ret != LS_UNHANDLED);
        if (how_ == how_none && *expr_p && !is_ir_stmt(*expr_p)){



        }
        if (how_ == how_none || ir_test_if == is_ir_stmt){
            /* CASE CLOSED */
            //
            if (internal_pre != NULL || internal_post != NULL){
                //ir
                lower_seq_add(&internal_pre, internal_post);
                lower_seq_add(pre_p, internal_pre);
                goto out;
            }
        }

        /* only no-inter-postqueue && (id || *()) has the left-value */

        if (internal_post == NULL && (*ir_test_if)(*expr_p)){
            goto out;
        }
        /* new temporary needed to be created */
        if ((how_ & how_l) && internal_post == NULL && is_ir_addressable(*expr_p)){


            tmp = build_addr_expr(*expr_p);

            lower_expr(&tmp, pre_p, post_p, is_ir_reg, how_r);

            anode_int_cst *int_zero = new anode_int_cst(0);
            ANODE_TYPE(int_zero) = new anode_type(INTEGER_TYPE);

            *expr_p = build_stmt(MEM_REF, tmp, int_zero);

        }else if ((how_ & how_r) && is_ir_rhs_or_call(*expr_p)){

            *expr_p = get_init_tmp_var(*expr_p, pre_p, NULL);

        }else {
            ret = LS_ERROR;
            goto out;
        }

out:
        return ret;
}
/*
        For d = (*p++)--
generate:
*p++ =>         t0 = *p;

 post= p++;     t1 = t0 - 1;            build arith_code
 upload post    *p = t1;                gimplify_assign
                d = t0;                 *expr_p = lhs; ret = ALL_DONE

-------------
dule the post by upper
-------------
                 p = p + 1;             post later executed

        For  (*p++)--


                t0 = *p;                all-executed by self_mod
dule post:      
                t1 = t0 - 1;
                lower(*p = t1 ) with-post: p = p + 1;  ret = OK;  lower AGAIN
                



*/
enum lower_status lower_self_mod_expr(anode *expr_p, anode_seq *pre_p, anode_seq *post_p, int want)
{
        int code, arith_code;

        bool postfix;
        anode lvalue, lhs, rhs, t1;
        anode_seq post = NULL, *orig_post = post_p;
        lower_status ret;

        code = ANODE_CODE(*expr_p);

        assert(code == POSTINCREMENT_EXPR || code == POSTDECREMENT_EXPR
                || code == PREINCREMENT_EXPR || code == PREDECREMENT_EXPR);

        if (code == POSTDECREMENT_EXPR || code == POSTDECREMENT_EXPR)
                postfix = want;
        else
                postfix = false;
        if (postfix)/* we need the post-side-effect executed after this expr self*/
                post_p = &post;

        if (code == POSTDECREMENT_EXPR || code == PREDECREMENT_EXPR)
                arith_code = MINUS_EXPR;
        else
                arith_code = PLUS_EXPR;

        lvalue = ANODE_OPERAND(*expr_p, 0);
        /* we need the l-value */
        if((ret = lower_expr(&lvalue, pre_p, post_p, is_ir_lvalue, how_l)) == LS_ERROR) {
                return ret;
        }
        lhs = lvalue;
        rhs = ANODE_OPERAND(*expr_p, 1);
        /* we need the r-value from l-value */
        if (postfix){
                if((ret = lower_expr(&lhs, pre_p, post_p, is_ir_val, how_r)) == LS_ERROR)
                        return ret;
                lhs = get_init_tmp_var(lhs, pre_p, NULL);
        }

        if (postfix){
                lower_ir_assign(lvalue, t1, pre_p);/* generate seq: x = p0 */
                lower_add_ir(orig_post, post);/*  add 'p++'      */
                *expr_p = lhs;
                return LS_DONE;
        }else {
                *expr_p = build_stmt(MODIFY_EXPR, lvalue, t1);
                ANODE_TYPE(*expr_p) = ANODE_TYPE(lvalue);


                return LS_OK;
        }

}

enum lower_status lower_cond_expr(anode *expr_p, anode_seq *pre_p, int how){

}
enum lower_status lower_stmt(anode *expr_p, anode_seq *pre_p){
        if (*pre_p == NULL){
            *pre_p = new anode_list();
        }

        lower_expr(expr_p, pre_p, NULL, is_ir_stmt, how_none);
}
enum lower_status lower_and_add(anode expr_p, anode_seq *pre_p){
        lower_stmt(&expr_p, pre_p);
}



/*
        the key is lower_expr
*/
void lower_block(anode *ast_expr, anode_seq *ret_seq){
    anode expr;
    for (expr = *ast_expr; expr; expr = ANODE_CHAIN(expr)){
        lower_stmt(&ANODE_VALUE(expr), ret_seq);
    }

}
void lower_bb(basic_block_t *b){
    basic_block_t *t;
    for (t = b; t != EXIT_BLOCK_PTR; t = t->next) {
        anode_seq ret = NULL;
        lower_block(&t->entry, &ret);
        t->lower_ir = ret;
    }
}

