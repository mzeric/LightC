#include <stdio.h>
#include <vector>
#include <map>
#include "anode.h"
using namespace std;

struct basic_block{
	vector<struct basic_block*> pred,succ;
	anode entry;
	anode exit;
	anode stmt_list;
};

struct cfg_struct{
};
#if 0
map<map<basic_block*,tree>, tree> currentDef;
void split_while(tree t, basic_block bb){

}
void naming_it(tree var){}
/* recursive build ssa while cfg */
bool is_branch(tree t){
	switch(TREE_CODE(t)){
		case WHILE_STMT:
		case FOR_STMT:
		case IF_STMT:
			return true;
		default:
			return false;
	}
}
#endif
void build_func_ssa(anode func){

	// for each stmt s
	//
	// if(is_branch(s)) {
	// }
}
int t_main(){


	return 0;
}
