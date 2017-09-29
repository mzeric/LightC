#ifndef _OLIVE_H
#define _OLIVE_H

#include "anode.h"
typedef struct tree_t {
        int op;
        struct tree_t *kids[2];
        int val;
        struct { struct burm_state *state; } x;
        anode node;
} *NODEPTR, *Tree;

#define GET_KIDS(p)     ((p)->kids)
#define PANIC printf
#define STATE_LABEL(p) ((p)->x.state)
#define SET_STATE(p,s) (p)->x.state=(s)
#define DEFAULT_COST    break
#define NO_ACTION(x)

typedef struct COST {
    int cost;
} COST;

struct burm_state {
  int op;
  NODEPTR node;
  struct burm_state **kids;
  COST cost[13];
  struct {
    unsigned burm_stmt:3;
    unsigned burm_reg:3;
    unsigned burm_cnst:1;
    unsigned burm_con:1;
    unsigned burm_con1:1;
    unsigned burm_rc:2;
    unsigned burm_mem4:1;
    unsigned burm_mrc:2;
    unsigned burm_mrca:2;
    unsigned burm_addr:1;
    unsigned burm_addrj:1;
    unsigned burm__:1;
  } rule;
};

enum {
        ADDI=309, ADDRLP=295, ASGNI=53,
        CNSTI=21, CVCI=85, I0I=661, INDIRC=67
};

#endif