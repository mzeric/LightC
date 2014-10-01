#include "anode.h"
#include <libguile.h>

struct scm_node {
	anode 	*node;
	SCM 	name;


};
static scm_t_bits snode_tag;

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
	//need empty-list first for suck scm_append()
	ret = scm_list_n(SCM_UNDEFINED);
	for (stmt; stmt != NULL; stmt = ANODE_CHAIN(stmt)) {
		ret = scm_append(scm_list_2(ret, scm_list_1(scm_from_pointer(ANODE_VALUE(stmt), NULL))));
	}

	return ret;
}
SCM node_type(SCM pointer)
{
	SCM ret;
	anode p = (anode)scm_to_pointer(pointer);
	const char *name = anode_code_name(anode_code(p));
	return scm_from_locale_string(name);
}
void _init_guile(basic_block_t *t)
{
	scm_init_guile();
	scm_c_define_gsubr("block-stmts", 1, 0, 0, (scm_t_subr)&block_stmts);
	scm_c_define_gsubr("node-type", 1, 0, 0, (scm_t_subr)node_type);
	scm_c_primitive_load("script.lisp");
	scm_c_define("entry_block", scm_from_pointer(t, NULL));
	scm_call_0( scm_variable_ref(scm_c_lookup("block_func") ));
}
