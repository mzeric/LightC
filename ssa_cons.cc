#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "anode.h"

using namespace std;
struct basic_block_s entry_exit_blocks[2];

static unsigned global_bb_index = 0;
static unsigned global_label_index = 0;

struct basic_block_s *current_bb;

anode current_decl_context = NULL;

anode_node undefine_variable;
anode_node uninitial_variable;

typedef map<anode, anode> ssa_table_t;
typedef map<anode, anode> ssa_unsealed_var_t;
typedef map<basic_block_t*, ssa_unsealed_var_t > ssa_unsealed_t;
ssa_unsealed_t unsealed_block;

enum fill_ttype{
	BB_UNFILLED = 0,
	BB_FILLED = 1,
	BB_SEALED = 2,
};

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
        if ((e = get_edge(src, dst))){
            e->flag = flag;
            return e;
    	}
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
edge redirect_edge(edge re, bb new_dst){
	//for (edge e = re; e; e = e->)
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

	b->entry = chain_cat(b->entry, last);
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
	b->ssa_table = NULL;
	b->status = BB_UNFILLED;
	b->phi = NULL;


	return b;
}

basic_block_t *bb_fork_after(anode *first, anode *after, basic_block_t *prev_bb){
	//bb_split_after(first, after);
	return new_basic_block(*first, prev_bb, NULL);
}

void bb_insert_last(basic_block_t *b, anode stmt){
	bb_add_stmt(b, stmt);

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
	anode ir_br = build_stmt(IR_BRANCH, if_cond, NULL, NULL);
	anode ir_br_list = build_list(NULL, ir_br);
	if_bb = build_cfg(ir_br, list, before, "if_cond");

//	bb_insert_last(if_bb, ir_br);
	//FIXME shall we need the then_entry empty-bb
	then_bb = build_cfg(then_stmt, if_bb, if_bb, "then ");
	if_bb->succ->flag = EDGE_TRUE;

	else_bb = then_bb;
	if(else_stmt){
		else_bb = build_cfg(else_stmt, then_bb, if_bb, "else ");
		if_bb->succ->flag = EDGE_FALSE;

	}
	if_exit = new_basic_block(NULL, else_bb, "if_exit");

	make_edge(then_bb, if_exit, EDGE_PASSTHOUGH);
	if(else_stmt)
		make_edge(else_bb, if_exit, EDGE_PASSTHOUGH);
	else
		make_edge(if_bb, if_exit, EDGE_FALSE);

	return if_exit;

}
basic_block_t *build_compound_cfg(anode cp_stmt, basic_block_t *list, basic_block_t *before){
	printf("build_compound_cfg\n");
	anode s = COMPOUND_BODY(cp_stmt);
	/*
		prepare the { push_decl_space
	*/

	anode old_c = COMPOUND_DS_OUTER((anode_expr*)cp_stmt);
	current_decl_context = old_c;

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
void dump_bb_var(basic_block_t *b){
	printf("dump: %d ssa_table = %x\n", b->index, b->ssa_table);
	if(b->ssa_table){
		map<anode, anode>::iterator iter;
		for (iter = b->ssa_table->begin(); iter != b->ssa_table->end(); ++iter){
			if(iter->first == &undefine_variable){
				printf("undefine_variable\n");
				continue;
			}
			printf("[%s](%x) 0x%x\n",
				IDENTIFIER_POINTER(decl_name(iter->first)),
				iter->first,
				iter->second);
		}
	}

	//for ()
}
void dump_bb(basic_block_t *start_bb){
	basic_block_t *b;
	for(b = start_bb; b; b = b->next){
		int i = 0;
		printf("block %u %s %x\n", b->index, b->comment, b->decl_context);
		map<anode, anode>::iterator iter;
		if(b->ssa_table){
			printf("has use-def table status %d\n", b->status);
			for(iter = (*b->ssa_table).begin(); iter != (*b->ssa_table).end(); ++iter){
				if(iter->first == &undefine_variable){
					printf("undefine_variable\n");
					continue;
				}
				if (iter->first == &uninitial_variable){
					printf("uninitial_variable\n");
					continue;
				}
				printf("[%s](%x) 0x%x\n",
					IDENTIFIER_POINTER(decl_name(iter->first)),
					iter->first,
					iter->second);
			}
		}
		printf("phi:\n");
		for (anode p = b->phi; p; p = p->chain){
			printf("\tphi %x :\t", p);
			for (anode t = ((anode_phi*)p)->targets; t; t = t->chain){
				printf(" %x", ANODE_VALUE(t));
			}
			printf("\n");
		}
		printf("pred:\t");
		for (edge e = b->pred; e; e = e->pred_next){
			printf(" %d(%d)", e->src->index, e->flag);
		}
		printf("\n");

		printf("succ:\t");
		for (edge e = b->succ; e; e = e->succ_next){
			printf(" %d(%d)", e->dst->index, e->flag);
		}
		printf("\n");
		for (anode t = b->entry; t; t = ANODE_CHAIN(t)){
			i++;
			printf("\tstmt >> %s\n", anode_code_name(anode_code(ANODE_VALUE(t))));
		}
		printf("\t\t all %d stmts\n", i);
		
	}
}
void dump_edges(basic_block_t *start_bb){

}
void elimate_empty_join(basic_block_t *b){
	/* is the only pred for succ */
	int i = 0;
	edge succ = b->succ;
	for (edge e = b->pred; e; e = e->pred_next)
		if (e->flag == EDGE_NO_MERGE)
			return;
	if (succ == NULL)
		return;
	basic_block_t *b_succ = b->succ->dst;
	edge pred = b_succ->pred;
	if (succ->succ_next == NULL && pred->pred_next == NULL){
		assert(pred == succ);
		/* ok let's do it */
		printf("elimate %d\n", b->index);
		b_succ->pred = b->pred;
		for (edge e = b->pred; e; e = e->pred_next){
			e->dst = b_succ;
		}
		/* delete from the list */
		bb prev = b->prev;
		if(b->prev)
			b->prev->next = b_succ;
		b_succ->prev = b->prev;

		/* retarget goto index table */
	}

}
void simplify_bb(basic_block_t *b){
	for (basic_block_t *t = b; t; t = t->next){
		if (t->entry == NULL)
			elimate_empty_join(t);
	}
}
/*
	SSA Construction
*/
anode _get_def(const char* name, bb b){
	int i = 0;
	for (anode t = b->decl_current; t; t = ANODE_CHAIN(t)){
		anode d = decl_name(ANODE_VALUE(t));
		if(strcmp(IDENTIFIER_POINTER(d), name) == 0)
			return ANODE_VALUE(t);
		i++;
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
	printf("write ssa table bb:%d, 0x%s, 0x%x\n",
		b->index, 
		IDENTIFIER_POINTER(decl_name(name)),
		value);
	if(b->ssa_table == NULL)
		b->ssa_table = new map<anode, anode>();
	(*b->ssa_table)[name] = value;
}
/* make sure that id is anode_decl not anode_identifier */
anode read_variable(anode id, basic_block_t *b);
anode add_phi_operands(anode, anode&);
bool is_pred_ok(basic_block_t *b, int flag){
	for (edge e = b->pred; e; e = e->pred_next)
		if (!(e->src->status & flag))
			return false;
	return true;
}
anode read_var_recursive(anode id, basic_block_t *b){
	anode val;
	int i = 0;
	
	if(!is_pred_ok (b, BB_FILLED)){
		/* not all filled */
		val = new_phi(b);
		unsealed_block[b][id] = val;
		return val;
	}
	for(edge e = b->pred; e; e = e->pred_next){
		i++;
	}
	if(i == 0)/* dumm entry_block_ptr */
		return &undefine_variable;
	if(i == 1)
		val = read_variable(id, b->pred->src);
	else{

		val = new_phi(b);
		write_variable(id, val, b);
		val = add_phi_operands(id, val);

	}
	if(val)
		write_variable(id, val, b);
	assert(val);
	return val;
}
anode read_variable(anode id, basic_block_t *b){
	if(!b)
		return NULL;
	assert(anode_code_class(anode_code(id)) == 'd');
	if (b->ssa_table && (*b->ssa_table)[id])
		return (*b->ssa_table)[id];

	return read_var_recursive(id, b);
}

/* trivial when operands consist of only one non-phi value */
anode remove_trivial_phi(anode p){
	anode_phi *phi = (anode_phi*)p;
	anode same = NULL;
	for (anode t = phi->targets; t; t = ANODE_CHAIN(t)){
		anode v = ANODE_VALUE(t);
		if (v == same || v == phi)
			continue;
		if (same != NULL)
			return phi;
		same = v;
	}
	if (same == NULL)
		same = &undefine_variable;
	
	phi->replace_by(same);
	for (set<anode*>::iterator iter = phi->users->begin(); iter != phi->users->end(); ++iter){
		if (**iter == phi)
			continue;
		if (anode_code(**iter) == IR_PHI)
			remove_trivial_phi(**iter);
	}
	return same;

}
anode simplify_phi(anode_phi *phi){
	assert(anode_code(phi) == IR_PHI);
	//return phi;
	return remove_trivial_phi(phi);
}

anode add_phi_operands(anode id, anode& phi_node){

	anode_phi *phi = (anode_phi*)phi_node ;
	basic_block_t *b = ANODE_CHECK(phi, IR_PHI)->block;

	for(edge e = b->pred; e; e = e->pred_next){
		basic_block_t *b = e->src;
		if(!(b->status & BB_FILLED)){

			assert("some block missing filled"&&0);
		}

		anode tt = read_variable(id, b);
		//printf("add phi \n");
		add_phi_user(tt, &phi_node);
		phi->append_operand(tt);
	}
	return simplify_phi(phi);
}

anode rewrite_operand(anode expr, basic_block_t*b){

	int length = anode_code_length(anode_code(expr));

	if(length == 0){
		/* identifier or cst*/
		if (anode_code_class(anode_code(expr)) == 'c')
			return expr;
		if (anode_code(expr) == IDENTIFIER_NODE){
			return read_variable(get_def(expr, b), b);
		}
	}
	for(int i = 0; i < length; i++){

		anode op = ANODE_OPERAND(expr, i);
		printf("rewrite, %s\n", anode_code_name(anode_code(op)));
		if(anode_code(op) == IDENTIFIER_NODE){
			anode u = read_variable(get_def(op, b), b);
			add_phi_user(u, ANODE_OPERAND_P(expr, i));
			ANODE_OPERAND(expr, i) = u;
		}else if(anode_code_class(anode_code(op)) == 'c'){
			continue;
		}else if(anode_code_class(anode_code(op)) == 'e'){
			anode u = rewrite_operand(op, b);
			add_phi_user(u, ANODE_OPERAND_P(expr, i));
			ANODE_OPERAND(expr, i) = u;
		}
	}
	return expr;

}
/*
	1. if all pred is filled ,try 
			a. seal all phi of current
			b. mark it as sealed , then all fill action below could search up to pred
	2. some pred is not filled yet (been parse yet)
		mark the phi for unknown , will up-search it later
	3. seal at known-point, or just do iterator-seal at end
*/
void try_seal(basic_block_t *b){
	/* only all pred filled could try to seal it */
	map<anode, anode>::iterator iter;
	if (!b)
		return;
	if (!is_pred_ok(b, BB_FILLED))
		return;

	for (iter = unsealed_block[b].begin(); iter != unsealed_block[b].end(); ++iter){
		add_phi_operands(iter->first, iter->second);

	}
	unsealed_block.erase(b);
	b->status |= BB_SEALED;
}
void seal_all_last(basic_block_t *b){
	for (basic_block_t *t = b; t; t = t->next){
		try_seal(t);
	}
}
void fill_bb(basic_block_t *b){

	b->decl_current = b->decl_context;
	try_seal(b);
	printf("fill the %d\n", b->index);
	if (b->status & BB_FILLED)
		return;
	for (anode t = b->entry; t; t = ANODE_CHAIN(t)){
		anode stmt = ANODE_VALUE(t);
		if(anode_code_class(anode_code(stmt)) == 'd'){
			anode_decl *d = ANODE_(ANODE_CLASS_CHECK(stmt, 'd'), anode_decl);
		        b->decl_current = anode_cons(NULL, d, b->decl_current);
		        write_variable(get_def(decl_name(stmt), b), &uninitial_variable, b);
		        printf("fill one decl %x\n", d);
		}else if(anode_code_class(anode_code(stmt)) == 'e'){
			switch(anode_code(stmt)){
				case MODIFY_EXPR:
					printf("get MODIFY_EXPR\n");
					anode tt = rewrite_operand(ANODE_OPERAND(stmt, 1), b);
					add_phi_user(tt, ANODE_OPERAND_P(stmt, 1));
					ANODE_OPERAND(stmt, 1) = tt;
					anode id = get_def(ANODE_OPERAND(stmt, 0), b);
					write_variable(id, ANODE_OPERAND(stmt, 1), b);
					break;
			}
		}

	}
	b->status |= BB_FILLED;

}

/**/

void get_phi_use(basic_block_t *b){
	for (basic_block_t *t = b; t; t = t->next){
		for (ssa_table_t::iterator iter = b->ssa_table->begin();
			iter != b->ssa_table->end(); ++iter){
			if (anode_code(iter->second) == IR_PHI){

			}
		}
	}
}

void build_ssa(basic_block_t *b){
	for(basic_block_t *t = b; t; t = t->next){
		fill_bb(t);
	}
	seal_all_last(b);
	printf("undefine_variable %x\n", &undefine_variable);

}