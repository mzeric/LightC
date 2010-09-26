%{
	
//extern "C"
#include <stdio.h>
	
extern "C" void A_EXP_(int,int,char*);
extern "C" int yyparse(void);
	

extern "C" int yyerror(char*s){

	printf("error:%s\n",s);
	
}

extern "C" int yylex(void);
%}


%union {int num; char* id;}
%token IDENTIFIER CONSTANT STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token <num> CONSTANT_INT
%token <id>  ID
%token END LP RP
%type  <num> exp
%start START 

%left PLUS MINUS
%left TIMES
%left UMINUS

%%
START:
	|exp END {printf("exp value is :%d\n",$1);}
	;
/*
primary_exp
	: IDENTIFIER {;}
	| CONSTANT_INT {$$ = $1;}
	|LP primary_exp RP {$$ = $2;}
	;
*/
exp
	:CONSTANT_INT{$$ = $1;}
	|exp '+' exp {$$ = $1+$3;A_EXP_($1,$3,"add");}
	|exp '-' exp {$$ = $1 - $3;A_EXP_($1,$3,"sub");}
	|exp '*' exp {$$ = $1 * $3;A_EXP_($1,$3,"mul");}
	|'-' exp %prec UMINUS {$$ = - $2;A_EXP_($2,NULL,"-");}
	;

%%
extern "C"{
int yywrap(void){

		return 1;
		}

	
}

