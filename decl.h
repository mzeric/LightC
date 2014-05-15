#ifndef __DECL__H
#define __DECL__H
#include <stdio.h>
#include "anode.h"

struct c_scope{
        struct c_scope *outer;
        struct c_scope *outer_function;

        anode names;
        anode names_last;

        anode parms;/* store all parms of the function declaration */
        anode parms_last;
        /* tag the struct, union, enum type */
        anode tags;

        anode shadowed;/* a anode_list */
        anode shadowed_tags;
        anode blocks; /* a block chain */
        anode blocks_last;

        int parm_flag : 1;
        int warned_forward_parm_decls : 1;
        int function_body : 1; /* True if this contains the parms ... */
        int keep : 1;
};

#define SCOPE_LIST_APPEND(scope, list, decl) do {       \
        struct c_scope *s_ = (scope);                   \
        anode d_ = (decl);                               \
        if (s_->list##_last)                            \
                TREE_CHAIN (s_->list##_last) = d_;      \
        else                                            \
                s_->list = d_;                          \
        s_->list##_last = d_;                           \
} while (0)

#define SCOPE_LIST_CONCAT(tscope, to, fscope, from) do {\
        struct c_scope *t_ = (tscope);                  \
        struct c_scope *f_ = (fscope);                  \
        if (t_->to##_last)                              \
                TREE_CHAIN (t_->to##_last) = f_->from;  \
        else                                            \
                t_->to = f_->from;                      \
        t_->to##_last = f_->from##_last;                \
} while (0)

void pushlevel (int dummy);


#endif
