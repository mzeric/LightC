/*

Register Alloc

need : dom-tree

*/
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <set>
#include "dfa.h"
#include "anode.h"


int k_press = 0;
#define MAX_NEXTUSE 0xffffff
extern void get_all_var(anode expr, bb b, live_set_t &var, live_set_t &r_def);

/**********************

nextuse will NOT cross basic-block, so, if, just random pick one

*/
extern void get_all_var(anode expr, bb b, live_set_t &var, live_set_t &r_def);
int nextuse(anode stmt, anode var){
        assert(var);
        if (!stmt)  return 0;
        bb_t *b = stmt->basic_block;
        
        /* var is dead here */
        if ((*b->stmt_live)[stmt].count(var) == 0)
            return MAX_NEXTUSE;
        
        /* var used at l */
        live_set_t use_set, def_set;
        get_all_var(stmt, stmt->basic_block, use_set, def_set);
        if (use_set.count(var) > 0)
            return 0;
        
        /* var not live at l */
        // how to detect live or not

        return 1 + nextuse(ANODE_CHAIN(stmt), var);
}
void displace(Set &Q, anode stmt, Set &V, Set &X, bool spill){
        Set R;
        /* R <- V - Q */
        std::set_difference(V.begin(), V.end(),Q.begin(), Q.end(),
                std::inserter(R, R.begin()));
        X.clear();
         int max = R.size() + Q.size() - k_press;
        if (max < 0 )
                max = 0;
        for (int i = 0; i < max; ++i){
             Set::iterator iter;
             anode w = NULL;
             int m = 0;
             for (iter = Q.begin(); iter != Q.end(); ++iter){
                int ret =  nextuse(stmt, *iter);
                if (m < ret){
                        m = ret;
                        w = *iter;
                }
             }
             X.insert(w);
             
             Q.erase(w); 
             /* 
                if w not in Q. w must in V. but all in V is ZERO of next-use
                so. w must be in Q, and Must STORE it
             */
        }
        /* Q = Q + V */
        Q.insert(V.begin(), V.end());
}
void bleady_block(basic_block_t *block){
        Set in_b, Q, I, U;
        I.insert(block->live->in.begin(), block->live->in.end());
        I.insert(block->live->phi_def.begin(), block->live->phi_def.end());

        /* k_pressure max */

        Q = I;
        U.clear();

        for (anode stmt = block->entry; stmt; stmt = ANODE_CHAIN(stmt)){

                Set X;
                Set V;
                Set df;
                /* FIXME liveness of V , have here */
                get_all_var(stmt, block, V, df);


/* insert IR_RELOAD, after insert all ness IR_STORE */
   
                displace(Q, stmt, V, X, true);
/* insert IR_STORE before label */
                for (Set::iterator iter = U.begin(); iter != U.end(); ++iter)
                        X.erase(*iter);
                for (Set::iterator iter = X.begin(); iter != X.end(); ++iter)
                        in_b.erase(*iter);

                U.insert(V.begin(), V.end());
                anode stmt2 = ANODE_CHAIN(stmt);
                displace(Q, stmt2, df, X, false);
/* insert IR_STORE before label */

        }

        block->live->r_in = in_b;
        block->live->r_out = Q;

}
#define FOR_EACH_BB(b, start) for (basic_block_t *(b) = (start); (b) != EXIT_BLOCK_PTR; (b) = (b)->next)
#define FOR_EACH_PRED(e, block) for (edge (e) = (block)->pred; (e); (e) = (e)->pred_next)
#define EDGE_PRED_BLOCK(e) (e)->src
void spill_belady(basic_block_t *start, int k){
    FOR_EACH_BB(b, start){
        bleady_block(b);
    }
    FOR_EACH_PRED(e, start){
        basic_block_t *b = EDGE_PRED_BLOCK(e);
        //FIXME
        /* insert RELOAD on Edges for all in(b) - out(p) */
    }

}
void color_roution(basic_block_t *start){

}