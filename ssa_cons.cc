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
void bb_add_stmt(basic_block_t *b, anode s){
	anode last = build_list(NULL, s);

	b->entry = anode_cat(b->exit, last);
	b->exit = last;
	s->basic_block = b;
}
/*
	head is the first stmt of bb
*/
basic_block_t *new_basic_block(anode head, bb ahead){
	bb b = (bb)malloc(sizeof(*b));
	memset(b, 0, sizeof(*b));
	if(head){
		b->entry = build_list(NULL, head);
		head->basic_block = b;
	}
	b->exit = b->entry;
	if(ahead)
		ahead->next = b;
	b->prev = ahead;
}

basic_block_t *bb_fork_after(anode *first, anode *after, basic_block_t *prev_bb){
	//bb_split_after(first, after);
	return new_basic_block(*first, prev_bb);
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
basic_block_t *build_if_cfg(anode if_stmt, basic_block_t *ahead){

	anode if_cond = IF_COND(if_stmt);
	anode then_stmt = THEN_CLAUSE(if_stmt);
	anode else_stmt = ELSE_CLAUSE(if_stmt);

	bb if_bb = build_cfg(if_cond);
	bb then_bb = build_cfg(then_stmt);
	bb else_bb = build_cfg(else_stmt);
	bb if_exit;


	return if_exit;

}
basic_block_t *build_cfg(anode body){
	
	int need_new = 0;

	basic_block_t *h = new_basic_block(NULL, NULL);
	basic_block_t *b = h;
	anode stmt = body;
	anode prev_stmt = NULL;
	while(stmt){

		if(need_new){
			
			b = new_basic_block(NULL, b);
			need_new = 0;
		}

		bb_add_stmt(b, stmt);
		
		if(is_branch(stmt)){
			switch(anode_code(stmt)){
				case IF_STMT:
					b = build_if_cfg(stmt, b);/* return if_exit */

					break;
				case WHILE_STMT:
					break;
				case FOR_STMT:
					break;
				default:;
			}
			need_new = 1;/* seperate xx_exit empty bb */
		}

		prev_stmt = stmt;
		stmt = ANODE_CHAIN(stmt);
	}
	return h;
	
}
