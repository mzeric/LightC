#ifndef _LOWER_H
#define _LOWER_H

void lower_bb(bb_t *b);

enum lower_status{
        LW_OK,
        LW_DONE,
        LW_ERROR,
};

#endif