#ifndef CODE_H


basic_block_t *new_basic_block(anode head, bb ahead, const char *comment);

anode get_rhs(anode expr, list<anode_ins*> &ins);
anode code_br(anode expr, list<anode_ins*> &ins);
anode code_cmp(anode expr, list<anode_ins*> &ins);
anode code_assign(anode a, anode r1, list<anode_ins*> &ins);
anode code_op2(anode b, anode c, int op, list<anode_ins*> &ins);
void gen_code_block(basic_block_t *block);
void gen_code(anode stmt, list<anode_ins*> &ins);
void code_gen(basic_block_t *start);



#endif