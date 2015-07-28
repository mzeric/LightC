#ifndef DFA_H
#define DFA_H

#include <set>
#include <vector>
using namespace std;

typedef struct anode_node *anode;
typedef struct basic_block_s basic_block_t;
typedef set<anode> live_set_t;
typedef set<anode*> live_set_p_t;
struct live_ness_t{

        live_set_t  gen;
        live_set_t  kill;
        live_set_t  phi_use;
        live_set_t  phi_def;
        live_set_t  use;
        live_set_t  def;
        live_set_t  in;
        live_set_t  out;

        live_set_t      r_in;
        live_set_t      r_out;
};
extern anode_node undefine_variable;
extern anode_node uninitial_variable;

void get_all_var(anode expr, basic_block_t *b, live_set_t &var, live_set_t &r_def);

void dfa_handle(basic_block_t *b);


#endif