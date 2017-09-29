#ifndef _BACKEND_H
#define _BACKEND_H
#include "anode.h"
typedef struct tree_t {
	int op;
	struct tree_t *kids[2];
	int val;
	struct { struct burm_state *state; } x;
        anode node;
        char *code;

} *NODEPTR, *Tree;

int memop(NODEPTR a);
int OP_LABEL(NODEPTR p);

#endif
