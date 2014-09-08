/*

Register Alloc


*/
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <set>
#include "dfa.h"
#include "anode.h"


int k_press = 0;

/**********************

nextuse will NOT cross basic-block, so, if, just random pick one

*/
int nextuse(anode stmt, anode var){
        assert(var);
        if (!stmt)  return 0;
        bb_t *b = stmt->basic_block;
        
        /* var used at l */
        if ((*b->stmt_live)[stmt].count(var) != 0)
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
             /* insert IR_SPILL */
             Q.erase(w);
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

                Set X;Set V;
                displace(Q, stmt, V, X, true);

                /* insert IR_RELOAD, after insert all ness IR_SPILL */

                for (Set::iterator iter = U.begin(); iter != U.end(); ++iter)
                        X.erase(*iter);
                for (Set::iterator iter = X.begin(); iter != X.end(); ++iter)
                        in_b.erase(*iter);
                Set df;
                U.insert(V.begin(), V.end());
                displace(Q, stmt, df, X, false);

        }

        block->live->r_in = in_b;
        block->live->r_out = Q;

}