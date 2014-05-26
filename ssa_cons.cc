#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "anode.h"

using namespace std;
struct basic_block_s entry_exit_blocks[2];

static unsigned global_bb_index = 0;

struct basic_block_s *current_bb;

anode current_decl_context = NULL;

anode_node undefine_variable;

map<string, pair<anode, anode> > t;

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
	    case COMPOUND_STMT:
            	return 1;
        }
        return 0;
}

void current_def(anode decl){

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
        if(!src || !dst)
        	return NULL;
        if ((e = get_edge(src, dst)))
            return e;
    	printf("link %d -> %d\n", src->index, dst->index);
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
void bb_add_stmt(basic_block_t *b, anode s){
	anode last = build_list(NULL, s);

	b->entry = anode_cat(b->exit, last);
	b->exit = last;
	s->basic_block = b;
}
/*
	head is the first stmt of bb
*/
basic_block_t *new_basic_block(anode head, bb ahead, const char *comment){
	printf("new_basic_block  %s after %u:%s\n", comment, ahead->index, ahead->comment);
	bb b = (bb)malloc(sizeof(*b));
	memset(b, 0, sizeof(basic_block_t));
	if(head){
		b->entry = build_list(NULL, head);
		head->basic_block = b;
	}
	b->exit = b->entry;
	if(ahead)
		ahead->next = b;
	b->prev = ahead;
	if(comment)
	b->comment = strdup(comment);
	b->index = global_bb_index++;
	b->decl_context = current_decl_context;
	b->decl_current = b->decl_context;
	b->ssa_table = new map<anode, anode>;
	b->filled = 0;
	b->phi = NULL;


	return b;
}

basic_block_t *bb_fork_after(anode *first, anode *after, basic_block_t *prev_bb){
	//bb_split_after(first, after);
	return new_basic_block(*first, prev_bb, NULL);
}
/*
	parm must be the function body
	lower the if_stmt to cond as cfg
	the return bb should be the exit_bb
*/

basic_block_t *build_if_cfg(anode if_stmt, basic_block_t *list, basic_block_t *before){
	bb if_bb, then_bb, else_bb, if_exit;

	anode if_cond = IF_COND(if_stmt);
	anode then_stmt = THEN_CLAUSE(if_stmt);
	anode else_stmt = ELSE_CLAUSE(if_stmt);

	if_bb = build_cfg(if_cond, list, before, "if_cond");
	//FIXME shall we need the then_entry empty-bb
	then_bb = build_cfg(then_stmt, if_bb, if_bb, "then ");

	else_bb = then_bb;
	if(else_stmt){
		else_bb = build_cfg(else_stmt, then_bb, if_bb, "else ");

	}
	if_exit = new_basic_block(NULL, else_bb, "if_exit");

	make_edge(then_bb, if_exit, 0);
	if(else_stmt)
		make_edge(else_bb, if_exit, 0);
	else
		make_edge(if_bb, if_exit, 0);

	return if_exit;

}
basic_block_t *build_compound_cfg(anode cp_stmt, basic_block_t *list, basic_block_t *before){
	printf("build_compound_cfg\n");
	anode s = COMPOUND_BODY(cp_stmt);
	/*
		prepare the { push_decl_space
	*/
	anode old_c = current_decl_context;
	current_decl_context = COMPOUND_DS_OUTER((anode_expr*)cp_stmt);
	basic_block_t *b = build_cfg(s, list, before, "compound");
	current_decl_context = old_c;

	/*
		restore the outer decl_space
	*/

	printf("end build_compound_cfg %u\n", b->index);

	return b;
}
basic_block_t *build_while_cfg(anode w_stmt, basic_block_t *list, basic_block_t *before){

	return NULL;
}
basic_block_t *build_for_cfg(anode for_stmt, basic_block_t *list, basic_block_t *before){
	return NULL;
}
basic_block_t *build_cfg(anode start_stmt, basic_block_t *list, basic_block_t *before,
			const char* comment){
	
	int need_new = 0;

	basic_block_t *h = new_basic_block(NULL, list, comment);
	basic_block_t *b = h;
	anode stmt = start_stmt;
	anode prev_stmt = NULL;
	if(start_stmt)
		printf("build_cfg begin :%s\n", anode_code_name(anode_code(start_stmt)));
	make_edge(before, h, 0);
	before = h;
	
	for(stmt = start_stmt; stmt; stmt = ANODE_CHAIN(stmt)){

		if(need_new){
			
			bb dst = new_basic_block(NULL, b, NULL);
			make_edge(b, dst, 0);
			b = dst;
			need_new = 0;
		}

		prev_stmt = stmt;
		if(is_branch(stmt)){
			switch(anode_code(stmt)){
				case IF_STMT:
					b = build_if_cfg(stmt, b, b);/* return if_exit */
					break;
				case WHILE_STMT:
					break;
				case FOR_STMT:
					break;
				case COMPOUND_STMT:
					b = build_compound_cfg(stmt, b, b);

				default:;
			}
			need_new = 1;/* seperate xx_exit empty bb */
			printf("get branch %s\n", anode_code_name(anode_code(stmt)));
			continue;
		}else{
			bb_add_stmt(b, stmt);
			printf("add stmt %s\n", anode_code_name(anode_code(stmt)));
		}


	}

	return b;	
}
void build_edges(basic_block_t *start_bb){

}
basic_block_t *start_func_bb(anode body){
	basic_block_t *start = build_cfg(body, NULL, NULL, NULL);
	build_edges(start);
}
void dump_bb(basic_block_t *start_bb){
	basic_block_t *b;
	for(b = start_bb; b; b = b->next){
		int i = 0;
		printf("block %u %s %x\n", b->index, b->comment, b->decl_context);
		map<anode, anode>::iterator iter;
		if(b->ssa_table){
			printf("has use-def table filled %d\n", b->filled);
			for(iter = (*b->ssa_table).begin(); iter != (*b->ssa_table).end(); ++iter){
				if(iter->first == &undefine_variable){
					printf("undefine_variable\n");
					continue;
				}
				printf("[%s] 0x%x\n",
					IDENTIFIER_POINTER(decl_name(iter->first)),
					iter->first);
			}
		}
		printf("phi:\n");
		for (anode p = b->phi; p; p = p->chain){
			printf("\tphi %x\n", p);
		}

		for (anode t = b->entry; t; t = ANODE_CHAIN(t)){
			i++;
			printf("\tstmt >> %s\n", anode_code_name(anode_code(ANODE_VALUE(t))));
		}
		printf("\t\t all %d stmts\n", i);
		
	}
}
void dump_edges(basic_block_t *start_bb){

}

anode _get_def(const char* name, bb b){

	for (anode t = b->decl_current; t; t = ANODE_CHAIN(t)){
		anode d = decl_name(ANODE_VALUE(t));

		if(strcmp(IDENTIFIER_POINTER(d), name) == 0)
			return ANODE_VALUE(t);
	}
	return &undefine_variable;
}
anode get_def(anode id, bb b){
	return _get_def(IDENTIFIER_POINTER(id), b);
}
anode new_phi(basic_block_t *b){
	anode_phi *p = new anode_phi(b);
	return p;
}
anode write_variable(anode name, anode value, basic_block_t *b){
	(*b->ssa_table)[name] = value;
}
/* make sure that id is anode_decl not anode_identifier */
anode read_variable(anode id, basic_block_t *b);
anode add_phi_operands(anode, anode);
anode read_var_recursive(anode id, basic_block_t *b){
	anode val;
	int i = 0;
	for(edge e = b->pred; e; e = e->pred_next){
		i++;
	}
	if(i == 1)
		val = read_variable(id, b->pred->src);
	else{
		exit(1);
		val = new_phi(b);
		write_variable(id, val, b);
		val = add_phi_operands(id, val);
	}
	write_variable(id, val, b);
	return val;
}
anode read_variable(anode id, basic_block_t *b){
	assert(anode_code_class(anode_code(id)) == 'd');
	if ((*b->ssa_table)[id])
		return (*b->ssa_table)[id];
	return read_var_recursive(id, b);
}
anode simplify_phi(anode_phi *phi){
	assert(anode_code(phi) == IR_PHI);
	return phi;
}
anode add_phi_operands(anode id, anode phi_node){
	printf("trigger phi\n");
	anode_phi *phi = (anode_phi*)phi_node ;
	basic_block_t *b = ANODE_CHECK(phi, IR_PHI)->block;
	for(edge e = b->pred; e; e = e->pred_next){
		basic_block_t *b = e->src;
		phi->append_operand(read_variable(id, b));
	}
	return simplify_phi(phi);
}
void rewrite_operand(anode expr, basic_block_t*b){
	int length = anode_code_length(anode_code(expr));
	for(int i = 0; i < length; i++){
		anode op = ANODE_OPERAND(expr, i);
		if(anode_code(op) == IDENTIFIER_NODE){
			ANODE_OPERAND(expr, i) = read_variable(get_def(op, b), b);
		}else if(anode_code_class(anode_code(op)) == 'c'){
			continue;
		}else if(anode_code_class(anode_code(op)) == 'e'){
			rewrite_operand(op, b);
		}
	}

}
void fill_bb(basic_block_t *b){
	b->decl_current = b->decl_context;
	if(b->filled)
		return;
	for (anode t = b->entry; t; t = ANODE_CHAIN(t)){
		anode stmt = ANODE_VALUE(t);
		if(anode_code_class(anode_code(stmt)) == 'd'){
			anode_decl *d = ANODE_(ANODE_CLASS_CHECK(stmt, 'd'), anode_decl);
		        b->decl_current = anode_cons(NULL, d, b->decl_current);
		}else if(anode_code_class(anode_code(stmt)) == 'e'){
			switch(anode_code(stmt)){
				case MODIFY_EXPR:
					rewrite_operand(ANODE_OPERAND(stmt, 1), b);
					anode id = get_def(ANODE_OPERAND(stmt, 0), b);
					write_variable(id, ANODE_OPERAND(stmt, 1), b);
					break;
			}
		}

	}
	b->filled = 1;
}
void build_ssa(basic_block_t *b){
	for(basic_block_t *t = b; t; t = t->next){
		fill_bb(t);
	}
}