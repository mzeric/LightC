#include <stdio.h>
#include <vector>
#include <map>
#include "anode.h"

using namespace std;
struct basic_block_s entry_exit_blocks[2];

struct basic_block_s *current_bb;
anode bb_node = NULL;
void push_block_note(){
	anode_list *l = new anode_list();
	ANODE_VALUE(l) = bb_node;
	current_declspaces = anode_cons(NULL, l, current_declspaces);
}
void push_ssa_decl(anode id, anode value){

      ANODE_CHECK(id, IDENTIFIER_NODE);
      current_declspaces = anode_cons(value, id, current_declspaces);


}
void  ssa_write(anode id, anode value){
      push_ssa_decl(id, value);
}
anode ssa_read(const char *name){
      anode t = lookup_name_current_bb(name);
      if(t)
      	    return t;

}
anode function_body(anode f){
		return NULL;
}
int is_branch(anode t){
        switch(anode_code(t)){
            case IF_STMT:
            case WHILE_STMT:
	    case FOR_STMT:
            	return 1;
        }
        return 0;
}

void current_def(anode decl){

}
void bb_add_stmt(bb b, anode s){
	s->basic_block = b;
}
bb new_basic_block(anode head, anode end, bb ahead){
	bb b = (bb)malloc(sizeof(*b));
	b->head = head;
	b->exit = end;
	ahead->next = b;
	b->prev = ahead;
}
/*
	cut the body's chain before s
	end the basic_block_t previous
*/
void bb_split_after(anode *first, anode *after){
	if(after == NULL)
		return;
	if(*after)
		(*after)->chain = NULL;
	(*first)->basic_block->exit = *after;
	if(*after)
		*first = (*after)? (*after)->chain:NULL;
}
basic_block_t *bb_fork_after(anode *first, anode *after, basic_block_t *prev_bb){
	bb_split_after(first, after);
	return new_basic_block(*first, NULL, prev_bb);
}
/*

*/

/* basic block cfg */
edge get_edge(bb src, bb dst){
        edge e;
        for (e = src->succ; e; e = e->succ_next){
            if (e->dst == dst){
                return e;
            }
        }
        return NULL;
}
edge make_edge(bb src, bb dst, int flag){

        edge e;
        if ((e = get_edge(src, dst)))
            return e;

        e = (edge)malloc(sizeof(edge_t));
        e->succ_next = src->succ;
        e->pred_next = dst->pred;

        e->src = src;
        e->dst = dst;
        e->flag = flag;

        src->succ = e;
        dst->pred = e;

        return e;

}
void free_edge(edge e){
        free(e);
}
void remove_edge(edge e){

}
void dump_edge_info(edge e){

}
/*
	parm must be the function body
	lower the if_stmt to cond as cfg
*/
basic_block_t *build_cfg(anode body);
anode build_if_cfg(anode if_stmt){

	anode if_cond = IF_COND(if_stmt);
	anode then_stmt = THEN_CLAUSE(if_stmt);
	anode else_stmt = ELSE_CLAUSE(if_stmt);

	bb if_bb = build_cfg(if_cond);
	bb then_bb = build_cfg(then_stmt);
	bb else_bb = build_cfg(else_stmt);



}
basic_block_t *build_cfg(anode body){
	
	int need_new = 1;
	int first = 1;
	basic_block_t *b = ENTRY_BLOCK_PTR;
	anode stmt = body;
	anode prev_stmt = NULL;
	while(stmt){

		if(need_new){
			
			if(!first){
				//bb_split_before(&body, &stmt);
				bb_split_after(&body, &prev_stmt);

			}
			b = new_basic_block(body, NULL, b);
			need_new = 0;
		}

		bb_add_stmt(b, stmt);
		
		if(is_branch(stmt)){
			switch(anode_code(stmt)){
				case IF_STMT:
					build_if_cfg(stmt);
					bb_split_after(&body, &prev_stmt);
					b = new_basic_block(body, NULL, b);
					bb_add_stmt(b, stmt);
					break;
				case WHILE_STMT:
					break;
				case FOR_STMT:
					break;
				default:;
			}
			need_new = 1;
		}

		prev_stmt = stmt;
		stmt = ANODE_CHAIN(stmt);
		first = 0;
	}
	return b;
	
}
