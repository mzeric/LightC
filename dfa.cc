/*
        author : tianxiao
        e-mail : tianxiao.xtx@gmail.com
        description:
                Data Flow Analysis, compute the def-use,liveness, etc

    two space : syntax lex space, cfg flow space, latter matters more here
    无法具体测试整个后端的效率，但可以估计的是大量的时间会消耗在dfa具体的技术上，尤其是基础信息的计算优化
*/
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <list>
#include <map>

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
/*    get use & def of one stmt  */
void get_all_var(anode expr, bb b, live_set_t &var, live_set_t &r_def){
        /* some expr in cfg left NULL intendely as ir_branch(cond, NULL, NULL) */
        if (!expr)
            return;

        if (expr == &undefine_variable){
            printf("\tundefine_variable\n");
            return;
        }
        //printf("\tGET_ALL_VAR %s\n", anode_code_name(anode_code(expr)));
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
            //printf("%s\n", anode_code_name(anode_code(ANODE_OPERAND(expr, 0))));
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

void build_bb_use_def(basic_block_t *b) {

    b->live->use.clear();
    b->live->def.clear();

    for (anode p = b->phi; p; p = ANODE_CHAIN(p)){
            anode_phi *phi = (anode_phi*)p;
            for (auto t : phi->targets){
                b->live->phi_use.insert(t);

            }

            b->live->phi_def.insert(phi->def_name);
            b->live->def.insert(phi->def_name);
    }

    b->live->use = b->live->phi_use;

    for (anode stmt_list = b->entry; stmt_list; stmt_list = ANODE_CHAIN(stmt_list)){
        anode stmt = ANODE_VALUE(stmt_list);
        printf("dfa %d epxr: %s\n", b->index, anode_code_name(anode_code(stmt)));


        live_set_t l_use, l_def;

        get_all_var(stmt, b, l_use, l_def);
        /* phi is nor var */

        for (auto it: l_use){
                if (b->live->def.find(it) == b->live->def.end())
                    b->live->use.insert(it);
        }

        b->live->def.insert(l_def.begin(), l_def.end());

    }
}
/******************************************************************



******************************************************************/
void compute_bb_liveness(basic_block_t *start, basic_block_t *end){
    bool in_changed = true;

    EXIT_BLOCK_PTR->live->in.clear();

    for (basic_block_t *b = start; b != end; b = b->next)
        b->live->in.clear();

    while(in_changed) {
        in_changed = false;

        for (basic_block_t *b = start; b != end; b = b->next) {
            /* 
                out(b) = phi_in(b->succ) || phi_in (b->succ) 
                ? ==>
                out(b) = in(b->succ) || in(b->succ)
                in(b) = use(b)_use_before_def || (out(b) - def(b))
            */

            live_set_t union_out;
            live_set_t diff_set;
            live_set_t union_in;

            for (edge e = b->succ; e; e = e->succ_next){
                live_set_t::iterator succ_iter;
                live_ness_t *np = e->dst->live;

                for (auto it: np->in){
                    assert(anode_code(it) == IR_SSA_NAME);

/*                    edge be = (*e->dst->phi_edge)[(anode_ssa_name*)*succ_iter];
           
                    if (be && be->src != b)
                        continue;
*/
                    union_out.insert(it);
                    

                }

            }


            set_difference(union_out.begin(), union_out.end(),
                            b->live->def.begin(), b->live->def.end(),
                            std::inserter(diff_set, diff_set.begin()));

            set_union(b->live->use.begin(), b->live->use.end(),
                            diff_set.begin(), diff_set.end(),
                            std::inserter(union_in, union_in.begin()));
            if (b->live->in != union_in)
                in_changed = true;


            b->live->in = union_in;
            b->live->out = union_out;            

        }
    }
    
}
/* compute after build the in/out */
void build_bb_gen(basic_block_t *b) {
        /* for ssa it's defs also, in ssa_table */
        ssa_table_t::iterator iter;
        live_set_t t_set;
        for (iter = b->ssa_table->begin(); iter != b->ssa_table->end(); ++iter){
            if (anode_code(iter->second) != IR_PHI)
                t_set.insert(iter->first);
        }
        set_intersection(t_set.begin(), t_set.end(),
                    b->live->out.begin(), b->live->out.end(),
                    inserter(b->live->gen, b->live->gen.begin()));
}
void build_bb_kill(basic_block_t *b) {
        /* || OUT(pred(b)) - OUT(b) */
}
void kill_kill(live_set_t &old, live_set_t &def){
    live_set_t tmp;
    for (auto it : old){
        for (auto def_it: def){
            printf("kill is_ssa_name? %s\n", anode_code_name(anode_code(it)));
            if (anode_code(it) == IR_SSA_NAME){
                anode_ssa_name *a = (anode_ssa_name*)it;
                anode_ssa_name *b = (anode_ssa_name*)def_it;
                printf("kill %s -> %s\n", anode_code_name(anode_code(a)),
                        anode_code_name(anode_code(b)));
                if (a->var == b->var && a->version != b->version){
                    /* not iterated */
                    tmp.insert(it);
                }
            }
        }
    }
    old.erase(tmp.begin(), tmp.end());
}
void compute_stmt_live(bb_t *b){
    live_set_t start_live;
    set_difference(b->live->in.begin(), b->live->in.end(),
                b->live->phi_use.begin(), b->live->phi_use.end(),
                std::inserter(start_live, start_live.begin()));
    start_live.insert(b->live->phi_def.begin(), b->live->phi_def.end());

    /*
        start_live = start_live + use
    */
    live_set_t lp = start_live;
    for (anode stmt = b->entry; stmt; stmt = ANODE_CHAIN(stmt)){
        live_set_t l_use, l_def;

        get_all_var(stmt, b, l_use, l_def);
        //printf("debug kill %lu, %lu\n", l_use.size(), l_def.size());
        /* GEN = l_use + l_def */
        lp.insert(l_use.begin(), l_use.end());
        lp.insert(l_def.begin(), l_def.end());

        /* KILL = ssa_name < l_def */

        /* lpn = lpn' + GENn - KILLn*/
        //printf("handle kill %lu, %lu\n", lp.size(), l_def.size());
        kill_kill(lp, l_def);
        (*b->stmt_live)[stmt] = lp;
    }
}
void compute_bb_gen_kill(basic_block_t *start, basic_block_t *end){
    for (bb_t *b_b = start; b_b != end; b_b = b_b->next){

    }
}

typedef std::set<basic_block_t*> iter_block_t;

typedef std::list<basic_block_t*> list_t;
static int postorder = 0;
std::map<basic_block_t*, int> doms_post;
void visit(basic_block_t *node, list_t& l){

    for (edge e = node->succ; e; e = e->succ_next){
        visit(node, l);
    }

    l.insert(l.begin(), node);
    doms_post[node] = postorder++;
}

void get_reverse_postorder(basic_block_t *start, list_t &l){
    visit(start, l);
}
std::map<basic_block_t*, basic_block_t*> doms;

basic_block_t *intersect(basic_block_t *a, basic_block_t *b){

    int ap,bp;
    ap = doms_post[a];
    bp = doms_post[b];


    while (ap != bp){
        while(ap < bp){
            a = doms[a];
            ap = doms_post[a];
        }
        while(ap > bp){
            b = doms[b];
            bp = doms_post[b];
        }
    }

    return a;
}
std::map<basic_block_t*, std::list<basic_block_t*> >doms_childs;
void doms_to_tree(){
    basic_block_t *doms_root = (basic_block_t*)malloc(sizeof(basic_block_t));
    typedef std::map<basic_block_t*, basic_block_t*> doms_t;
    for (doms_t::iterator iter = doms.begin(); iter != doms.end(); ++iter){
        basic_block_t *k = iter->first;
        basic_block_t *p = iter->second;
        while(p){
            doms_childs[p].push_back(k);
            k = p;
            p = doms[p];
        }
    }

}
void compute_cfg_dom(basic_block_t *start){
    bool changed = true;
    list_t rp;
    get_reverse_postorder(start, rp);
    /* remove the start node */
    FOR_EACH_BB(b, start){
        doms[b] = NULL;
    }

    doms[start] = start;
    changed = true;

    while(changed){
        changed = false;
        list_t::iterator iter;
        for (iter = rp.begin(); iter != rp.end(); ++iter){
            basic_block_t *new_idom = (*iter)->pred->src;
            for (edge e = (*iter)->pred->pred_next; e; e = e->pred_next){
                if (doms[e->src] != NULL)
                    new_idom = intersect(e->src, new_idom);
            }
            if (doms[*iter] != new_idom){
                doms[*iter] = new_idom;
                changed = true;
            }

        }
    }

    doms_to_tree();

}
/* compute ssa.defs & ssa.uses info 

    ssa_defs[VAR_DECL] = list_t<anode>
    ssa_uses[VAR_DECL] = list_t<pair<anode, int> >

*/
typedef std::map<anode, std::list<anode> > ssa_defs_t;
typedef std::map<anode, std::list<pair<anode, int> > > ssa_uses_t;
typedef std::set<pair<anode, int> > uses_set_t;

void get_all_var_pair(anode expr, int index, bb b, uses_set_t &var, live_set_t &r_def){
        /* some expr in cfg left NULL intendely as ir_branch(cond, NULL, NULL) */
        anode old_expr = expr;
        if (index >= 0)
            expr = ANODE_OPERAND(expr, index);


        if (!expr)
            return;

        if (expr == &undefine_variable){
            printf("\tundefine_variable\n");
            return;
        }
        printf("\tGET_ALL_VAR %s\n", anode_code_name(anode_code(expr)));
        if (anode_code(expr) == IDENTIFIER_NODE){
            printf("id:%s\n",IDENTIFIER_POINTER(decl_name(expr)));
            
        }

        int len = anode_code_length(anode_code(expr));

           
        if (anode_code_class(anode_code(expr)) == 'c'){
            return;
        }

        if (anode_code(expr) == IR_SSA_NAME || anode_code(expr) == IR_PHI){
            var.insert(pair<anode, int>(old_expr, index));
            return;
        }
        if (len == 0)
            return;
        if (anode_code(expr) == MODIFY_EXPR){
            printf("%s\n", anode_code_name(anode_code(ANODE_OPERAND(expr, 0))));
            r_def.insert(ANODE_OPERAND(expr, 0));
            get_all_var_pair(expr, 1, b, var, r_def);

        }else if (EXPR_P(expr)){
            for (int i = 0; i < len; ++i){
                get_all_var_pair(expr, i, b, var, r_def);
            }

        }


}
/*
    v.defs could in anode->ssa_defs
    v.uses could in anode->ssa_uses
*/
int compute_ssa_defs_uses(basic_block_t *start, basic_block_t *end){
    ssa_defs_t ssa_defs;
    ssa_uses_t ssa_uses;

    _FOR_EACH_BB(b, start, end){
        uses_set_t s_uses;
        live_set_t s_defs;

        for (anode stmt_list = b->entry; stmt_list; stmt_list = ANODE_CHAIN(stmt_list)){
            anode stmt = ANODE_VALUE(stmt_list);

            get_all_var_pair(stmt, -1, b, s_uses, s_defs);
        }
        
        for(live_set_t::iterator iter = s_defs.begin(); iter != s_defs.end(); ++iter){
                anode_ssa_name *def= (anode_ssa_name*)*iter;
                assert(anode_code(def) == IR_SSA_NAME || anode_code(def) == IR_PHI);
                
                /* work so, just follow order inside one block */
                ssa_defs[def->var].push_back(def);
        }

        for(uses_set_t::iterator iter = s_uses.begin(); iter != s_uses.end(); ++iter){
                anode expr = iter->first;
                int i = iter->second;
                anode_ssa_name *u = (anode_ssa_name*)&ANODE_OPERAND(expr, i);
                ssa_uses[u->var].push_back(*iter);
                
        }
    }
    /* now fill the ssa_defs_t & ssa_uses_t */
    

}


/* calc the define-rechable in/out of bb */

/* cals the live-variable info of bb */

/*
    Def-Use from use backward to it's Latest Define

    once have the Defs-DAG, we got Liveness at each stmt, if it on path of some var's Def-use path

*/

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
    for (basic_block_t *t = b; t != EXIT_BLOCK_PTR; t = t->next){
        build_bb_use_def(t);
    }
    compute_bb_liveness(b, EXIT_BLOCK_PTR);
    for (basic_block_t *t = b; t != EXIT_BLOCK_PTR; t = t->next){
        compute_stmt_live(t);
    }
}
