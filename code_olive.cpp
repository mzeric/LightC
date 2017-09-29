#include "code_olive.h"
#define burm_stmt_NT 1
#define burm_reg_NT 2
#define burm_cnst_NT 3
#define burm_con_NT 4
#define burm_con1_NT 5
#define burm_rc_NT 6
#define burm_mem4_NT 7
#define burm_mrc_NT 8
#define burm_mrca_NT 9
#define burm_addr_NT 10
#define burm_addrj_NT 11
#define burm___NT 12
extern int burm_max_nt;


void stmt_action(struct burm_state *_s, 

int indent);


void reg_action(struct burm_state *_s, 

int indent);


void cnst_action(struct burm_state *_s, 

int indent);


void con_action(struct burm_state *_s, 

int indent);


void con1_action(struct burm_state *_s, 

int indent);


void rc_action(struct burm_state *_s, 

int indent);


void mem4_action(struct burm_state *_s, 

int indent);


void mrc_action(struct burm_state *_s, 

int indent);


void mrca_action(struct burm_state *_s, 

int indent);


void addr_action(struct burm_state *_s, 

int indent);


void addrj_action(struct burm_state *_s, 

int indent);
#ifndef ALLOC
#define ALLOC(n) malloc(n)
#endif

#ifndef burm_assert
#define burm_assert(x,y) if (!(x)) {  y; abort(); }
#endif

static NODEPTR burm_np;

#define burm_stmt_NT 1
#define burm_reg_NT 2
#define burm_cnst_NT 3
#define burm_con_NT 4
#define burm_con1_NT 5
#define burm_rc_NT 6
#define burm_mem4_NT 7
#define burm_mrc_NT 8
#define burm_mrca_NT 9
#define burm_addr_NT 10
#define burm_addrj_NT 11
#define burm___NT 12
extern int burm_max_nt;
int burm_max_nt = 12;

std::string burm_ntname[] = {
  "",
  "stmt",
  "reg",
  "cnst",
  "con",
  "con1",
  "rc",
  "mem4",
  "mrc",
  "mrca",
  "addr",
  "addrj",
  "_",
  ""
};

static short burm_nts_0[] = { burm___NT, burm___NT, burm___NT, 0 };
static short burm_nts_1[] = { burm_addr_NT, burm_rc_NT, 0 };
static short burm_nts_2[] = { burm_addr_NT, burm_mem4_NT, burm_con1_NT, 0 };
static short burm_nts_3[] = { burm_reg_NT, 0 };
static short burm_nts_4[] = { 0 };
static short burm_nts_5[] = { burm_cnst_NT, 0 };
static short burm_nts_6[] = { burm_con_NT, 0 };
static short burm_nts_7[] = { burm_reg_NT, burm_mrc_NT, 0 };
static short burm_nts_8[] = { burm_addr_NT, 0 };
static short burm_nts_9[] = { burm_mem4_NT, 0 };
static short burm_nts_10[] = { burm_rc_NT, 0 };
static short burm_nts_11[] = { burm_reg_NT, burm_con_NT, 0 };
static short burm_nts_12[] = { burm_reg_NT, burm_reg_NT, 0 };
static short burm_nts_13[] = { burm_addrj_NT, 0 };
static short burm_nts_14[] = { burm_mem4_NT, burm_rc_NT, 0 };
static short burm_nts_15[] = { burm_mrca_NT, 0 };

short *burm_nts[] = {
  burm_nts_0,  /* 0 */
  burm_nts_1,  /* 1 */
  burm_nts_2,  /* 2 */
  burm_nts_3,  /* 3 */
  burm_nts_4,  /* 4 */
  burm_nts_4,  /* 5 */
  burm_nts_5,  /* 6 */
  burm_nts_6,  /* 7 */
  burm_nts_3,  /* 8 */
  burm_nts_7,  /* 9 */
  burm_nts_8,  /* 10 */
  burm_nts_9,  /* 11 */
  burm_nts_10,  /* 12 */
  burm_nts_9,  /* 13 */
  burm_nts_10,  /* 14 */
  burm_nts_4,  /* 15 */
  burm_nts_8,  /* 16 */
  burm_nts_8,  /* 17 */
  burm_nts_4,  /* 18 */
  burm_nts_8,  /* 19 */
  burm_nts_6,  /* 20 */
  burm_nts_11,  /* 21 */
  burm_nts_7,  /* 22 */
  burm_nts_12,  /* 23 */
  burm_nts_4,  /* 24 */
  burm_nts_4,  /* 25 */
  burm_nts_13,  /* 26 */
  burm_nts_14,  /* 27 */
  burm_nts_14,  /* 28 */
  burm_nts_15,  /* 29 */
};

char burm_arity[] = {
  3,  /* 0=BURP */
  2,  /* 1=ADDI */
  0,  /* 2=ADDRLP */
  2,  /* 3=ASGNI */
  0,  /* 4=CNSTI */
  1,  /* 5=CVCI */
  0,  /* 6=I0I */
  1,  /* 7=INDIRC */
  0,  /* 8=CNSTI4 */
  0,  /* 9=ADDRGP4 */
  1,  /* 10=JUMPV */
  1,  /* 11=ARGI4 */
  2,  /* 12=MULI4 */
  2,  /* 13=DIVI4 */
  2,  /* 14=EQI4 */
  2,  /* 15=GEI4 */
};

std::string burm_opname[] = {
  /* 0 */  "BURP",
  /* 1 */  "ADDI",
  /* 2 */  "ADDRLP",
  /* 3 */  "ASGNI",
  /* 4 */  "CNSTI",
  /* 5 */  "CVCI",
  /* 6 */  "I0I",
  /* 7 */  "INDIRC",
  /* 8 */  "CNSTI4",
  /* 9 */  "ADDRGP4",
  /* 10 */  "JUMPV",
  /* 11 */  "ARGI4",
  /* 12 */  "MULI4",
  /* 13 */  "DIVI4",
  /* 14 */  "EQI4",
  /* 15 */  "GEI4",
};


std::string burm_string[] = {
  /* 0 */  "stmt: BURP(_,_,_)",
  /* 1 */  "stmt: ASGNI(addr,rc)",
  /* 2 */  "stmt: ASGNI(addr,ADDI(mem4,con1))",
  /* 3 */  "stmt: reg",
  /* 4 */  "cnst: CNSTI4",
  /* 5 */  "con1: CNSTI4",
  /* 6 */  "con: cnst",
  /* 7 */  "rc: con",
  /* 8 */  "rc: reg",
  /* 9 */  "reg: ADDI(reg,mrc)",
  /* 10 */  "mem4: INDIRC(addr)",
  /* 11 */  "mrc: mem4",
  /* 12 */  "mrc: rc",
  /* 13 */  "mrca: mem4",
  /* 14 */  "mrca: rc",
  /* 15 */  "mrca: ADDRGP4",
  /* 16 */  "reg: CVCI(INDIRC(addr))",
  /* 17 */  "reg: INDIRC(addr)",
  /* 18 */  "reg: I0I",
  /* 19 */  "reg: addr",
  /* 20 */  "reg: con",
  /* 21 */  "reg: ADDI(reg,con)",
  /* 22 */  "reg: MULI4(reg,mrc)",
  /* 23 */  "reg: DIVI4(reg,reg)",
  /* 24 */  "addr: ADDRLP",
  /* 25 */  "addrj: ADDRGP4",
  /* 26 */  "stmt: JUMPV(addrj)",
  /* 27 */  "stmt: EQI4(mem4,rc)",
  /* 28 */  "stmt: GEI4(mem4,rc)",
  /* 29 */  "stmt: ARGI4(mrca)",
};


std::string burm_files[] = {
"sample4.brg",
};

int burm_file_numbers[] = {
  /* 0 */  0,
  /* 1 */  0,
  /* 2 */  0,
  /* 3 */  0,
  /* 4 */  0,
  /* 5 */  0,
  /* 6 */  0,
  /* 7 */  0,
  /* 8 */  0,
  /* 9 */  0,
  /* 10 */  0,
  /* 11 */  0,
  /* 12 */  0,
  /* 13 */  0,
  /* 14 */  0,
  /* 15 */  0,
  /* 16 */  0,
  /* 17 */  0,
  /* 18 */  0,
  /* 19 */  0,
  /* 20 */  0,
  /* 21 */  0,
  /* 22 */  0,
  /* 23 */  0,
  /* 24 */  0,
  /* 25 */  0,
  /* 26 */  0,
  /* 27 */  0,
  /* 28 */  0,
  /* 29 */  0,
};

int burm_line_numbers[] = {
  /* 0 */  84,
  /* 1 */  90,
  /* 2 */  100,
  /* 3 */  107,
  /* 4 */  117,
  /* 5 */  125,
  /* 6 */  130,
  /* 7 */  142,
  /* 8 */  152,
  /* 9 */  162,
  /* 10 */  171,
  /* 11 */  176,
  /* 12 */  183,
  /* 13 */  190,
  /* 14 */  194,
  /* 15 */  198,
  /* 16 */  204,
  /* 17 */  213,
  /* 18 */  223,
  /* 19 */  232,
  /* 20 */  242,
  /* 21 */  251,
  /* 22 */  264,
  /* 23 */  270,
  /* 24 */  278,
  /* 25 */  285,
  /* 26 */  289,
  /* 27 */  295,
  /* 28 */  302,
  /* 29 */  308,
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

static short burm_decode_stmt[] = {
   -1,
  0,
  1,
  2,
  3,
  26,
  27,
  28,
  29,
};

static short burm_decode_reg[] = {
   -1,
  9,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
};

static short burm_decode_cnst[] = {
   -1,
  4,
};

static short burm_decode_con[] = {
   -1,
  6,
};

static short burm_decode_con1[] = {
   -1,
  5,
};

static short burm_decode_rc[] = {
   -1,
  7,
  8,
};

static short burm_decode_mem4[] = {
   -1,
  10,
};

static short burm_decode_mrc[] = {
   -1,
  11,
  12,
};

static short burm_decode_mrca[] = {
   -1,
  13,
  14,
  15,
};

static short burm_decode_addr[] = {
   -1,
  24,
};

static short burm_decode_addrj[] = {
   -1,
  25,
};

static short burm_decode__[] = {
   -1,
};

int burm_rule(struct burm_state *state, int goalnt) {
  burm_assert(goalnt >= 1 && goalnt <= 12,
        PANIC("Bad goal nonterminal %d in burm_rule\n", goalnt));
  if (!state)
    return 0;
  switch (goalnt) {
  case burm_stmt_NT:  return burm_decode_stmt[((struct burm_state *)state)->rule.burm_stmt];
  case burm_reg_NT:  return burm_decode_reg[((struct burm_state *)state)->rule.burm_reg];
  case burm_cnst_NT:  return burm_decode_cnst[((struct burm_state *)state)->rule.burm_cnst];
  case burm_con_NT:  return burm_decode_con[((struct burm_state *)state)->rule.burm_con];
  case burm_con1_NT:  return burm_decode_con1[((struct burm_state *)state)->rule.burm_con1];
  case burm_rc_NT:  return burm_decode_rc[((struct burm_state *)state)->rule.burm_rc];
  case burm_mem4_NT:  return burm_decode_mem4[((struct burm_state *)state)->rule.burm_mem4];
  case burm_mrc_NT:  return burm_decode_mrc[((struct burm_state *)state)->rule.burm_mrc];
  case burm_mrca_NT:  return burm_decode_mrca[((struct burm_state *)state)->rule.burm_mrca];
  case burm_addr_NT:  return burm_decode_addr[((struct burm_state *)state)->rule.burm_addr];
  case burm_addrj_NT:  return burm_decode_addrj[((struct burm_state *)state)->rule.burm_addrj];
  case burm___NT:  return burm_decode__[((struct burm_state *)state)->rule.burm__];
  default:
    burm_assert(0, PANIC("Bad goal nonterminal %d in burm_rule\n", goalnt));
  }
  return 0;
}


struct burm_action {
  int nt;
  struct burm_state* st;
};

#ifndef RULE
#define RULE(n,s) \
    (act = (burm_action*) malloc(sizeof(struct burm_action)),act->nt=n,act->st=s,act)
#endif

int burm_cost_code(COST *_c, int _ern,struct burm_state *_s)
{
  NODEPTR *_children;
  struct burm_action *act;
  switch(_ern){
  default:
    DEFAULT_COST;
  case 0:
    _children = GET_KIDS(_s->node);
{


 return 1; 
}
  break;
  case 1:
{


 (*_c).cost=_s->kids[0]->cost[burm_addr_NT].cost+_s->kids[1]->cost[burm_rc_NT].cost+1; 
}
  break;
  case 2:
{


 (*_c).cost = memop(_s->node);
}
  break;
  case 3:
{


 (*_c).cost=_s->cost[burm_reg_NT].cost; 
}
  break;
  case 4:
{


 (*_c).cost=0; 
}
  break;
  case 5:
{


 (*_c).cost = 0; 
}
  break;
  case 6:
{


 (*_c).cost = _s->cost[burm_cnst_NT].cost; 
}
  break;
  case 7:
{


 (*_c).cost=_s->cost[burm_con_NT].cost; 
}
  break;
  case 8:
{


 (*_c).cost=_s->cost[burm_reg_NT].cost; 
}
  break;
  case 9:
{


 (*_c).cost=_s->kids[0]->cost[burm_reg_NT].cost+_s->kids[1]->cost[burm_mrc_NT].cost+1; 
}
  break;
  case 10:
{


 (*_c).cost = _s->kids[0]->cost[burm_addr_NT].cost; 
}
  break;
  case 11:
{


 (*_c).cost = _s->cost[burm_mem4_NT].cost + 1;
}
  break;
  case 12:
{


 (*_c).cost = _s->cost[burm_rc_NT].cost; 
}
  break;
  case 13:
{


(*_c).cost = _s->cost[burm_mem4_NT].cost; 
}
  break;
  case 14:
{


(*_c).cost = _s->cost[burm_rc_NT].cost; 
}
  break;
  case 15:
{


(*_c).cost = 0; 
}
  break;
  case 16:
{


 (*_c).cost = _s->kids[0]->kids[0]->cost[burm_addr_NT].cost+1; 
}
  break;
  case 17:
{


 (*_c).cost = _s->kids[0]->cost[burm_addr_NT].cost; 
}
  break;
  case 18:
{


 (*_c).cost=0; 
}
  break;
  case 19:
{


 (*_c).cost=_s->cost[burm_addr_NT].cost+1; 
}
  break;
  case 20:
{


 (*_c).cost= _s->cost[burm_con_NT].cost + 1;
}
  break;
  case 21:
{


 (*_c).cost=_s->kids[0]->cost[burm_reg_NT].cost+_s->kids[1]->cost[burm_con_NT].cost; 
}
  break;
  case 22:
{


 (*_c).cost=_s->kids[0]->cost[burm_reg_NT].cost+_s->kids[1]->cost[burm_mrc_NT].cost + 14; 
}
  break;
  case 23:
{


 (*_c).cost=_s->kids[0]->cost[burm_reg_NT].cost+_s->kids[1]->cost[burm_reg_NT].cost; 
}
  break;
  case 24:
{


 (*_c).cost=0; 
}
  break;
  case 25:
{


 (*_c).cost=0;
}
  break;
  case 26:
{


(*_c).cost = _s->kids[0]->cost[burm_addrj_NT].cost + 3;
}
  break;
  case 27:
{


(*_c).cost = _s->kids[0]->cost[burm_mem4_NT].cost + _s->kids[1]->cost[burm_rc_NT].cost + 5;
}
  break;
  case 28:
{


(*_c).cost = _s->kids[0]->cost[burm_mem4_NT].cost + _s->kids[1]->cost[burm_rc_NT].cost + 5;
}
  break;
  case 29:
{


(*_c).cost = _s->kids[0]->cost[burm_mrca_NT].cost + 1;
}
  break;
  }
  return 1;
}


void stmt_action(struct burm_state *_s, 

int indent);


void reg_action(struct burm_state *_s, 

int indent);


void cnst_action(struct burm_state *_s, 

int indent);


void con_action(struct burm_state *_s, 

int indent);


void con1_action(struct burm_state *_s, 

int indent);


void rc_action(struct burm_state *_s, 

int indent);


void mem4_action(struct burm_state *_s, 

int indent);


void mrc_action(struct burm_state *_s, 

int indent);


void mrca_action(struct burm_state *_s, 

int indent);


void addr_action(struct burm_state *_s, 

int indent);


void addrj_action(struct burm_state *_s, 

int indent);


#include <stdarg.h>

void burm_exec(struct burm_state *state, int nterm, ...) 
{
  va_list(ap);
  va_start(ap,nterm);

  burm_assert(nterm >= 1 && nterm <= 12,
        PANIC("Bad nonterminal %d in $exec\n", nterm));

  if (state)
    switch (nterm) {
    case burm_stmt_NT:
      stmt_action(state,va_arg(ap,int));
      break;
    case burm_reg_NT:
      reg_action(state,va_arg(ap,int));
      break;
    case burm_cnst_NT:
      cnst_action(state,va_arg(ap,int));
      break;
    case burm_con_NT:
      con_action(state,va_arg(ap,int));
      break;
    case burm_con1_NT:
      con1_action(state,va_arg(ap,int));
      break;
    case burm_rc_NT:
      rc_action(state,va_arg(ap,int));
      break;
    case burm_mem4_NT:
      mem4_action(state,va_arg(ap,int));
      break;
    case burm_mrc_NT:
      mrc_action(state,va_arg(ap,int));
      break;
    case burm_mrca_NT:
      mrca_action(state,va_arg(ap,int));
      break;
    case burm_addr_NT:
      addr_action(state,va_arg(ap,int));
      break;
    case burm_addrj_NT:
      addrj_action(state,va_arg(ap,int));
      break;
    default:
      PANIC("Bad nonterminal %d in $exec\n", nterm);
      break;
    }
  else
    PANIC("Bad state for $exec in nonterminal %d \n",nterm);
  va_end(ap);
}

#define EXEC(s,n,a) ( \
  (n == burm_stmt_NT)? burm_exec(s,n,a): \
  (n == burm_reg_NT)? burm_exec(s,n,a): \
  (n == burm_cnst_NT)? burm_exec(s,n,a): \
  (n == burm_con_NT)? burm_exec(s,n,a): \
  (n == burm_con1_NT)? burm_exec(s,n,a): \
  (n == burm_rc_NT)? burm_exec(s,n,a): \
  (n == burm_mem4_NT)? burm_exec(s,n,a): \
  (n == burm_mrc_NT)? burm_exec(s,n,a): \
  (n == burm_mrca_NT)? burm_exec(s,n,a): \
  (n == burm_addr_NT)? burm_exec(s,n,a): \
  (n == burm_addrj_NT)? burm_exec(s,n,a): \
  PANIC("Bad nonterminal %d in $exec\n", n))

struct burm_state *burm_immed(struct burm_state *s,int n);
#ifndef NO_ACTION
#define NO_ACTION assert(0)
#endif


void stmt_action(struct burm_state *_s, 

int indent)
{
  struct burm_state *_t;
  int _ern=burm_decode_stmt[_s->rule.burm_stmt];
  NODEPTR *_children;
  if(_s->rule.burm_stmt==0)
    NO_ACTION(stmt);
  switch(_ern){
  case 0:
    _children = GET_KIDS(_s->node);
{



		stmt_action(burm_immed(_s,0),10);
	
}
  break;
  case 1:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << " ";
		std::cerr << burm_string[_ern] << "\n";
		addr_action(_s->kids[0],indent+1);
		rc_action(_s->kids[1],indent+1);
		EM_CODE("movl %1, %0\n");
	
}
  break;
  case 2:
{




		EM_CODE("incl %1\n");
	
}
  break;
  case 3:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << " ";
		std::cerr << burm_string[_ern] << "\n";
		reg_action(_s,indent+1);
	
}
  break;
  case 26:
{



	
	addrj_action(_s->kids[0],1+indent);
	EM_CODE("jmp %0");

}
  break;
  case 27:
{



			
			mem4_action(_s->kids[0],1+indent);
			rc_action(_s->kids[1],1+indent);
			EM_CODE("cmpl %1,%0\nje %a\n");


}
  break;
  case 28:
{



			mem4_action(_s->kids[0],1+indent);
			rc_action(_s->kids[1],1+indent);
			EM_CODE("cmpl %1,%0\njge %a\n");


}
  break;
  case 29:
{



	mrca_action(_s->kids[0],1+indent);
	EM_CODE("pushl %0\n");//将参数让入堆栈

}
  break;
  }
}


void reg_action(struct burm_state *_s, 

int indent)
{
  struct burm_state *_t;
  int _ern=burm_decode_reg[_s->rule.burm_reg];
  NODEPTR *_children;
  if(_s->rule.burm_reg==0)
    NO_ACTION(reg);
  switch(_ern){
  case 9:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << " ";
		std::cerr << burm_string[_ern] << "\n";
		reg_action(_s->kids[0],indent+1);
		mrc_action(_s->kids[1],indent+1);
	
}
  break;
  case 16:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << " ";
		std::cerr << burm_string[_ern] << "\n";
		addr_action(_s->kids[0]->kids[0],indent+1);
	
}
  break;
  case 17:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << " ";
		std::cerr << burm_string[_ern] << "\n";
		addr_action(_s->kids[0],indent+1);
	
}
  break;
  case 18:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << " ";
		std::cerr << burm_string[_ern] << "\n";
	
}
  break;
  case 19:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << " ";
		std::cerr << burm_string[_ern] << "\n";
		addr_action(_s,indent+1);
		EM_CODE("leal %0, %c\n");
	
}
  break;
  case 20:
{



		
		con_action(_s,indent+1);
		EM_CODE("mov %0, %c\n");

	
}
  break;
  case 21:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << " ";
		std::cerr << burm_string[_ern] << "\n";
		reg_action(_s->kids[0],indent+1);
		con_action(_s->kids[1],indent+1);
		_s->node->node = NULL;

		EM_CODE("?movl %0,%c\naddl %1,%c\n");

	
}
  break;
  case 22:
{



			reg_action(_s->kids[0],1+indent);
			mrc_action(_s->kids[1],1+indent);
			EM_CODE("?movl %0,%c\nimull %1,%c\n");

}
  break;
  case 23:
{




			reg_action(_s->kids[0],1+indent);
			reg_action(_s->kids[1],1+indent);
			EM_CODE("?movl %0, %eax\ncdq\nidivl %1\n");

}
  break;
  }
}


void cnst_action(struct burm_state *_s, 

int indent)
{
  struct burm_state *_t;
  int _ern=burm_decode_cnst[_s->rule.burm_cnst];
  NODEPTR *_children;
  if(_s->rule.burm_cnst==0)
    NO_ACTION(cnst);
  switch(_ern){
  case 4:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << " ";
		printf("CNSTI4\n");
		std::cerr << burm_string[_ern] << "\n";
	
}
  break;
  }
}


void con_action(struct burm_state *_s, 

int indent)
{
  struct burm_state *_t;
  int _ern=burm_decode_con[_s->rule.burm_con];
  NODEPTR *_children;
  if(_s->rule.burm_con==0)
    NO_ACTION(con);
  switch(_ern){
  case 6:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << "\t";
		std::cerr << burm_string[_ern] << "\n";
		cnst_action(_s,indent+1);
		printf("$%%0\n");// x86 对常量的引用需要前缀'$',con 代表常量,cnst 代表常量的值

	
}
  break;
  }
}


void con1_action(struct burm_state *_s, 

int indent)
{
  struct burm_state *_t;
  int _ern=burm_decode_con1[_s->rule.burm_con1];
  NODEPTR *_children;
  if(_s->rule.burm_con1==0)
    NO_ACTION(con1);
  switch(_ern){
  case 5:
{



		//本身没有代码生成，配合上层生成 ++, --等指令
	
}
  break;
  }
}


void rc_action(struct burm_state *_s, 

int indent)
{
  struct burm_state *_t;
  int _ern=burm_decode_rc[_s->rule.burm_rc];
  NODEPTR *_children;
  if(_s->rule.burm_rc==0)
    NO_ACTION(rc);
  switch(_ern){
  case 7:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << " ";
		std::cerr << burm_string[_ern] << "\n";
		con_action(_s,indent+1);
	
}
  break;
  case 8:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << " ";
		std::cerr << burm_string[_ern] << "\n";
		reg_action(_s,indent+1);
	
}
  break;
  }
}


void mem4_action(struct burm_state *_s, 

int indent)
{
  struct burm_state *_t;
  int _ern=burm_decode_mem4[_s->rule.burm_mem4];
  NODEPTR *_children;
  if(_s->rule.burm_mem4==0)
    NO_ACTION(mem4);
  switch(_ern){
  case 10:
{




		addr_action(_s->kids[0],1+indent);
		EM_CODE("%0");
	
}
  break;
  }
}


void mrc_action(struct burm_state *_s, 

int indent)
{
  struct burm_state *_t;
  int _ern=burm_decode_mrc[_s->rule.burm_mrc];
  NODEPTR *_children;
  if(_s->rule.burm_mrc==0)
    NO_ACTION(mrc);
  switch(_ern){
  case 11:
{




	mem4_action(_s,1+indent);

	EM_CODE("%0");

}
  break;
  case 12:
{




	rc_action(_s,1+indent);

	EM_CODE("%0");
	

}
  break;
  }
}


void mrca_action(struct burm_state *_s, 

int indent)
{
  struct burm_state *_t;
  int _ern=burm_decode_mrca[_s->rule.burm_mrca];
  NODEPTR *_children;
  if(_s->rule.burm_mrca==0)
    NO_ACTION(mrca);
  switch(_ern){
  case 13:
{



	mem4_action(_s,1+indent);
	EM_CODE("%0");

}
  break;
  case 14:
{



	rc_action(_s,1+indent);
	EM_CODE("%0");

}
  break;
  case 15:
{



	EM_CODE("$%a");

}
  break;
  }
}


void addr_action(struct burm_state *_s, 

int indent)
{
  struct burm_state *_t;
  int _ern=burm_decode_addr[_s->rule.burm_addr];
  NODEPTR *_children;
  if(_s->rule.burm_addr==0)
    NO_ACTION(addr);
  switch(_ern){
  case 24:
{



		int i;
		for (i = 0; i < indent; i++)
			std::cerr << " ";
		std::cerr << burm_string[_ern] << "\n";
		EM_CODE("%a(%%ebp)");
	
}
  break;
  }
}


void addrj_action(struct burm_state *_s, 

int indent)
{
  struct burm_state *_t;
  int _ern=burm_decode_addrj[_s->rule.burm_addrj];
  NODEPTR *_children;
  if(_s->rule.burm_addrj==0)
    NO_ACTION(addrj);
  switch(_ern){
  case 25:
{



	EM_CODE("%a");

}
  break;
  }
}
static void burm_closure_reg(struct burm_state *, COST);
static void burm_closure_cnst(struct burm_state *, COST);
static void burm_closure_con(struct burm_state *, COST);
static void burm_closure_rc(struct burm_state *, COST);
static void burm_closure_mem4(struct burm_state *, COST);
static void burm_closure_addr(struct burm_state *, COST);

static void burm_closure_reg(struct burm_state *s, COST c) {
  if(burm_cost_code(&c,8,s) && COST_LESS(c,s->cost[burm_rc_NT])) {
burm_trace(burm_np, 8, c);     s->cost[burm_rc_NT] = c ;
    s->rule.burm_rc = 2;
    burm_closure_rc(s, c );
  }
  if(burm_cost_code(&c,3,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 3, c);     s->cost[burm_stmt_NT] = c ;
    s->rule.burm_stmt = 4;
  }
}

static void burm_closure_cnst(struct burm_state *s, COST c) {
  if(burm_cost_code(&c,6,s) && COST_LESS(c,s->cost[burm_con_NT])) {
burm_trace(burm_np, 6, c);     s->cost[burm_con_NT] = c ;
    s->rule.burm_con = 1;
    burm_closure_con(s, c );
  }
}

static void burm_closure_con(struct burm_state *s, COST c) {
  if(burm_cost_code(&c,20,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 20, c);     s->cost[burm_reg_NT] = c ;
    s->rule.burm_reg = 6;
    burm_closure_reg(s, c );
  }
  if(burm_cost_code(&c,7,s) && COST_LESS(c,s->cost[burm_rc_NT])) {
burm_trace(burm_np, 7, c);     s->cost[burm_rc_NT] = c ;
    s->rule.burm_rc = 1;
    burm_closure_rc(s, c );
  }
}

static void burm_closure_rc(struct burm_state *s, COST c) {
  if(burm_cost_code(&c,14,s) && COST_LESS(c,s->cost[burm_mrca_NT])) {
burm_trace(burm_np, 14, c);     s->cost[burm_mrca_NT] = c ;
    s->rule.burm_mrca = 2;
  }
  if(burm_cost_code(&c,12,s) && COST_LESS(c,s->cost[burm_mrc_NT])) {
burm_trace(burm_np, 12, c);     s->cost[burm_mrc_NT] = c ;
    s->rule.burm_mrc = 2;
  }
}

static void burm_closure_mem4(struct burm_state *s, COST c) {
  if(burm_cost_code(&c,13,s) && COST_LESS(c,s->cost[burm_mrca_NT])) {
burm_trace(burm_np, 13, c);     s->cost[burm_mrca_NT] = c ;
    s->rule.burm_mrca = 1;
  }
  if(burm_cost_code(&c,11,s) && COST_LESS(c,s->cost[burm_mrc_NT])) {
burm_trace(burm_np, 11, c);     s->cost[burm_mrc_NT] = c ;
    s->rule.burm_mrc = 1;
  }
}

static void burm_closure_addr(struct burm_state *s, COST c) {
  if(burm_cost_code(&c,19,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 19, c);     s->cost[burm_reg_NT] = c ;
    s->rule.burm_reg = 5;
    burm_closure_reg(s, c );
  }
}

struct burm_state *burm_alloc_state(NODEPTR u,int op,int arity)
{
  struct burm_state *p, **k;
  p = (struct burm_state *)ALLOC(sizeof *p);
  burm_assert(p, PANIC("1:ALLOC returned NULL in burm_alloc_state\n"));
    burm_np = u;
  p->op = op;
  p->node = u;
  if(arity){
    k=(struct burm_state **)ALLOC(arity*sizeof (struct burm_state *));
    burm_assert(k, PANIC("2:ALLOC returned NULL in burm_alloc_state\n"));
    p->kids=k;
  }else
    p->kids=0;
  p->rule.burm_stmt =
  p->rule.burm_reg =
  p->rule.burm_cnst =
  p->rule.burm_con =
  p->rule.burm_con1 =
  p->rule.burm_rc =
  p->rule.burm_mem4 =
  p->rule.burm_mrc =
  p->rule.burm_mrca =
  p->rule.burm_addr =
  p->rule.burm_addrj =
  p->rule.burm__ =
    0;
  p->cost[1] =
  p->cost[2] =
  p->cost[3] =
  p->cost[4] =
  p->cost[5] =
  p->cost[6] =
  p->cost[7] =
  p->cost[8] =
  p->cost[9] =
  p->cost[10] =
  p->cost[11] =
  p->cost[12] =
    COST_INFINITY;
  return p;
}
struct burm_state *burm_label1(NODEPTR u) {
  int op, arity, i, immed_matched=0;
  COST c=COST_ZERO;
  struct burm_state *s,**k;
  NODEPTR *children;
  op=OP_LABEL(u);
  arity=burm_arity[op];
  switch(op){
  case 0:		/* BURP */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
        /*immediate rule: stmt: BURP(_,_,_) */
    if(burm_cost_code(&c,0,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 0, c);       s->cost[burm_stmt_NT] = c ;
      s->rule.burm_stmt = 1;
      immed_matched=1;
    }
    if(immed_matched){
      for(i=0;i<arity;i++)k[i]=0;
      return s;
    }
    break;
  case 1:		/* ADDI */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: ADDI(reg,con) */
      k[0]->rule.burm_reg && 
      k[1]->rule.burm_con
    ) {
      if(burm_cost_code(&c,21,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 21, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 7;
        burm_closure_reg(s, c );
      }
    }
    if (   /* reg: ADDI(reg,mrc) */
      k[0]->rule.burm_reg && 
      k[1]->rule.burm_mrc
    ) {
      if(burm_cost_code(&c,9,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 9, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 1;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 2:		/* ADDRLP */
#ifdef LEAF_TRAP
    if(s=LEAF_TRAP(u,op))
      return s;
#endif
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=0;
    {  		/* addr: ADDRLP */
      if(burm_cost_code(&c,24,s) && COST_LESS(c,s->cost[burm_addr_NT])) {
burm_trace(burm_np, 24, c);         s->cost[burm_addr_NT] = c ;
        s->rule.burm_addr = 1;
        burm_closure_addr(s, c );
      }
    }
    break;
  case 3:		/* ASGNI */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* stmt: ASGNI(addr,ADDI(mem4,con1)) */
      k[0]->rule.burm_addr && 
      k[1]->op == 1 && 	/* ADDI */
      k[1]->kids[0]->rule.burm_mem4&&
      k[1]->kids[1]->rule.burm_con1
    ) {
      if(burm_cost_code(&c,2,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 2, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 3;
      }
    }
    if (   /* stmt: ASGNI(addr,rc) */
      k[0]->rule.burm_addr && 
      k[1]->rule.burm_rc
    ) {
      if(burm_cost_code(&c,1,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 1, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 2;
      }
    }
    break;
  case 4:		/* CNSTI */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 5:		/* CVCI */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: CVCI(INDIRC(addr)) */
      k[0]->op == 7 && 	/* INDIRC */
      k[0]->kids[0]->rule.burm_addr
    ) {
      if(burm_cost_code(&c,16,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 16, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 2;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 6:		/* I0I */
#ifdef LEAF_TRAP
    if(s=LEAF_TRAP(u,op))
      return s;
#endif
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=0;
    {  		/* reg: I0I */
      if(burm_cost_code(&c,18,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 18, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 4;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 7:		/* INDIRC */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: INDIRC(addr) */
      k[0]->rule.burm_addr
    ) {
      if(burm_cost_code(&c,17,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 17, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 3;
        burm_closure_reg(s, c );
      }
    }
    if (   /* mem4: INDIRC(addr) */
      k[0]->rule.burm_addr
    ) {
      if(burm_cost_code(&c,10,s) && COST_LESS(c,s->cost[burm_mem4_NT])) {
burm_trace(burm_np, 10, c);         s->cost[burm_mem4_NT] = c ;
        s->rule.burm_mem4 = 1;
        burm_closure_mem4(s, c );
      }
    }
    break;
  case 8:		/* CNSTI4 */
#ifdef LEAF_TRAP
    if(s=LEAF_TRAP(u,op))
      return s;
#endif
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=0;
    {  		/* con1: CNSTI4 */
      if(burm_cost_code(&c,5,s) && COST_LESS(c,s->cost[burm_con1_NT])) {
burm_trace(burm_np, 5, c);         s->cost[burm_con1_NT] = c ;
        s->rule.burm_con1 = 1;
      }
    }
    {  		/* cnst: CNSTI4 */
      if(burm_cost_code(&c,4,s) && COST_LESS(c,s->cost[burm_cnst_NT])) {
burm_trace(burm_np, 4, c);         s->cost[burm_cnst_NT] = c ;
        s->rule.burm_cnst = 1;
        burm_closure_cnst(s, c );
      }
    }
    break;
  case 9:		/* ADDRGP4 */
#ifdef LEAF_TRAP
    if(s=LEAF_TRAP(u,op))
      return s;
#endif
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=0;
    {  		/* addrj: ADDRGP4 */
      if(burm_cost_code(&c,25,s) && COST_LESS(c,s->cost[burm_addrj_NT])) {
burm_trace(burm_np, 25, c);         s->cost[burm_addrj_NT] = c ;
        s->rule.burm_addrj = 1;
      }
    }
    {  		/* mrca: ADDRGP4 */
      if(burm_cost_code(&c,15,s) && COST_LESS(c,s->cost[burm_mrca_NT])) {
burm_trace(burm_np, 15, c);         s->cost[burm_mrca_NT] = c ;
        s->rule.burm_mrca = 3;
      }
    }
    break;
  case 10:		/* JUMPV */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* stmt: JUMPV(addrj) */
      k[0]->rule.burm_addrj
    ) {
      if(burm_cost_code(&c,26,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 26, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 5;
      }
    }
    break;
  case 11:		/* ARGI4 */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* stmt: ARGI4(mrca) */
      k[0]->rule.burm_mrca
    ) {
      if(burm_cost_code(&c,29,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 29, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 8;
      }
    }
    break;
  case 12:		/* MULI4 */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: MULI4(reg,mrc) */
      k[0]->rule.burm_reg && 
      k[1]->rule.burm_mrc
    ) {
      if(burm_cost_code(&c,22,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 22, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 8;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 13:		/* DIVI4 */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: DIVI4(reg,reg) */
      k[0]->rule.burm_reg && 
      k[1]->rule.burm_reg
    ) {
      if(burm_cost_code(&c,23,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 23, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 9;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 14:		/* EQI4 */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* stmt: EQI4(mem4,rc) */
      k[0]->rule.burm_mem4 && 
      k[1]->rule.burm_rc
    ) {
      if(burm_cost_code(&c,27,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 27, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 6;
      }
    }
    break;
  case 15:		/* GEI4 */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* stmt: GEI4(mem4,rc) */
      k[0]->rule.burm_mem4 && 
      k[1]->rule.burm_rc
    ) {
      if(burm_cost_code(&c,28,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 28, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 7;
      }
    }
    break;
  default:
    burm_assert(0, PANIC("Bad operator %d in burm_state\n", op));
  }
  return s;
}

struct burm_state *burm_label(NODEPTR p) {
  burm_label1(p);
  return ((struct burm_state *)STATE_LABEL(p))->rule.burm_stmt ? STATE_LABEL(p) : 0;
}

void burm_free(struct burm_state *s)
{
  int i,arity=burm_arity[s->op];
  if(s->kids==0)
    free(s);
  else {
    for(i=0;i<arity;i++)
      burm_free(s->kids[i]);
    free(s->kids);
  }
}
struct burm_state *burm_immed(struct burm_state *s,int n)
{
  NODEPTR *children = GET_KIDS(s->node);
  if(s->kids[n])
    return s->kids[n];
  else
  return s->kids[n]=burm_label1(children[n]);
}
int burm_op_label(NODEPTR p) {
  burm_assert(p, PANIC("NULL tree in burm_op_label\n"));
  return OP_LABEL(p);
}

struct burm_state *burm_state_label(NODEPTR p) {
  burm_assert(p, PANIC("NULL tree in burm_state_label\n"));
  return STATE_LABEL(p);
}

NODEPTR burm_child(NODEPTR p, int index) {
  NODEPTR *kids;
  burm_assert(p, PANIC("NULL tree in burm_child\n"));
  kids=GET_KIDS(p);
  burm_assert((0<=index && index<burm_arity[OP_LABEL(p)]),
    PANIC("Bad index %d in burm_child\n", index));

  return kids[index];
}
NODEPTR *burm_kids(NODEPTR p, int eruleno, NODEPTR kids[]) {
  burm_assert(p, PANIC("NULL tree in burm_kids\n"));
  burm_assert(kids, PANIC("NULL kids in burm_kids\n"));
  switch (eruleno) {
  case 0: /* stmt: BURP(_,_,_) */
    kids[0] = burm_child(p,0);
    kids[1] = burm_child(p,1);
    kids[2] = burm_child(p,2);
    break;
  case 28: /* stmt: GEI4(mem4,rc) */
  case 27: /* stmt: EQI4(mem4,rc) */
  case 23: /* reg: DIVI4(reg,reg) */
  case 22: /* reg: MULI4(reg,mrc) */
  case 21: /* reg: ADDI(reg,con) */
  case 9: /* reg: ADDI(reg,mrc) */
  case 1: /* stmt: ASGNI(addr,rc) */
    kids[0] = burm_child(p,0);
    kids[1] = burm_child(p,1);
    break;
  case 2: /* stmt: ASGNI(addr,ADDI(mem4,con1)) */
    kids[0] = burm_child(p,0);
    kids[1] = burm_child(burm_child(p,1),0);
    kids[2] = burm_child(burm_child(p,1),1);
    break;
  case 20: /* reg: con */
  case 19: /* reg: addr */
  case 14: /* mrca: rc */
  case 13: /* mrca: mem4 */
  case 12: /* mrc: rc */
  case 11: /* mrc: mem4 */
  case 8: /* rc: reg */
  case 7: /* rc: con */
  case 6: /* con: cnst */
  case 3: /* stmt: reg */
    kids[0] = p;
    break;
  case 25: /* addrj: ADDRGP4 */
  case 24: /* addr: ADDRLP */
  case 18: /* reg: I0I */
  case 15: /* mrca: ADDRGP4 */
  case 5: /* con1: CNSTI4 */
  case 4: /* cnst: CNSTI4 */
    break;
  case 29: /* stmt: ARGI4(mrca) */
  case 26: /* stmt: JUMPV(addrj) */
  case 17: /* reg: INDIRC(addr) */
  case 10: /* mem4: INDIRC(addr) */
    kids[0] = burm_child(p,0);
    break;
  case 16: /* reg: CVCI(INDIRC(addr)) */
    kids[0] = burm_child(burm_child(p,0),0);
    break;
  default:
    burm_assert(0, PANIC("Bad external rule number %d in burm_kids\n", eruleno));
  }
  return kids;
}

void dumpCover(NODEPTR p, int goalnt, int indent)
{
  int eruleno = burm_rule(STATE_LABEL(p), goalnt);
  short *nts = burm_nts[eruleno];
  NODEPTR kids[10];
  int i;

  std::cerr << "\t\t";
  for (i = 0; i < indent; i++)
    std::cerr << " ";
  std::cerr << burm_string[eruleno] << "\n";
  burm_kids(p, eruleno, kids);
  for (i = 0; nts[i]; i++)
    dumpCover(kids[i], nts[i], indent + 1);
}


#pragma GCC diagnostic pop



/* burm_trace - print trace message for matching p */
static void burm_trace(NODEPTR p, int eruleno, COST cost) {
	if (shouldTrace)
		std::cerr << "0x" << p << " matched " << burm_string[eruleno] << " = " << eruleno << " with cost " << cost.cost << "\n";
}

 void gen(NODEPTR p) {
	if (burm_label(p) == 0)
		std::cerr << "no cover\n";
	else {
		stmt_action(p->x.state,0);
		if (shouldCover != 0)
			dumpCover(p, 1, 0);
	}
}

 Tree tree(int op, Tree l, Tree r) {
	Tree t = (Tree) malloc(sizeof *t);

	t->op = op;
	t->kids[0] = l; t->kids[1] = r;
	t->val = 0;
	t->x.state = 0;
	return t;
}
extern void emit_code();
extern void register_alloc();

int tmain(int argc, char *argv[]) {
	Tree t;
	int i;

	for (i = 1; i < argc ; ++i) {
		if (strcmp(argv[i], "--cover") == 0){
			shouldCover = 1;
		}
		if (strcmp(argv[i], "--trace") == 0){
			shouldTrace = 1;
		}
	}
	printf("i = c + 4;\n");
	t = tree(ASGNI,
		tree(ADDRLP, 0, 0),
		tree(ADDI,
			//tree(CVCI, tree(INDIRC, tree(ADDRLP, 0, 0), 0), 0),
			tree(INDIRC, tree(ADDRLP, 0, 0), 0),
			(t = tree(CNSTI4, 0, 0), t->val = 4, t)
			//tree(INDIRC, tree(ADDRLP, 0, 0), 0)
		)
	);
	gen(t);
	register_alloc();
	emit_code();
	return 0;
}
