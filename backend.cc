#include "backend.h"
#include "sample4.h"

enum { EAX=0, ECX=1, EDX=2, EBX=3, ESI=6, EDI=7 };
int REG_MASK = (1<<EDI) | (1<<ESI) | (1<<EBX) | (1<<EDX) | (1<<ECX) | (1<<EAX);
int RA_MASK = 0;
int memop(NODEPTR a){
        return 3;
}
int OP_LABEL(NODEPTR p) {
        switch (p->op) {
        case CNSTI:  if (p->val == 0) return 661 /* I0I */;
        default:     return p->op;
        }
}

void emit_code(){

}
void register_alloc(){

}
