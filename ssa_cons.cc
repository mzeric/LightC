#include <stdio.h>
#include <vector>
#include <map>
#include "anode.h"
#include "pass.h"
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

anode build_bb(anode l){
	
}
