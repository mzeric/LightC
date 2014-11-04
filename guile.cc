#include "anode.h"
#include <libguile.h>

struct scm_node {
	anode 	*node;
	SCM 	name;


};
static scm_t_bits snode_tag;
#define SCM_EXPORT 
static int print_snode(SCM snode_smob, SCM port, scm_print_state *state)
{
	struct scm_node *snode = (struct scm_node*)SCM_SMOB_DATA(snode_smob);
	scm_puts("#<anode ", port);
	scm_display(snode->name, port);
	scm_puts(">", port);
}
static size_t free_snode(SCM snode_smob)
{
	struct scm_node *snode = (struct scm_node*)SCM_SMOB_DATA(snode_smob);

	scm_gc_free(snode, sizeof(struct scm_node), "snode");
	return 0;
}

void init_snode_type(void)
{
	snode_tag = scm_make_smob_type("snode", sizeof(struct scm_node));
	scm_set_smob_print (snode_tag, print_snode);
}
SCM get_p(void*p)
{
	return scm_from_pointer(p, NULL);
}
SCM block_stmts(SCM bb_pointer)
{
	SCM ret;
	basic_block_t *b = (basic_block_t*)scm_to_pointer(bb_pointer);
	anode stmt = b->entry;
	ret = SCM_EOL;
	for (stmt; stmt != NULL; stmt = ANODE_CHAIN(stmt)) {
		ret = scm_cons(scm_from_pointer(ANODE_VALUE(stmt), NULL), ret);
	}

	return scm_reverse(ret);
}
SCM all_blocks(SCM first_bb)
{
	SCM ret;
	basic_block_t *b = (basic_block_t*)scm_to_pointer(first_bb);
	ret = SCM_EOL;
	for (; b != NULL; b = b->next) {
		ret = scm_cons(scm_from_pointer(b, NULL), ret);
	}

	return scm_reverse(ret);
}
SCM node_type(SCM pointer)
{
	SCM ret;
	anode p = (anode)scm_to_pointer(pointer);
	const char *name = anode_code_name(anode_code(p));
	return scm_from_locale_string(name);
}
SCM node_class(SCM pointer)
{
	SCM ret;
	anode p = (anode)scm_to_pointer(pointer);
	const char c = anode_code_class(anode_code(p));
	return scm_from_char(c);
}
/*
	For CFG output
*/
SCM block_preds(SCM pointer)
{
	SCM ret;
	basic_block_t *b = (basic_block_t*)scm_to_pointer(pointer);
	anode p;
	ret = SCM_EOL;
	for(edge e = b->pred; e; e = e->pred_next) {
		ret = scm_cons(scm_from_pointer(e->src, NULL), ret);
	}

	return scm_reverse(ret);
}
SCM block_succs(SCM pointer)
{
	SCM ret;
	basic_block_t *b = (basic_block_t*)scm_to_pointer(pointer);
	anode p;
	ret = SCM_EOL;
	for(edge e = b->succ; e; e = e->succ_next) {
		ret = scm_cons(scm_from_pointer(e->dst, NULL), ret);
	}

	return scm_reverse(ret);
}
/*
	For expr expand
*/
SCM expand_expr(SCM pointer)
{
	SCM ret;
	int op_len;
	anode e = (anode)scm_to_pointer(pointer);
	ret = SCM_EOL;

	if (EXPR_P(e)) {

		op_len = anode_code_length(anode_code(e));
		ret = scm_cons(scm_from_pointer(e, NULL), ret);
		for (int i = 0; i < op_len; i++) {
			anode op = ANODE_OPERAND(e, i);
			if(op == anode_null){
				ret = scm_cons(SCM_EOL, ret);
				continue;
			}
			if (EXPR_P(op)) {

				ret = scm_cons(expand_expr(scm_from_pointer(op, NULL)), ret);
			}else{

				ret = scm_cons(scm_from_pointer(op, NULL), ret);
			}
		}
	}else{

		ret = scm_cons(scm_from_pointer(e, NULL), ret);
	}

	return scm_reverse(ret);
}
SCM exprp(SCM pointer)
{
	anode p = (anode)scm_to_pointer(pointer);
	return scm_from_bool(EXPR_P(p));
}
void _call_guile_cfg(basic_block_t *t){

	scm_c_define("entry_block", scm_from_pointer(t, NULL));
	scm_call_0( scm_variable_ref(scm_c_lookup("block_func") ));
}
void _call_guile_ssa(basic_block_t *t){
	
	scm_call_0( scm_variable_ref(scm_c_lookup("hook_ssa")));
}
void _init_guile(void)
{
	scm_init_guile();
	scm_c_define_gsubr("block-stmts", 1, 0, 0, (scm_t_subr)&block_stmts);
	scm_c_define_gsubr("all-blocks", 1, 0, 0, (scm_t_subr)&all_blocks);
	scm_c_define_gsubr("node-type", 1, 0, 0, (scm_t_subr)node_type);
	scm_c_define_gsubr("node-class", 1, 0, 0, (scm_t_subr)node_class);
	scm_c_define_gsubr("block-preds", 1, 0, 0, (scm_t_subr)block_preds);
	scm_c_define_gsubr("block-succs", 1, 0, 0, (scm_t_subr)block_succs);
	scm_c_define_gsubr("expand-expr", 1, 0, 0, (scm_t_subr)expand_expr);
	scm_c_define_gsubr("expr?", 1, 0, 0, (scm_t_subr)exprp);
	scm_c_primitive_load("script.scm");
}
