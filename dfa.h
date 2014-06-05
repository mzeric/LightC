#ifndef DFA_H
#define DFA_H

#include <set>

using namespace std;

typedef struct anode_node *anode;
typedef struct basic_block_s basic_block_t;
typedef set<anode> live_set_t;
struct live_ness_t{

        live_set_t  gen;
        live_set_t  kill;
        live_set_t  use;
        live_set_t  def;
        live_set_t  in;
        live_set_t  out;
};
extern anode_node undefine_variable;
extern anode_node uninitial_variable;
void dfa_handle(basic_block_t *b);

#endif