#include "anode.h"


#include <list>
#include <iostream>
#include "lower.h"
#include "code.h"
const char* reg[] = {0, "eax", "ebx", "ecx", "edx"};
using namespace std;

string show_anode(anode op);

/*
        gencode for:(intel i386)
                a = b + c 
        op: IRC_ADD, IRC_SUB
*/
typedef std::map<anode_ssa_name*, vector<anode_ssa_name*> > ssa_out_t;
typedef std::map<anode_ssa_name*, anode_ssa_name*> ssa_in_t;
/*
    Test




*/



ssa_in_t build (basic_block_t *block , int index){
    // get def_name and ssa_use of anode_phi
    ssa_in_t    ssa_in;

    for(anode p = block->phi; p; p = ANODE_CHAIN(p)){
        anode_phi *phi = (anode_phi*)p;

        ssa_in[phi->def_name] = (anode_ssa_name*)phi->targets[index];
    }
    return ssa_in;
}

bool check_lost_block(basic_block_t *pblock){
    edge succ = pblock->succ;
    if(succ  == NULL)
        return false;

    
    if(succ->succ_next == NULL){
        return false;
    }

    for(edge *e = &pblock->succ; *e; e = &(*e)->succ_next){

        basic_block_t *down_block = (*e)->dst;
        edge pred = down_block->pred;
        if(pred->pred_next == NULL)
            continue;
        // now the LOST Critical Edge
        basic_block_t *empty_block = new_basic_block(NULL, pblock, "lost_empty");
        down_block->prev = empty_block;
        empty_block->prev = pblock;
        empty_block->next = down_block;
        pblock->next = empty_block;

        // change the edge same as replace_succ_edge()

        edge old_edge = *e;
        edge new_edge = clone_edge(*e);
        
        new_edge->dst = empty_block;
        (*e)->src = empty_block;

        /* shape the empty block from clone */
        new_edge->pred_next = NULL;
        (*e)->succ_next = NULL;

        *e = new_edge;

        empty_block->succ = old_edge;
        empty_block->pred = new_edge;


        
    }

    return true;

}

ssa_out_t in2out(ssa_in_t &ssa_in){
    ssa_out_t   ssa_out;

    for( auto it = ssa_in.begin(); it != ssa_in.end(); ++it){
        anode_ssa_name *to = it->first;
        anode_ssa_name *from = it->second;
        ssa_out[from].push_back(to);
        if(ssa_out[to].empty()){
            // auto init, need nothing
        }
    }
    return ssa_out;
}

void reduce(ssa_out_t &so, anode_ssa_name *x){
    so.erase(x);
    for (auto it = so.begin(); it != so.end(); ++it){
        anode_ssa_name *from = it->first;
        auto ik = it->second.end();
        for(auto iv = it->second.begin(); iv != it->second.end(); ++iv){
            if (*iv == x){
                ik = iv;
                break;
            }
        }
        if(ik != it->second.end())
            it->second.erase(ik);
    }
}
anode_ssa_name *find_next(ssa_out_t& so){
    for (auto it = so.begin(); it != so.end(); ++it){
        if (it->second.size() == 0)
            return it->first;
    }


    return NULL;
}
void out_mov(anode from, anode to){
    std::cout<<"mov "<<show_anode(from)<<" -> "<<show_anode(to)<<std::endl;
}
void topological_phi(ssa_out_t &so, ssa_in_t &si){

    anode_ssa_name *break_sycle = NULL;
    anode_ssa_name *break_next = NULL;

    while(so.size() > 0){
        anode_ssa_name *n = find_next(so);

        if(n == NULL){
            auto break_it = so.begin();
            break_sycle = break_it->first;
            break_next = break_it->second.front();
    
            anode tmp = create_tmp_var(break_sycle);

            out_mov(break_sycle, tmp);
            reduce(so, break_next);

            topological_phi(so, si);

            out_mov(tmp, break_next);

        }


        if(si.find(n) == si.end()){

        }else{
            out_mov(si[n], n);
        }

        if(n){
            reduce(so, n);
        }
    }


}
anode_ssa_name *build_test_ssa(char *name){
    anode var_2 = build_decl(VAR_DECL, new anode_identifier(name), NULL);

    return new anode_ssa_name(var_2);
}
void out_of_ssa_test(){
/*   ssa_in_t ssa_test;
   ssa_test[1] = 4;
   ssa_test[2] = 1
   ssa_test[3] = 4
   ssa_test[4] = 2
   ssa_test[5] = 2
*/
   anode_decl *var_1 = new anode_decl();
   var_1->name = new anode_identifier("phi_var1");
   var_1->code = VAR_DECL;
   anode_ssa_name *ssa_1 = new anode_ssa_name(var_1);
   anode_ssa_name *ssa_2 = build_test_ssa("phi_var2");
   anode_ssa_name *ssa_3 = build_test_ssa("phi_var3");
   anode_ssa_name *ssa_4 = build_test_ssa("phi_var4");
   anode_ssa_name *ssa_5 = build_test_ssa("phi_var5");

 
   ssa_in_t ssa_test2;
   ssa_test2[ssa_2] = ssa_1;
   ssa_test2[ssa_1] = ssa_4;
   ssa_test2[ssa_3] = ssa_4;
   ssa_test2[ssa_4] = ssa_2;
   ssa_test2[ssa_5] = ssa_2;
   ssa_out_t so = in2out(ssa_test2);

   topological_phi(so, ssa_test2);
}


anode code_op2(anode b, anode c, int op, list<anode_ins*> &ins){
    
    anode r1 = create_tmp_var(b);
    anode r2 = create_tmp_var(c);
    r1->is_reg = true;/* must be a register */
    r2->is_reg = true;

        /* load r1, b */
    anode_ins *load1 = new anode_ins(IRC_ASSIGN);
    ANODE_OPERAND(load1, 0) = r1;
    ANODE_OPERAND(load1, 1) = b;

        /* load r2, c */
    anode_ins *load2 = new anode_ins(IRC_ASSIGN);
    ANODE_OPERAND(load2, 0) = r2;
    ANODE_OPERAND(load2, 1) = c;

        /* add r1, r2 => r1 = r1 + r2 */
    anode_ins *op_ins = new anode_ins(op);
    ANODE_OPERAND(op_ins, 0) = r1;
    ANODE_OPERAND(op_ins, 1) = r2;

    ins.push_back(load1);
    ins.push_back(load2);
    ins.push_back(op_ins);

    return r1;
}
anode code_assign(anode a, anode r1, list<anode_ins*> &ins){
        /* load a, r1 */
        anode_ins *ret = new anode_ins(IRC_ASSIGN);
        ANODE_OPERAND(ret, 0) = a;
        ANODE_OPERAND(ret, 1) = r1;
        ins.push_back(ret);
        return NULL;
}
anode code_cmp(anode expr, list<anode_ins*> &ins){
        assert(anode_code_class(anode_code(expr)) == '<');
        anode a = get_rhs(ANODE_OPERAND(expr, 0), ins);
        anode b = get_rhs(ANODE_OPERAND(expr, 1), ins);

        anode_ins *cmp_ins = new anode_ins(IRC_CMP);
        ANODE_OPERAND(cmp_ins, 0) = a;
        ANODE_OPERAND(cmp_ins, 1) = b;

        ins.push_back(cmp_ins);

        return NULL;
}
string show_anode(anode op){

    if(anode_code(op) == IR_SSA_NAME){
        anode_ssa_name *ssa_name = (anode_ssa_name*)op;
        return "ssa:"+string(IDENTIFIER_POINTER(decl_name((ssa_name)->var)))+"_"+to_string(ssa_name->version);
    }
    switch(anode_code(op)){
        case INTEGER_CST:
        {
            int v = ((anode_int_cst*)op)->int_cst;
            return " int:"+std::to_string(v)+" ";
        }
            break;
        case PLUS_EXPR:
        {
            anode op1 = ANODE_OPERAND(op, 0);
            anode op2 = ANODE_OPERAND(op, 1);
            return show_anode(op1) + " + " + show_anode(op2);
        }
            break;
        case VAR_DECL:
        {
            char *name = IDENTIFIER_POINTER(decl_name(op));
            return "var:" + std::string(name);

        }
            break;
        case IR_GOTO:
        {

            anode op1 = ANODE_OPERAND(op, 0);
            return "goto " + string(((anode_label*)op1)->label);

        }
            break;
        case IR_LABEL:
        {   anode_label *l = (anode_label*)op;
            return string(l->label)+":";
        }
        case IRC_ASSIGN:
        {
            anode op1 = ANODE_OPERAND(op, 0);
            anode op2 = ANODE_OPERAND(op, 1);
            return show_anode(op1) + " = " + show_anode(op2);
        }
            break;


        default:{
            string ret = string(anode_code_name(anode_code(op))) + " (";
            for(int i = 0; i < anode_code_length(anode_code(op)); i++){
                ret += show_anode(ANODE_OPERAND(op, i))+" ";
            }
            ret += ")";
            return ret;

        }



    }
    return "null";
}
anode code_jump(anode expr, list<anode_ins*> &ins){

        printf("[IR_JMP]\n");
        assert(anode_code(expr) == IR_JUMP);
        //assert(anode_code_class(anode_code(cond)) == '<');/* must be a comparison */


        anode jmp_expr = ANODE_OPERAND(expr, 0);
        anode jmp_label = ANODE_OPERAND(expr, 1);
        /* ir () ture, false */
        if(anode_code_class(anode_code(jmp_expr)) == '<'){
            anode ce = ANODE_OPERAND(expr, 0);
            printf("cmp expr: %s\n", anode_code_name(anode_code(ce)));
            if(anode_code(jmp_expr) == LT_EXPR){
                anode op1 = ANODE_OPERAND(jmp_expr, 0);
                anode op2 = ANODE_OPERAND(jmp_expr, 1);
                printf("\t 2 operand is: %s %s\n", anode_code_name(anode_code(op1)), anode_code_name(anode_code(op2)));


            }else if(anode_code(jmp_expr) == GT_EXPR){
                anode op1 = ANODE_OPERAND(jmp_expr, 0);
                anode op2 = ANODE_OPERAND(jmp_expr, 1);
                printf("\t 2 operand is: %s(%s) %s(%s)\n", anode_code_name(anode_code(op1)), show_anode(op1).c_str(), anode_code_name(anode_code(op2)),show_anode(op2).c_str());


            }
            //anode_ins *br = new anode_ins(IRC_JMP);
        }
        printf("[end]\n");


        //ins.push_back(br);
}

/* code engine */
anode get_rhs(anode expr, list<anode_ins*> &ins){
        anode ret = NULL;
        if(!expr)
                return expr;

        if(is_ins_rhs(expr)){
            printf("%s is rhs already\n", anode_code_name(anode_code(expr)));
                return expr;
        }
        switch(anode_code(expr)){
                case MODIFY_EXPR:
                {
                        anode r = get_rhs(ANODE_OPERAND(expr, 1), ins);
                        ret = code_assign(ANODE_OPERAND(expr, 0), r, ins);
                        break;
                }
                case PLUS_EXPR:
                {
                        ret = code_op2(ANODE_OPERAND(expr,0), ANODE_OPERAND(expr, 1), IRC_ADD, ins);
                        break;
                }
                case IR_JUMP:
                {
                    code_jump(expr, ins);
                    break;
                }
                case GT_EXPR:
                {
                    code_cmp(expr, ins);

                    break;
                }


                default:
                        printf("unknown expr %s\n", anode_code_name(anode_code(expr)));
        }

        return ret;
}
void gen_code(anode stmt, list<anode_ins*> &ins){
    printf("gen code %d:%s\n", stmt->basic_block->index,anode_code_name(anode_code(stmt)));
    get_rhs(stmt, ins);
}
void gen_code_block(basic_block_t *block){
        if(!block)
            return;
        list<anode_ins*> block_ins;
        std::cout<<"phi note:\n";

        for(anode p = block->phi; p; p = ANODE_CHAIN(p)){
            anode_phi *phi = (anode_phi*)p;
            std::cout<<"phi :"<<show_anode(phi->def_name)<<" = (  ";
            for(auto it : phi->targets){
                std::cout<<show_anode(it)<<"  ";
            }
            std::cout<<")\n";
        }
        for(anode s = block->entry; s; s = ANODE_CHAIN(s)){
                anode stmt = ANODE_VALUE(s);
                std::cout<<show_anode(stmt)<<std::endl;

                //gen_code(stmt, block_ins);
        }
        if(!block->ins)
            block->ins = new list<anode_ins*>();

        block->ins->clear();
        for(auto it: block_ins){        

        block->ins->push_back(it);
        }

}


void gen_jasmin(basic_block_t *block){
    if(block->ins->empty())
        return;
        for(auto it: *block->ins){
                int len = anode_code_length(anode_code(it));
                printf("jasmin: %s\n", anode_code_name(anode_code(it)));
                std::cout<<show_anode(it)<<std::endl;
                for(int i = 0; i < len; i++){
                    printf("\t%s\n", anode_code_name(anode_code(ANODE_OPERAND(it, i))));


                }
                

        }


}

void code_gen(basic_block_t *start){
        printf("begin rewrite\n");
        printf("out_of_SSA\n");
        out_of_ssa_test();
        for(basic_block_t *block = start; block && block != EXIT_BLOCK_PTR; block = block->next){
                gen_code_block(block);
        }
        printf("begin output nasm\n");
        for(basic_block_t *block = start->next; block && block != EXIT_BLOCK_PTR; block = block->next){
                gen_jasmin(block);
        }

}

