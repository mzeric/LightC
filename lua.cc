#include <stdio.h>
#include <string.h>
#include <lua.hpp>
#include <lualib.h>
#include <iostream>
#include <set>
#include <map>
#include "anode.h"
#include "dfa.h"
using namespace std;

map<int, basic_block_t*> index2addr;
extern map<basic_block_t*, set<basic_block_t*> > ssa_DF;
extern map<basic_block_t*, basic_block_t*> ssa_IDOM;
extern map<basic_block_t*, vector<basic_block_t*> > ssa_D_Tree;

extern map<anode, set<basic_block_t*> > var_phisite;
int int_to_addr[1024];

void bail(lua_State *L, const char *msg){
        fprintf(stderr, "\nFATAL ERROR:\n    %s: %s\n\n",
                msg, lua_tostring(L, -1));
        exit(1);
}
void push_pair(lua_State *L, int key, int value){
	lua_pushnumber(L, key);
	lua_pushnumber(L, value);
	lua_settable(L, -3);
}

void append_ft(lua_State *L, int from, int to){

	static int index = 1;

	lua_pushnumber(L, index++);

	lua_newtable(L);
	push_pair(L, 1, from);
	push_pair(L, 2, to);

	lua_settable(L, -3);

}


void get_block_live(void);
set<anode> get_vars_need_phi(){
        basic_block_t *b = ENTRY_BLOCK_PTR;
        set<anode> vars;
        for(; b != EXIT_BLOCK_PTR; b = b->next){
                set<anode> lo = b->live->out;
                vars.insert(lo.begin(), lo.end());
        }

        return vars;
}
typedef map<anode, set<anode> > defsite_t;

defsite_t get_defs_of_var(){
        basic_block_t *b = ENTRY_BLOCK_PTR;
        
        for(; b != EXIT_BLOCK_PTR; b = b->next){
        
                for (anode stmt = b->entry; stmt; stmt = ANODE_CHAIN(stmt)){
                               live_set_t l_use, l_def;

                        get_all_var(stmt, b, l_use, l_def);

                }
        }

}

int ssa_get_cfg(lua_State *L){

}
struct luaL_Reg ssa_libs[] = {
        {"get_cfg", ssa_get_cfg},
        
        {NULL, NULL}
};
void lua_create_table_cfg(lua_State *L, basic_block_t *entry){
        basic_block_t *p = entry;

        lua_newtable(L);
	
        for(p; p != EXIT_BLOCK_PTR; p = p->next){
		for(edge e = p->succ; e; e = e->succ_next){
			basic_block_t *t = e->dst;
                        int from = p->index;
                        int to =  t->index;
                        //printf("from %d(%p) -> %d(%p)\n", p->index, p, t->index, t);
                        //if (from == 0 and to == 0) /* from 0 to 0 is wrong need FIXME */
                         //       continue;
			append_ft(L, from, to);
		}
	}

        lua_setglobal(L, "cfg_");
}
void lua_set_env(lua_State *L, basic_block_t *entry){

        if (luaL_loadstring(L, "")){
                printf("Error msg is %s.\n", lua_tostring(L, -1));
                return;
        }
        lua_create_table_cfg(L, entry);
}

LUALIB_API int luaopen_newlib(lua_State *L){
        luaL_newlib(L, ssa_libs);
        return 1;
}
lua_State *lua_new(){
        lua_State *L = luaL_newstate();
        luaL_requiref(L, "ssalib", luaopen_newlib, 0);
        return L;
}
void _call_lua_cfg(basic_block_t *p){
        lua_State *L = lua_new();
        luaL_openlibs(L);
        lua_set_env(L, p);
        if(luaL_loadfile(L, "script.lua") || lua_pcall(L, 0, 0, 0)){
                bail(L, "loadfile() or lua_pcall failed");
        }

	for(basic_block_t*t = p; t != EXIT_BLOCK_PTR; t = t->next)
		index2addr[t->index] = t;

	/* fill ssa_DF */
        lua_getglobal(L, "DF");
        if (!lua_istable(L, -1))
        	bail(L, "type(DF) mismatch");

        lua_pushvalue(L, -1);
        lua_pushnil(L);
        while(lua_next(L, -2)){
        	lua_pushvalue(L, -2);
        	int node = lua_tointeger(L, -1);
		printf("key %d \n", node);

		/* read the DF[x] table */
		lua_pushvalue(L, -2);
		lua_pushnil(L);
		while(lua_next(L, -2)){
			lua_pushvalue(L, -2);
			int d = lua_tointeger(L, -2);
			printf("\t-> Frontier %d\n", d);
			ssa_DF[index2addr[node]].insert(index2addr[d]);

			lua_pop(L, 2);
		}
		lua_pop(L, 1);
		/* end of DF[x] table */

		lua_pop(L, 2);

        }
        lua_pop(L, 1);

        /* fill PHI[] */
         
	   lua_getglobal(L, "IDOM");
	   if (!lua_istable(L, -1))
	       bail(L, "type(PHI) mismatch");
       lua_pushvalue(L, -1);
       lua_pushnil(L);
       while(lua_next(L, -2)){
            lua_pushvalue(L, -2);
            int key = lua_tointeger(L, -1);
            int idom_of_key = lua_tointeger(L, -2);
//            printf("%d's idom = %d\n", key, idom_of_key);
            ssa_IDOM[index2addr[key]] = index2addr[idom_of_key];
            lua_pop(L, 2);
       }
       lua_pop(L, 1);

       /* build D-Tree for ssa-cons */
       for(auto it: ssa_IDOM){
            ssa_D_Tree[it.second].push_back(it.first);
        }
	   




        lua_close(L);
}
