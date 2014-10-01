#include "anode.h"
#include "lower.h"

/*
        the key is lower_expr
*/

void lower_bb(basic_block_t *b){

}

enum lower_status lower_expr(anode *expr, anode *pre, anode *post, bool(*test_if)(anode), int how_){

        anode old_expr;
        enum lower_status ret;
        
        if (expr == NULL)
                return LW_DONE;

        do {
                old_expr = *expr;
                ret = LW_DONE;

                switch(anode_code(*expr)){

                }



        }while(ret == LW_OK);

        return ret;
}
