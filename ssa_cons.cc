#include <stdio.h>
#include <vector>
#include <map>
#include "anode.h"

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
anode function_body(anode f){
		return NULL;
}
int is_branch(anode t){
        switch(anode_code(t)){
            case IF_STMT:
            case WHILE_STMT:
			case FOR_STMT:
            return 1;
        }
        return 0;
}
bb new_basic_block(anode s, bb old){

}
void current_def(anode decl){

}
void bb_add_stmt(bb b, anode s){

}
void bb_split_before(anode *body, anode *s){
	
		anode old_stmt = *s;
}
/*
	parm must be the function body
	lower the if_stmt to cond as cfg
*/
anode build_cfg(anode body){
	
	int need_new = 1;
	int first = 1;
	basic_block_t *b;
	anode stmt = body;
	while(stmt){

		if(need_new){
			
			if(!first){
				bb_split_before(&body, &stmt);
				first = 0;
			}
			b = new_basic_block(body, b);
			need_new = 0;
		}

		bb_add_stmt(b, stmt);
		
		if(is_branch(stmt)){
			switch(anode_code(stmt)){
				case IF_STMT:
					break;
				case WHILE_STMT:
					break;
				case FOR_STMT:
					break;
				default:;
			}
			need_new = 1;
		}


		stmt = ANODE_CHAIN(stmt);
		first = 0;
	}
	
}
