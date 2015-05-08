D                       [0-9]
L                       [a-zA-Z_]
H                       [a-fA-F0-9]
E                       [Ee][+-]?{D}+
P                       [Pp][+-]?{D}+
FS                      (f|F|l|L)
IS                      ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))

%option yylineno

%{
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "anode.h"
#include "parse.hh"
#include "lexer.h"


void count(void);
void comment(void);
int column = 0;
int line = 1;
#define LINE_LEN 1024
char line_buf[LINE_LEN+1];
int line_len = 0;

%}

%%
"/*"                    { comment(); }
"//"[^\n]*              { /* consume //-comment */ }


"auto"                  { count(); return(AUTO); }
"_Bool"                 { count(); return(BOOL); }
"break"                 { count(); return(BREAK); }
"case"                  { count(); return(CASE); }
"char"                  { count(); return(CHAR);}
"_Complex"              { count(); return(COMPLEX); }
"const"                 { count(); return(CONST); }
"continue"              { count(); return(CONTINUE); }
"default"               { count(); return(DEFAULT); }
"do"                    { count(); return(DO); }
"double"                { count(); return(DOUBLE); }
"else"                  { count(); return(ELSE); }
"enum"                  { count(); return(ENUM); }
"extern"                { count(); return(EXTERN); }
"float"                 { count(); return(FLOAT); }
"for"                   { count(); return(FOR); }
"goto"                  { count(); return(GOTO); }
"if"                    { count(); return(IF); }
"_Imaginary"            { count(); return(IMAGINARY); }
"inline"                { count(); return(INLINE); }
"int"                   { count(); return(INT); }
"long"                  { count(); return(LONG); }
"register"              { count(); return(REGISTER); }
"restrict"              { count(); return(RESTRICT); }
"return"                { count(); return(RETURN_ID); }
"short"                 { count(); return(SHORT); }
"signed"                { count(); return(SIGNED); }
"sizeof"                { count(); return(SIZEOF); }
"static"                { count(); return(STATIC); }
"struct"                { count(); return(STRUCT); }
"switch"                { count(); return(SWITCH); }
"typedef"               { count(); return(TYPEDEF); }
"union"                 { count(); return(UNION); }
"unsigned"              { count(); return(UNSIGNED); }
"void"                  { count(); return(VOID); }
"volatile"              { count(); return(VOLATILE); }
"while"                 { count(); return(WHILE); }

{L}({L}|{D})*           { count();
                                /* 
                                        type vs variable name-ambiguity 
                                        handled by lookup Symbol-Table
                                        so, we need a declcontext;
                                */
                                yylval.id = strdup(yytext);

                                return(IDENTIFIER);
                        }
0[xX]{H}+{IS}?          { count(); return(CONSTANT); }
0{D}+{IS}?              { count(); return(CONSTANT); }
{D}+{IS}?               { count();
				yylval.num = atoi(yytext);
                                return(CONSTANT_INT);
                        }
L?'(\\.|[^\\'\n])+'     { count(); return(CONSTANT); }

{D}+{E}{FS}?            { count(); return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?  { count(); 
//				yylval.fnum = atof(yytext);
				return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?  { count(); 
//				yylval.fnum = atof(yytext);
				return(CONSTANT); }
0[xX]{H}+{P}{FS}?               { count(); return(CONSTANT); }
0[xX]{H}*"."{H}+({P})?{FS}?     { count(); return(CONSTANT); }
0[xX]{H}+"."{H}*({P})?{FS}?     { count(); return(CONSTANT); }

L?\"(\\.|[^\\"\n])*\" 	{ count();
				yylval.id = strdup(yytext);
				return(STRING_LITERAL); }


"..."                   { count(); return(ELLIPSIS); }
">>="                   { count(); return(RIGHT_ASSIGN); }
"<<="                   { count(); return(LEFT_ASSIGN); }
"+="                    { count(); return(ADD_ASSIGN); }
"-="                    { count(); return(SUB_ASSIGN); }
"*="                    { count(); return(MUL_ASSIGN); }
"/="                    { count(); return(DIV_ASSIGN); }
"%="                    { count(); return(MOD_ASSIGN); }
"&="                    { count(); return(AND_ASSIGN); }
"^="                    { count(); return(XOR_ASSIGN); }
"|="                    { count(); return(OR_ASSIGN); }
">>"                    { count(); return(RIGHT_OP); }
"<<"                    { count(); return(LEFT_OP); }
"++"                    { count(); return(INC_OP); }
"--"                    { count(); return(DEC_OP); }
"->"                    { count(); return(PTR_OP); }
"&&"                    { count(); return(AND_OP); }
"||"                    { count(); return(OR_OP); }
"<="                    { count(); return(LE_OP); }
">="                    { count(); return(GE_OP); }
"=="                    { count(); return(EQ_OP); }
"!="                    { count(); return(NE_OP); }
";"                     { count(); return(';'); }
("{"|"<%")              { count(); return(LEFT_BRACE);/* '{' */ }
("}"|"%>")              { count(); return(RIGHT_BRACE)/* '}' */; }
","                     { count(); return(','); }
":"                     { count(); return(':'); }
"="                     { count(); //yylval.id = yytext;
                                return(ASSIGN); 
                        }
"("                     { count(); return('('); }
")"                     { count(); return(')'); }
("["|"<:")              { count(); return('['); }
("]"|":>")              { count(); return(']'); }
"."                     { count(); return('.'); }
"&"                     { count(); return('&'); }
"!"                     { count(); return('!'); }
"~"                     { count(); return('~'); }
"-"                     { count(); return('-'); }
"+"                     { count(); return('+'); }
"*"                     { count(); return('*'); }
"/"                     { count(); return('/'); }
"%"                     { count(); return('%'); }
"<"                     { count(); return('<'); }
">"                     { count(); return('>'); }
"^"                     { count(); return('^'); }
"|"                     { count(); return('|'); }
"?"                     { count(); return('?'); }

[ \t\v\n\f]             { count(); }
.                       { /* Add code to complain about unmatched characters */ }

%%



#ifdef __cplusplus
#define LEX_INPUT yyinput
#else
#define LEX_INPUT input
#endif
extern int yyinput();
extern "C" void yyerror(char const *s)
{
	fflush(stdout);
	printf("Error: %s\n", s);
	int c = printf("%d:%d ", yylineno, column);
	printf("%s\n", line_buf);
	printf("%*s\n",column , "^");
}
int yywrap(){
        return 1;
}
void count(void)
{
        int i;

        for (i = 0; yytext[i] != '\0'; i++){
                if (yytext[i] == '\n'){
                        column = 0;
			line++;
			line_len = 0;
			line_buf[line_len] = 0;
                }
                else if (yytext[i] == '\t'){
                        column += 8 - (column % 8);
                        line_buf[line_len++] = yytext[i];
                }
                else{
                        column++;
                        line_buf[line_len++] = yytext[i];
                }

        }
        line_buf[line_len] = 0;
#ifdef LVC_DEBUG
        ECHO;
#endif
}


void comment(void)
{
        char c, prev = 0;
        while ((c = LEX_INPUT()) != 0)      /* (EOF maps to 0) */
        {
                if (c == '/' && prev == '*')
                        return;
                prev = c;
        }
        yyerror("unterminated comment");
}
