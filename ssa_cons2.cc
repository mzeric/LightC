
#include <list>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <stack>

#include "anode.h"
#include "dfa.h"

using namespace std;

typedef map<anode, set<basic_block_t*> > var_bset_t; /* vars & defsites */
var_bset_t var_defsite;
var_bset_t var_phisite;

typedef map<anode, int> C_t;/* Count[var] */
C_t C;
typedef map<anode, stack<anode_ssa_name*> > S_t; /* stack of var = S[var] */
S_t S;

typedef map<basic_block_t*, set<basic_block_t*> > ssa_DF_t;
ssa_DF_t ssa_DF;
map<basic_block_t*, basic_block_t*> ssa_IDOM;
map<basic_block_t*, vector<basic_block_t*> > ssa_D_Tree;


anode get_def2(anode id, anode decl){
	for (anode t = decl; t; t = ANODE_CHAIN(t)){
		anode d = decl_name(ANODE_VALUE(t));
		const char *name = IDENTIFIER_POINTER(id);

		if (strcmp(IDENTIFIER_POINTER(d), name) == 0)
			return ANODE_VALUE(t);
	}

	return &undefine_variable;
}
void get_stmt_def(anode expr, anode decl, live_set_t &r_def){
        if(!expr)
                return;
        if(anode_code(expr) == MODIFY_EXPR){
                r_def.insert(ANODE_OPERAND(expr, 0));
        }
}
void get_stmt_use_def(anode &expr, anode decl, live_set_p_t &r_use, live_set_p_t &r_def){
        if(!expr)
                return;
        if(expr == &undefine_variable){
                printf("\tundefine_variable\n");
                return;
        }
        char c = anode_code_class(anode_code(expr));
        printf("*** >>> %s <<<\n", anode_code_name(anode_code(expr)));
        if(c == 'c')
                return;
        if(c == 'd'){
                printf("get used >>>: %s addr %p\n", IDENTIFIER_POINTER(decl_name(expr)), &expr);
                r_use.insert(&expr);
                return;
        }
        int len = anode_code_length(anode_code(expr));
        if(len == 0)
                return;
        if(EXPR_P(expr)){
                printf("get exp %s\n", anode_code_name(anode_code(expr)));
        }
        if(anode_code(expr) == DECL_STMT){
                for(anode *t = &ANODE_VALUE(ANODE_OPERAND(expr,0)); *t; t = &ANODE_CHAIN(*t)){
                        printf("get def <<<: %s \n", IDENTIFIER_POINTER(decl_name(*t)));
                        r_def.insert(t);
                }

        }else if(anode_code(expr) == MODIFY_EXPR){
                r_def.insert(&ANODE_OPERAND(expr, 0));
                printf("get def <<<: new %s \n", IDENTIFIER_POINTER(decl_name(ANODE_OPERAND(expr,0))));

                get_stmt_use_def(ANODE_OPERAND(expr, 1), decl, r_use, r_def);
        }else if(EXPR_P(expr)){
                for(int i = 0; i < len; i++){
                        get_stmt_use_def(ANODE_OPERAND(expr, i), decl, r_use, r_def);
                }
        }
}


void fill_the_defsite(basic_block_t *start){
        for(basic_block_t *t = start; t != EXIT_BLOCK_PTR; t = t->next){
                for(anode stmt_list = t->entry; stmt_list; stmt_list = ANODE_CHAIN(stmt_list)){
                        anode stmt = ANODE_VALUE(stmt_list);

                        live_set_p_t use, def;
                        get_stmt_use_def(stmt, stmt->decl_outer, use, def);
			for(auto iter = def.begin(); iter != def.end(); ++iter){
				var_defsite[*(*iter)].insert(t);
                                char *pname = IDENTIFIER_POINTER(decl_name(**iter));
                                printf("test ** %s\n", pname);
			}

                        cout<<"fill_the_defsite in "<<t->index<<" use "<<use.size()<<" "<<def.size()<<endl;
                }
        }
}
/* 
 * which th pred of Y is X 
 *	My be store the ith in edge
 * */
int which_pred_n(basic_block_t *x, basic_block_t *y){
	int i = 0;
	for(edge e = y->pred; e; e = e->pred_next){
		if(e->src == x)
			return i;
		i++;
	}

	assert("no pred of Y is X");

}
edge which_pred(basic_block_t *x, basic_block_t *y){
        for(edge e = y->pred; e; e = e->pred_next)
                if(e->src == x)
                        return e;
        assert("no pred of y is x");
        return NULL;
}

anode_phi *get_phi(anode var, basic_block_t *y){
        for (anode t = y->phi; t; t = ANODE_CHAIN(t)){
                anode_phi *v = (anode_phi*)t;
                if(v->def_name->var == var)
                        return v;
        }
        assert("no phi in y");
        return NULL;
}

void insert_phi(anode var, basic_block_t *block){
	/* no need */
        char *pname = IDENTIFIER_POINTER(decl_name(var));
	printf("insert phi_var(%s) in %d (%u)\n", pname, block->index, block->index);
        anode_phi * phi = new anode_phi(block);
        phi->def_name = new anode_ssa_name(var);
        phi->def_name->def_stmt = phi;
        for(edge e = block->pred; e; e = e->pred_next){
                phi->targets.push_back(NULL);
        }

}
void place_phi(basic_block_t *start){
	set<anode> all_vars;

	var_bset_t PHI;

	for (var_bset_t::iterator iter_var = var_defsite.begin(); iter_var != var_defsite.end(); ++iter_var){
		set<basic_block_t*> w = iter_var->second;
		set<basic_block_t*> orig = w;
		anode var = iter_var->first;

		C[var] = 0;
		//S[var].push(0);

		while(w.size()){
			set<basic_block_t*>::iterator itdef = w.begin();
			basic_block_t *p = *itdef;
			w.erase(itdef);
			set<basic_block_t*>::iterator itdf = ssa_DF[p].begin();
			for(; itdf != ssa_DF[p].end(); ++itdf){
				basic_block_t *y = *itdf;
				//printf(" var %p df -> %d\n", var, y->index);		
				if (PHI[var].find(y) == PHI[var].end()){
					insert_phi(var, y);
					PHI[var].insert(y);
					if (orig.find(y) == orig.end()){
						//printf("add y \n");
						w.insert(y);
					}

				}
			}
		}
	}


}
/*
 * stmt MUST be GIMPLIFY stmt
 * */
void replace_use_def(anode stmt, C_t &ct, S_t &st){
	if (!stmt)
		return;

        int len = anode_code_length(anode_code(stmt));

        char c = anode_code_class(anode_code(stmt));
        assert(c == 'd' || c == 'e');


        if (len == 0)
            return;

        if (anode_code(stmt) == MODIFY_EXPR){
            //printf("%s\n", anode_code_name(anode_code(ANODE_OPERAND(expr, 0))));
            anode def = get_def2(ANODE_OPERAND(stmt, 0), stmt->decl_outer);
            anode tmp = ANODE_OPERAND(stmt, 0);
//            get_use_def(ANODE_OPERAND(stmt, 1), stmt->decl_outer, var, r_def);

        }else if (EXPR_P(stmt)){
            for (int i = 0; i < len; ++i){
 //               get_use_def(ANODE_OPERAND(stmt, i), stmt->decl_outer, var, r_def);
            }

        }


}

anode_ssa_name *test_ssa = NULL;

void rename(basic_block_t *start){
        printf("begin rename >>> %d\n", start->index);
	/* C & S already init so we do SEARCH-Algorithm */
        map<anode, int> pop_map;
        /* for phi-defs */
        for(anode p = start->phi; p; p = ANODE_CHAIN(p)){
                anode_ssa_name *defname = ((anode_phi*)p)->def_name;
                int i = ++C[defname->var];
                defname->version = i;
                printf("push %p\n", defname->var);
                S[defname->var].push(defname);

                pop_map[defname->var]++;

                //all_ssa_defs[defname->var].push_back(defname);

        }

	for(anode stmt = start->entry; stmt; stmt = ANODE_CHAIN(stmt)){
		printf("rename stmt %p(%d)\n", stmt, start->index);
                live_set_p_t s_use, s_def;
                get_stmt_use_def(ANODE_VALUE(stmt), stmt->decl_outer, s_use, s_def);
                /* for normal(no-phi) use */
                for(auto it : s_use){
                        anode_ssa_name *tname = S[*it].top();
                        *it = tname;
                        use_t t(ANODE_VALUE(stmt), 0, false);
                        t.tmp = it;
                        t.block = start;
                        tname->add_user(t);
                        printf("try rename normal-use %s(%d) -> %d\n",
                                IDENTIFIER_POINTER(decl_name(tname->var)),
                                start->index, (*it)->version);
                }
                /* for normal defs */
                for(auto it : s_def){
                        anode_ssa_name *tname = new anode_ssa_name(*it);
                        tname->def_stmt = ANODE_VALUE(stmt);
                        tname->version = ++C[*it];
                        printf("push %p\n", *it);
                        S[*it].push(tname);
                        pop_map[*it]++;
                        *it = tname;

                        test_ssa = tname;/* for debug */
                        //all_ssa_defs[tname->var].push_back(tname);


                        printf("try rename normal-def %s(%d) -> %d\n",
                                IDENTIFIER_POINTER(decl_name(tname->var)),
                                start->index, (*it)->version);

                }

	}

        /* for-each succ of start */
        for(edge e = start->succ; e; e = e->succ_next){
                /* for each phi in succ(start) */
                basic_block_t *succ_block = e->dst;

                for(anode p = succ_block->phi; p; p = ANODE_CHAIN(p)){
                        anode_phi *v = (anode_phi*)p;
                        char *pname = IDENTIFIER_POINTER(decl_name(v->def_name->var));

                        int j = which_pred_n(start, succ_block);
                        anode_ssa_name *tname = S[((anode_phi*)p)->def_name->var].top();
                        ((anode_phi*)p)->targets[j] = tname;

                        use_t t(p, j, true);
                        t.pred_block = start;
                        tname->add_user(t);

                        printf("try rename phi %s(%d) --> %d\n", 
                                pname, succ_block->index, v->targets[j]->version);

                }

        }
        /* for-each child in D-Tree */
        for(auto it : ssa_D_Tree[start])
                rename(it);

        /* pop stack to recover */
        for(auto it : pop_map){
                for(int i = 0; i < it.second; i++)
                        S[it.first].pop();
        }

        printf("leve rename block %d\n", start->index);
}
/*
*
*   Reconstruction based dominance Frontier
    need:
        1. one weay to 
*    
*/
map<basic_block_t*, list<anode_ssa_name*> > block_defs;
set<basic_block_t*>  all_idfs;

/* 第一次查找的时候, 在u和def同一个block内时 */
anode_ssa_name *find_last_def(basic_block_t *block, anode use_stmt, anode_ssa_name *u){

        anode_ssa_name *pname = NULL;
        /* search defs in phi */
        for(anode p = block->phi; p; p = ANODE_CHAIN(p)){
            anode_ssa_name *defname = ((anode_phi*)p)->def_name;
            if(defname->var == u->var){
                pname = defname;
            }
        }
        /* search defs in all stmt */

        for(anode stmt = block->entry; stmt; stmt = ANODE_CHAIN(stmt)){
            if(ANODE_VALUE(stmt) == use_stmt)
                break;
            live_set_p_t s_use, s_def;
            get_stmt_use_def(ANODE_VALUE(stmt), stmt->decl_outer, s_use, s_def);
            for(auto it : s_def){
                if((*(anode_ssa_name**)it)->var == u->var){
                    pname = *(anode_ssa_name**)it;
                }
            }

        }

        return pname;
}
/*
        当block没有defs的时候，调用
        往D-Tree上溯 或 插入phi节点后 前溯

*/
anode_ssa_name *find_def_from_end(basic_block_t *block, anode_ssa_name *name);

anode_ssa_name *find_def_from_begin(basic_block_t *block, anode_ssa_name *name){
        /* a. get IDF of all dfs of new_ssa 
                (if IDF of some ssa has already been placed, we would't been here at all) 
        */
        anode_ssa_name *d = NULL;
        if(all_idfs.count(block) != 0){
            /* is IDF of some def and no phi-def yet!!! */
            anode_phi *pname = new anode_phi(block);
            pname->def_name = new anode_ssa_name(name->var);
            pname->def_name->def_stmt = pname;
            pname->version = ++C[name->var];

            all_idfs.insert(ssa_DF[block].begin(), ssa_DF[block].end());

            /* append phi to defs */

            for(edge e = block->pred; e; e = e->pred_next){
                anode_ssa_name *o = find_def_from_end(e->src, name);
                /* fill the op of PHI */
                pname->targets.push_back(o);
            }



        }else{
            d = find_def_from_end(ssa_IDOM[block], name);
        }

        return d;

}
/*
        在当前节点查找，没找到就回溯(上溯或前溯)
*/
anode_ssa_name *find_def_from_end(basic_block_t *block, anode_ssa_name *name){
        //if(!block_defs[block].empty())
        //        return block_defs[block].front();
        anode_ssa_name *d = find_last_def(block, NULL, name);
        if(d)
            return d;
        return find_def_from_begin(block, name);
}
void for_all_use(anode_ssa_name *old_ssa, anode_ssa_name *new_ssa){
     /* for every use of old_ssa, even not on the path of new_ssa */
	 for(auto u: old_ssa->ulist){
		anode_ssa_name *d = NULL;
        basic_block_t *b = NULL;

		if(u.is_phi){
                anode_phi *pname = (anode_phi*)(u.expr);
                b = u.block; /* pred block */
                d = find_def_from_end(b, new_ssa);
		}else{
		    d = find_last_def(u.block, u.expr, new_ssa);

            if(d == NULL)
                d = find_def_from_begin(b, new_ssa);
    		
        }

        if(u.is_phi){
                ((anode_phi*)u.expr)->targets[u.op_index] = d;
        }else{
                *(anode_ssa_name **)u.tmp = d;
        }

    }
}
void rebuild(anode_ssa_name *dup_ssa, anode_ssa_name *new_ssa){
        // block_defs.clear();
        all_idfs.clear();
        basic_block_t *block = new_ssa->def_stmt->basic_block;
        all_idfs = ssa_DF[block];
        all_idfs.insert(ssa_DF[block].begin(),ssa_DF[block].end());

        /** calc the IDF **/
        int last = all_idfs.size();
        do{
            set<basic_block_t*> tmp;
            for(auto it : all_idfs){
                tmp.insert(ssa_DF[it].begin(), ssa_DF[it].end());
            }

            all_idfs.insert(tmp.begin(), tmp.end());

        }while(all_idfs.size() !=  last);
        
}

/*
    three-address code linear
*/
void code_3(basic_block_t *block){
    block->ins.clear();
    for(anode stmt = block->entry; stmt; stmt = ANODE_CHAIN(stmt)){

    }
}

void build_ssa_legacy(basic_block_t *start){
        anode a;
        cout<<"undefine_variable is "<<&undefine_variable<<endl;
        printf("Dumping DF...\n");
        for(ssa_DF_t::iterator iter = ssa_DF.begin(); iter != ssa_DF.end(); ++iter){
    		cout<<"node "<<iter->first->index<<endl;
    		for(set<basic_block_t*>::iterator it = iter->second.begin(); it != iter->second.end(); ++it){
    			cout<<"\t-> "<<(*it)->index<<endl;
    		}

        }
        fill_the_defsite(start);
        place_phi(start);
        printf("Dump D-Tree...\n");
        for(auto it: ssa_D_Tree){
                printf("block %d:\t", it.first->index);
                for(auto it2: it.second){
                        printf(" %d", it2->index);
                }
                printf("\n");
        }


        rename(start);
        rebuild(test_ssa, test_ssa);
        for(basic_block_t *t = start; t != EXIT_BLOCK_PTR; t = t->next){
                if (S[a].empty())
                        printf("dd\n");
                cout<<"hello "<<S[a].size()<<std::endl;
        }
        for(basic_block_t *t = start; t != EXIT_BLOCK_PTR; t = t->next){
                code_3(t);
        }

}
