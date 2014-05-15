#ifndef _LEXER_H
#define _LEXER_H
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" int yylex(void);
extern int yyparse(void);
extern int column;
//char *yytext;
//size_t yyleng;
//extern "C" FILE *yyin, *yyout;
//extern "C" int yylex(void);
//extern "C" File* yyin, *yyout;

//extern void *token_value;
//extern unsigned token_leng;

#endif
