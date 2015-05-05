#include "anode.h"
#include "dfa.h"

#include <list>
#include <vector>
#include <map>
#include <set>
#include <iostream>

using namespace std;

map<anode, set<basic_block_t*> > var_defsite; /* vars & defsites */
map<anode, int> C;/* Count[var] */
map<anode, vector<int> > S; /* stack of var = S[var] */

anode get_def2(anode id, anode decl){
	for (anode t = decl; t; t = ANODE_CHAIN(t)){
		anode d = decl_name(ANODE_VALUE(t));
		const char *name = IDENTIFIER_POINTER(id);

		if (strcmp(IDENTIFIER_POINTER(d), name) == 0)
			return ANODE_VALUE(t);
	}

	return &undefine_variable;

}
void get_use_def(anode expr, anode decl, live_set_t &var, live_set_t &r_def){
        /* some expr in cfg left NULL intendely as ir_branch(cond, NULL, NULL) */
        if (!expr){

            return;
        }

        if (expr == &undefine_variable){
            printf("\tundefine_variable\n");
            return;
        }

        //printf("\tGET_ALL_VAR %s\n", anode_code_name(anode_code(expr)));
        if (anode_code(expr) == IDENTIFIER_NODE){
                anode d = get_def2(expr, decl);
                var.insert(d);
                printf("id: %s\n",IDENTIFIER_POINTER(decl_name(expr)));
                cout<<"id use "<<d<<endl;

        }

        int len = anode_code_length(anode_code(expr));

        if (anode_code_class(anode_code(expr)) == 'c'){
            return;
        }


        if (len == 0)
            return;
        if (anode_code(expr) == MODIFY_EXPR){
            //printf("%s\n", anode_code_name(anode_code(ANODE_OPERAND(expr, 0))));
            r_def.insert(get_def2(ANODE_OPERAND(expr, 0), expr->decl_outer));
            anode tmp = ANODE_OPERAND(expr, 0);
            cout<<"def "<<tmp<<" "<<IDENTIFIER_POINTER(decl_name(tmp))<<" "<<get_def2(tmp, expr->decl_outer)<<endl;
            get_use_def(ANODE_OPERAND(expr, 1), expr->decl_outer, var, r_def);

        }else if (EXPR_P(expr)){
            for (int i = 0; i < len; ++i){
                get_use_def(ANODE_OPERAND(expr, i), expr->decl_outer, var, r_def);
            }

        }


}

#define GET_USE_DEF get_use_def


void fill_the_defsite(basic_block_t *start){
        for(basic_block_t *t = start; t != EXIT_BLOCK_PTR; t = t->next){
                for(anode stmt_list = t->entry; stmt_list; stmt_list = ANODE_CHAIN(stmt_list)){
                        anode stmt = ANODE_VALUE(stmt_list);

                        set<anode> use, def;
                        GET_USE_DEF(stmt, stmt->decl_outer, use, def);
                        cout<<"fill_the_defsite in "<<t->index<<" use "<<use.size()<<" "<<def.size()<<endl;
                }
        }
}

void rename_block(basic_block_t *b){

}

void build_ssa_legacy(basic_block_t *start){
        anode a;
        cout<<"undefine_variable is "<<&undefine_variable<<endl;
        fill_the_defsite(start);
        for(basic_block_t *t = start; t != EXIT_BLOCK_PTR; t = t->next){
                if (S[a].empty())
                        printf("dd\n");
                cout<<"hello "<<S[a].size()<<std::endl;
        }
}
