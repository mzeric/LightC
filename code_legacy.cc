#include "anode.h"


#include <list>
#include <iostream>
#include "lower.h"
#include "code.h"
#include "sample4.h"



extern vector<anode> get_decl_of_func(basic_block_t *start);

extern void gen(Tree);
extern int tmain(int , char **, Tree);
/*
static Tree tree(int op, Tree l, Tree r) {
        Tree t = (Tree) malloc(sizeof *t);

        t->op = op;
        t->kids[0] = l; t->kids[1] = r;
        t->val = 0;
        t->x.state = 0;
        return t;
}*/
extern Tree tree(int, Tree, Tree);
static Tree new_tree(anode node, int right);
Tree tree2(anode node, int op){
        anode l = ANODE_OPERAND(node, 0);
        anode r = ANODE_OPERAND(node, 1);
        Tree lt = NULL;
        Tree rt = NULL;

        if(l)
                lt = new_tree(l, 1);

        if(r)
                rt = new_tree(r, 1);
        return tree(op, lt, rt);
}
static Tree new_tree(anode node, int right=0){
        printf("cur tree %s\n", anode_code_name(anode_code(node)));
        int op;
        Tree ret;
        switch (anode_code(node)){
                case VAR_DECL:
                        printf("get var:%s\n", IDENTIFIER_POINTER(decl_name(node)));
                        ret = tree(ADDRLP, 0, 0);
                        ret->node = node;

                        if(right){
                            ret = tree(INDIRC, ret, 0);
                        }
                        break;

                case INTEGER_CST:{  
                        int v = ((anode_int_cst*)node)->int_cst;
                        ret = tree(CNSTI4, 0, 0);
                        ret->val = v;
                        printf("get CNSTI4 %d\n", v);
                        break;
                }
                case PLUS_EXPR:{
                        anode l = ANODE_OPERAND(node, 0);
                        anode r = ANODE_OPERAND(node, 1);
                        Tree lt = NULL;
                        Tree rt = NULL;

                        if(l)
                                lt = new_tree(l, 1);

                        if(r)
                                rt = new_tree(r, 1);
                        ret =  tree(ADDI, lt , rt);    
                        break;                   
                }
                case MODIFY_EXPR:{
                    printf("\t");
                        anode l = ANODE_OPERAND(node, 0);
                        anode r = ANODE_OPERAND(node, 1);
                        Tree lt = NULL;
                        Tree rt = NULL;

                        if(l)
                                lt = new_tree(l);

                        if(r)
                                rt = new_tree(r, 1);
                        ret =  tree(ASGNI, lt , rt);
                    printf("\n");
                        break;
                }
                case IR_JUMP:{
                    printf("get IR_JUMP\n");
                    ret = NULL;
                    break;
                }
                case IR_GOTO:{
                    anode l = ANODE_OPERAND(node, 0);
                    Tree lt = new_tree(l);
                    ret = tree(JUMPV, lt, NULL);
                    printf("get IR_GOTO\n");
                    break;
                }
                case IR_LABEL:{
                    printf("get ir_label:%s\n",((anode_label*)node)->label);
                    break;
                }
                default:{
                        ret = NULL;
                        printf("unknow ir:%s\n", anode_code_name(anode_code(node)));
                }
        }

        return ret;

}
void dump_tree(Tree t){
        if(!t){
            printf("0");
            return;
        }
            
        printf("\t(");
        switch(t->op){
            case ASGNI: printf("ASGNI[%x] ", ASGNI);break;
            case ADDRLP: printf("ADDRLP [%x]", ADDRLP);break;
            case ADDI: printf("ADDI ");break;
            case CNSTI4: printf("CNSTI4 [%x]", CNSTI4);break;
            case INDIRC: printf("INDIRC"); break;
            default: printf("unknow[%d]", t->op);
        }

        for(int i = 0; i < 2; i++){
            Tree tmp = t->kids[i];
            dump_tree(tmp);
            if(i != 1)
            printf(",");
        }
        printf(")\n");
}
void test_gen(){
    Tree t = tree(ASGNI,
        tree(ADDRLP, 0, 0),
        tree(ADDI,
            //tree(CVCI, tree(INDIRC, tree(ADDRLP, 0, 0), 0), 0),
            tree(INDIRC, tree(ADDRLP, 0, 0), 0),
            (t = tree(CNSTI4, 0, 0), t->val = 4, t)
            //tree(INDIRC, tree(ADDRLP, 0, 0), 0)
        )
    );

    gen(t);
}

void gen_code(basic_block_t *block){
        printf("gen code for block %d[%s]\n", block->index, block->comment);
        for(anode s = block->entry; s; s = ANODE_CHAIN(s)){
            anode stmt = ANODE_VALUE(s);
            printf("name:%s(%c)\n", anode_code_name(anode_code(stmt)),
                        anode_code_class(anode_code(stmt)));

            Tree t = new_tree(stmt);dump_tree(t);
            if(t)gen(t);

        }
}
void code_gen_legacy(basic_block_t *start){
        printf("code gen legacy\n");
        vector<Tree> ins;
        test_gen();


        for(basic_block_t *block = start; block && block != EXIT_BLOCK_PTR; block = block->next){
                gen_code(block);
        }

        get_decl_of_func(start);
}
