#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "dfa.h"
#include "anode.h"

using namespace std;
struct basic_block_s entry_exit_blocks[2];

static unsigned global_bb_index = 1;
static unsigned global_label_index = 0;

struct basic_block_s *current_bb;

anode current_decl_context = NULL;

anode_node undefine_variable;
anode_node uninitial_variable;

anode_ssa_name ssa_undef_variable;
anode_ssa_name ssa_unini_variable;

typedef map<anode, anode> ssa_unsealed_var_t;
typedef map<basic_block_t*, ssa_unsealed_var_t > ssa_unsealed_t;
ssa_unsealed_t unsealed_block;

enum fill_ttype{
	BB_UNFILLED = 0,
	BB_FILLED = 1,
	BB_SEALED = 2,
};


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
	return NULL;
}
void free_edge(edge e){
        free(e);
}

void bb_add_stmt(basic_block_t *b, anode s){
	anode last = build_list(NULL, s);

	b->entry = chain_cat(b->entry, last);
	//b->exit = last;
	s->basic_block = b;
}
/*
	head is the first stmt of bb
*/
void init_bb(basic_block_t*b, basic_block_t *ahead){
	//b->exit = b->entry;
	if(ahead)
		ahead->next = b;
	b->prev = ahead;
	b->index = global_bb_index++;
	b->decl_context = current_decl_context;
	b->decl_current = b->decl_context;
	b->ssa_table = new ssa_table_t();
	b->status = BB_UNFILLED;
	b->phi = NULL;
	b->live = new live_ness_t();
	b->stmt_live = new map<anode, live_set_t>();
	b->phi_edge = new phi_edge_t();
}
basic_block_t *new_basic_block(anode head, bb ahead, const char *comment){
	printf("new_basic_block  %s after %u:%s\n", comment, ahead->index, ahead->comment);
	bb b = (bb)malloc(sizeof(*b));
	memset(b, 0, sizeof(basic_block_t));
	if(head){
		b->entry = build_list(NULL, head);
		head->basic_block = b;
	}
	
	if(comment)
		b->comment = strdup(comment);
	
	init_bb(b, ahead);

	return b;
}

basic_block_t *bb_fork_after(anode *first, anode *after, basic_block_t *prev_bb){
	//bb_split_after(first, after);
	return new_basic_block(*first, prev_bb, NULL);
}
void bb_insert_first(basic_block_t *b, anode stmt){
	anode last = build_list(NULL, stmt);
	stmt->basic_block = b;
	ANODE_CHAIN(last) = b->entry;
	b->entry = last;

}
void bb_insert_last(basic_block_t *b, anode stmt){
	bb_add_stmt(b, stmt);

}
/*
	parm must be the function body
	lower the if_stmt to cond as cfg
	the return bb should be the exit_bb
*/
basic_block_t *build_cfg(anode start, basic_block_t *list, basic_block_t *b,const char* c);
basic_block_t *build_if_cfg(anode if_stmt, basic_block_t *list, basic_block_t *before){
	bb if_bb, then_bb, else_bb, if_exit;

	anode if_cond = IF_COND(if_stmt);
	anode then_stmt = THEN_CLAUSE(if_stmt);
	anode else_stmt = ELSE_CLAUSE(if_stmt);
	/* 
		IR_BRANCH : the true_jump & false_jump set in the edge->flag ie:EDGE_TRUE 
		so the IR_BRANCH only need ONE operand

	*/
	anode_label *true_label = new anode_label("if_true");
	anode_label *false_label = new anode_label("if_false");


	anode ir_br = build_stmt(IR_BRANCH, if_cond, true_label, false_label);
	anode old_c = COMPOUND_DS_OUTER((anode_expr*)if_cond);

	current_decl_context = old_c;
	if_bb = build_cfg(ir_br, list, before, "if_cond");
	current_decl_context = old_c;

	//FIXME shall we need the then_entry empty-bb
	then_bb = build_cfg(then_stmt, if_bb, if_bb, "then ");
	if_bb->succ->flag = EDGE_TRUE;
	bb_insert_first(then_bb, true_label);

	else_bb = then_bb;
	if(else_stmt){
		else_bb = build_cfg(else_stmt, then_bb, if_bb, "else ");
		if_bb->succ->flag = EDGE_FALSE;

	}
	if_exit = new_basic_block(NULL, else_bb, "if_exit");

	make_edge(then_bb, if_exit, EDGE_PASSTHOUGH);
	if(else_stmt){
		make_edge(else_bb, if_exit, EDGE_PASSTHOUGH);
		bb_insert_first(else_bb, false_label);
	}else{
		make_edge(if_bb, if_exit, EDGE_FALSE);
		bb_add_stmt(if_exit, false_label);

	}

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
basic_block_t *build_while_cfg(anode while_stmt, basic_block_t *list, basic_block_t *before){
	basic_block_t *cond_bb, *body_bb, *exit_bb;

	anode while_cond = WHILE_COND(while_stmt);
	anode while_body = WHILE_CLAUSE(while_stmt);

	anode_label *label_while_exit = new anode_label("label_while_exit");

	anode_label *label_while_cond = new anode_label("label_while_cond");
	anode_label *label_while_body = new anode_label("label_while_body");

	anode ir_goto_cond = build_stmt(IR_GOTO, label_while_cond);
	anode ir_jmp_body = build_stmt(IR_JUMP, while_cond, label_while_body);

//	anode ir_br = build_stmt(IR_BRANCH, while_cond, NULL, label_while_exit);

	anode old_c = COMPOUND_DS_OUTER((anode_expr*)while_cond);


	body_bb = build_cfg(while_body, list, before, "while body");

	current_decl_context = old_c;
	cond_bb = build_cfg(ir_jmp_body, body_bb, body_bb, "while_cond");
	bb_insert_first(cond_bb, label_while_cond);
	current_decl_context = old_c;

	//FIXME shall we need the then_entry empty-bb

	bb_insert_first(body_bb, label_while_body);
	bb_insert_first(body_bb, ir_goto_cond);

	exit_bb = new_basic_block(NULL, cond_bb, "while_exit");
	bb_add_stmt(exit_bb, label_while_exit);

	make_edge(cond_bb, exit_bb, EDGE_PASSTHOUGH);

	make_edge(body_bb, cond_bb, EDGE_PASSTHOUGH);

	return exit_bb;/* would be elimitated */
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
					b = build_while_cfg(stmt, b, b);
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
basic_block_t *build_func_cfg(anode top_ast) {
	//ENTRY_BLOCK_PTR = new_basic_block(NULL, NULL, "entry");
	ENTRY_BLOCK_PTR->comment = strdup("entry");
	EXIT_BLOCK_PTR->comment = strdup("exit");
	basic_block_t *b = build_cfg(top_ast, ENTRY_BLOCK_PTR, ENTRY_BLOCK_PTR, "start");
	init_bb(EXIT_BLOCK_PTR, b);
	
	return b;
}
void dump_bb_var(basic_block_t *b){
	printf("dump: %d ssa_table = %x\n", b->index, b->ssa_table);
	if(b->ssa_table){
		ssa_table_t::iterator iter;
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
void inline print_var(anode v){
	if (v == &undefine_variable)
		printf(" undef");
	else if (v == &uninitial_variable)
		printf(" uninit");
	else
		printf(" %x(%s)", v,
			anode_code_name(anode_code(v)));

}
void  print_ssa_var(anode_ssa_name *ssa) {

	if (!ssa)return;
	if (ssa == &undefine_variable || ssa->var == &undefine_variable){
		printf("ssa_undef\n");
		return;
	}

	if (ssa->is_phi()){
		printf ("%s.%d_phi %x",IDENTIFIER_POINTER(decl_name(ssa->var)),	ssa->version, ssa->var);

	}else{	
		printf ("%s.%d %x",IDENTIFIER_POINTER(decl_name(ssa->var)), ssa->version, ssa->var);
	}

}
void dump_bb(basic_block_t *start_bb){
	basic_block_t *b;
	for(b = start_bb; b; b = b->next){
		int i = 0;
		printf("block %u %s %x\n", b->index, b->comment, b->decl_context);
		ssa_table_t::iterator iter;

		if(b->ssa_table){
			printf("has use-def table status %d\n", b->status);
			for(iter = b->ssa_table->begin(); iter != b->ssa_table->end(); ++iter){
				if(iter->first->var == &undefine_variable){
					printf("undefine_variable\n");
					continue;
				}
				if (iter->first->var == &uninitial_variable){
					printf("uninitial_variable\n");
					continue;
				}
				printf("[%s](%x->%x:%d) ",
					IDENTIFIER_POINTER(decl_name(iter->first->var)),
					iter->first,
					iter->first->var,iter->first->version);
				print_var(iter->second);
				printf("\n");
				
			}
		}
		printf("phi:\n");
		for (anode p = b->phi; p; p = p->chain){
			anode_phi *phi = (anode_phi*)p;
			printf("\tphi %x :\t", p);
			for (auto t : ((anode_phi*)p)->targets){
//				printf(" %x", ANODE_VALUE(t));
				print_var(t);
			}
			printf("\n");
			printf("\tusers: ");
			for (std::set<anode*>::iterator iter = p->users->begin(); 
				iter != p->users->end(); ++iter){
				printf(" %x->%x ", *iter, **iter);
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
		/* dump the use_def info */
		printf("use:\n");
		if (!b->live){
			printf("no use\n");
			goto no_live;
		}
		for (live_set_t::iterator iter = b->live->use.begin();
			iter != b->live->use.end(); ++iter){
			assert(anode_code(*iter) == IR_SSA_NAME || anode_code(*iter) == IR_PHI);
			printf("\t");
			print_ssa_var((anode_ssa_name*)*iter);
		}
		printf("\n");
		printf("def:\n");
		for (live_set_t::iterator iter = b->live->def.begin();
			iter != b->live->def.end(); ++iter){

			assert(anode_code(*iter) == IR_SSA_NAME || anode_code(*iter) == IR_PHI||
				*iter == &undefine_variable);
			printf("\t");

			print_ssa_var((anode_ssa_name*)*iter);
		}
		printf("\n");
		printf("in:\n");
		for (live_set_t::iterator iter = b->live->in.begin();
			iter != b->live->in.end(); ++iter){
			printf("\t");
			print_ssa_var((anode_ssa_name*)*iter);

		}
		printf("\n");
		printf("out:\n");
		for (live_set_t::iterator iter = b->live->out.begin();
			iter != b->live->out.end(); ++iter){
			printf("\t");
			print_ssa_var((anode_ssa_name*)*iter);

		}
		printf("\n");
no_live:
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
	if (b == ENTRY_BLOCK_PTR || b == EXIT_BLOCK_PTR)
		return;
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

/*****************************************************
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
anode_ssa_name* new_ssa_name(anode id){
	//printf("new ssa name %s, %x, %x\n", anode_code_name(anode_code(id)), id, &undefine_variable);
	//assert(anode_code_class(anode_code(id)) == 'd');
	anode_ssa_name *ssa = new anode_ssa_name(id);
	ssa->version = id->version++;
	return ssa;
}
anode_ssa_name* write_variable(anode name, anode value, basic_block_t *b){
	printf("write ssa table bb:%d, %s, 0x%x, %s\n",
		b->index, 
		IDENTIFIER_POINTER(decl_name(name)),
		value, anode_code_name(anode_code(value)));
	if(b->ssa_table == NULL)
		b->ssa_table = new ssa_table_t();
	anode_ssa_name *ssa_name = new_ssa_name(name);
	(*b->ssa_table)[ssa_name] = value;
	return ssa_name;
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
anode get_ssa_name(anode id, int version, basic_block_t *b){
	for (ssa_table_t::iterator iter = b->ssa_table->begin();
		iter != b->ssa_table->end(); ++iter){
		if (iter->first->var == id && iter->first->version == version)
			return iter->first;
	}
	return NULL;
}
void reset_ssa_value(anode id, anode value, basic_block_t *b){
	anode_ssa_name *name ;

}
anode read_var_recursive(anode id, basic_block_t *b){
	anode val;
	int i = 0;
	
	if(!is_pred_ok (b, BB_FILLED)){
		/* not all filled */
		val = new_phi(b);
		unsealed_block[b][id] = val;
		anode_ssa_name *s_name = write_variable(id, val, b);
		s_name->set_phi(val);
		return s_name;
	}
	for(edge e = b->pred; e; e = e->pred_next){
		i++;
	}
	if(i == 0)/* dumm entry_block_ptr */
		return &undefine_variable;
	if(i == 1){
		val = read_variable(id, b->pred->src);
		printf("read get %s\n", anode_code_name(anode_code(val)));
		return val;
	}
	else{

		val = new_phi(b);
		anode_ssa_name *s_name = write_variable(id, val, b);
		s_name->set_phi(val);
		val = add_phi_operands(id, val);
		(*b->ssa_table)[s_name] = val;
		return s_name;

	}
	if(val)
		write_variable(id, val, b);
	assert(anode_code(val) == IR_SSA_NAME);
	return val;
}
anode_ssa_name *get_latest_variable(anode id, basic_block_t *b){
	ssa_table_t::iterator iter;
	int ver = 0;
	anode_ssa_name *p = NULL;

	for (iter = b->ssa_table->begin(); iter != b->ssa_table->end(); ++iter){
		if (iter->first->var == id)
			if (iter->first->version >= ver){
				ver = iter->first->version;
				p = iter->first;
			}
			
		
	}
	return p;

}
anode read_variable(anode id, basic_block_t *b){

	printf("read %s(%s:%x) in %d\n", anode_code_name(anode_code(id)),
		IDENTIFIER_POINTER(decl_name(id)), id, b->index);
	if(!b)
		return NULL;
	if (id == &undefine_variable){
		return id;
	}
	//assert(anode_code_class(anode_code(id)) == 'd');
	if (b->ssa_table){
		anode_ssa_name *t = get_latest_variable(id, b);
		if (t){
			printf("read latest get %s:%x,%x\n", anode_code_name(anode_code(t)), t, 
				&uninitial_variable);
			return t;
		}
	}

	anode xr = read_var_recursive(id, b);
	printf("read_variable return %s\n", anode_code_name(anode_code(xr)));
	return xr;
}

/* trivial when operands consist of only one non-phi value */
anode remove_trivial_phi(anode p){
	anode_phi *phi = (anode_phi*)p;
	anode same = NULL;
	for (auto v : phi->targets){
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

		//phi->append_operand(tt, e);
	}
	return simplify_phi(phi);
}

void rewrite_operand(anode &expr, basic_block_t*b){

	int length = anode_code_length(anode_code(expr));
	int code = anode_code(expr);
	printf("REWRITE %s %d operands @%x\n", anode_code_name(code), anode_code_length(code),
		b->decl_current);

	/* identifier or cst*/
	if (anode_code_class(anode_code(expr)) == 'c')
		return;
	if (anode_code(expr) == IDENTIFIER_NODE){
		printf("\t count id_node %s\n", IDENTIFIER_POINTER(decl_name(expr)));
		printf("\t id: %x\n", get_def(expr, b));
		expr = read_variable(get_def(expr, b), b);
		printf("\t  %s\n", anode_code_name(anode_code(expr)));
		return;
	}


	for(int i = 0; i < length; i++){

		anode *op = ANODE_OPERAND_P(expr, i);
		if (!*op)
			continue;
		rewrite_operand(*op, b);
		/*
		printf("rewrite, %s\n", anode_code_name(anode_code(*op)));
		if(anode_code(*op) == IDENTIFIER_NODE){
			*op = read_variable(get_def(*op, b), b);

		}else if(anode_code_class(anode_code(*op)) == 'c'){
			continue;
		//}else if(anode_code_class(anode_code(op)) == 'e'){
		}else if (EXPR_P(*op)){
			rewrite_operand(*op, b);
		}
		*/
	}

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

	if (b->status & BB_FILLED)
		return;
	for (anode t = b->entry; t; t = ANODE_CHAIN(t)){
		anode stmt = ANODE_VALUE(t);
		if(anode_code_class(anode_code(stmt)) == 'd'){
			anode_decl *d = ANODE_(ANODE_CLASS_CHECK(stmt, 'd'), anode_decl);
		        b->decl_current = anode_cons(NULL, d, b->decl_current);
		        write_variable(get_def(decl_name(stmt), b), &uninitial_variable, b);
		        printf("fill one decl %x\n", d);
		}else if(EXPR_P(stmt)){
			//switch(anode_code(stmt)){
				if (anode_code(stmt) == MODIFY_EXPR){
					printf("get MODIFY_EXPR %s = \n", 
						anode_code_name(anode_code(ANODE_OPERAND(stmt, 0))));
					
					rewrite_operand(ANODE_OPERAND(stmt, 1), b);

					anode id = get_def(ANODE_OPERAND(stmt, 0), b);
					write_variable(id, ANODE_OPERAND(stmt, 1), b);
					//*ANODE_OPERAND_P(stmt, 0) = read_variable(id, b);
					rewrite_operand(ANODE_OPERAND(stmt, 0), b);
					printf("after %s\n",
						anode_code_name(anode_code(ANODE_OPERAND(stmt, 1))) );
				}else
//				default:
					rewrite_operand(stmt, b);
			}
		//}

	}
	b->status |= BB_FILLED;

}



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

	for(basic_block_t *t = b; t != EXIT_BLOCK_PTR; t = t->next){
		fill_bb(t);
	}
	seal_all_last(b);
	printf("undefine_variable %x, uninitial_variable %x\n", &undefine_variable, &uninitial_variable);

}
