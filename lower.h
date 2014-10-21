#ifndef _LOWER_H
#define _LOWER_H

void lower_bb(bb_t *b);

enum lower_status{
        LS_ERROR        = -2,
        LS_UNHANDLED    = -1,
        LS_OK           = 0,
        LS_DONE         = 1
};
#define how_none 0
#define how_l 1
#define how_r 2
#define how_e (how_l | how_r)

enum gimple_rhs_class
{
  IR_INVALID_RHS,   /* The expression cannot be used on the RHS.  */
  IR_TERNARY_RHS,   /* The expression is a ternary operation.  */
  IR_BINARY_RHS,    /* The expression is a binary operation.  */
  IR_UNARY_RHS,     /* The expression is a unary operation.  */
  IR_SINGLE_RHS     /* The expression is a single object (an SSA
                           name, a _DECL, a _REF, etc.  */
};
typedef union ir_stmt_d * superir;

typedef bool(*if_ir_t)(anode);

enum lower_status lower_expr(anode *expr, anode_seq *pre, anode_seq *post, if_ir_t, int how_);
enum lower_status lower_and_add(anode expr_p, anode_seq *pre_p);
enum lower_status lower_self_mod_expr(anode *expr_p, anode_seq *pre_p, anode_seq *post_p, int want);
enum lower_status lower_cond_expr(anode *expr_p, anode_seq *pre_p, int how_);
#endif