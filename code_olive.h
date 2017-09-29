#ifndef __OLIVE_HEADER_INCLUDED__
#define __OLIVE_HEADER_INCLUDED__
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <string>
/*
   FILE: sample4.brg
  
   Copyright (c) 1997 Princeton University

   All rights reserved.

   This software is to be used for non-commercial purposes only,
   unless authorized permission to do otherwise is obtained.  
   For more information, contact spam@ee.princeton.edu
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
enum {
	ADDI=309, ADDRLP=295, ASGNI=53,
	CNSTI=21, CVCI=85, I0I=661, INDIRC=67
};

#include "backend.h"

#define GET_KIDS(p)	((p)->kids)
#define PANIC printf
#define STATE_LABEL(p) ((p)->x.state)
#define SET_STATE(p,s) (p)->x.state=(s)
#define DEFAULT_COST	break
#define NO_ACTION(x)

typedef struct COST {
    int cost;
} COST;
#define COST_LESS(a,b) ((a).cost < (b).cost)

static COST COST_INFINITY = { 32767 };
static COST COST_ZERO     = { 0 };

/*static char* burm_string = "FOO";*/
static int _ern = 0;

static int shouldTrace = 0;
static int shouldCover = 0;

extern int OP_LABEL(NODEPTR p);
extern int memop(NODEPTR a);
#define EM_CODE puts
static void burm_trace(NODEPTR, int, COST);

#define BURP 0
#define ADDI 1
#define ADDRLP 2
#define ASGNI 3
#define CNSTI 4
#define CVCI 5
#define I0I 6
#define INDIRC 7
#define CNSTI4 8
#define ADDRGP4 9
#define JUMPV 10
#define ARGI4 11
#define MULI4 12
#define DIVI4 13
#define EQI4 14
#define GEI4 15

struct burm_state {
  int op;
  NODEPTR node;
  struct burm_state **kids;
  COST cost[13];
  struct {
    unsigned burm_stmt:4;
    unsigned burm_reg:4;
    unsigned burm_cnst:1;
    unsigned burm_con:1;
    unsigned burm_con1:1;
    unsigned burm_rc:2;
    unsigned burm_mem4:1;
    unsigned burm_mrc:2;
    unsigned burm_mrca:2;
    unsigned burm_addr:1;
    unsigned burm_addrj:1;
    unsigned burm__:1;
  } rule;
};


struct burm_state *burm_label(NODEPTR);
struct burm_state *burm_label1(NODEPTR);

void dumpCover(NODEPTR,int,int);

#endif
