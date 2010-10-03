%{

#include <stdio.h>
#include "common.h"
	
//#define INT2VALUE(x) ConstantInt::get(getGlobalContext(),APInt(32,(x)))
extern "C" void A_EXP_(int,int,char*);
extern "C" int yyparse(void);
	


extern "C" int yylex(void);
extern char yytext[];
extern int column;

extern "C" void yyerror(char const *s)
{
	fflush(stdout);
	printf("%d,\n%*s\n%*s\n",column, column, "^", column, s);
}
void check(const char*str){

	printf(" [%s] ",str);

}


%}


%union {
    int num;
    char* id;
    Value* value;
    std::string *string;
    std::vector<Fstring> *parameter; 

    AST_expr    *ast_expr;
    AST_declare *ast_declare;
    AST_args    *ast_args;
    AST_func	*ast_func;  //func definition
    
    
}
%token CONSTANT STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token TYPEDEF EXTERN STATIC AUTO REGISTER INLINE RESTRICT
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token BOOL COMPLEX IMAGINARY
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%start translation_unit

%token <num> CONSTANT_INT
%token <id> IDENTIFIER
%type <ast_func> function_definition
%type <ast_declare> declarator
%type <ast_declare> direct_declarator
%type <ast_args>   parameter_type_list
%type <ast_args>   parameter_list
%type <ast_declare>   parameter_declaration
%type <ast_expr>    compound_statement 

%type <ast_expr>    primary_expression
%type <ast_expr>    postfix_expression 
%type <ast_expr>    argument_expression_list 
%type <ast_expr>    unary_expression 
%type <ast_expr>    cast_expression 
%type <ast_expr>    multiplicative_expression
%type <ast_expr>    additive_expression
%type <ast_expr>    shift_expression
%type <ast_expr>    relational_expression
%type <ast_expr>    equality_expression
%type <ast_expr>    and_expression
%type <ast_expr>    exclusive_or_expression
%type <ast_expr>    inclusive_or_expression
%type <ast_expr>    logical_and_expression
%type <ast_expr>    logical_or_expression
%type <ast_expr>    conditional_expression
%type <ast_expr>    assignment_expression
%type <ast_expr>    expression
%type <ast_expr>    constant_expression



%%

primary_expression
	: IDENTIFIER {check("identifier");}
	| CONSTANT_INT {printf(" [CONSTANT_INT] ");}
	| STRING_LITERAL{printf(" [literal] ");}
	| '(' expression ')'{printf(" [括号] ");}
	;

postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	| '(' type_name ')' '{' initializer_list '}'
	| '(' type_name ')' '{' initializer_list ',' '}'
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	;

unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression
	: unary_expression
	| '(' type_name ')' cast_expression
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression '*' cast_expression
	| multiplicative_expression '/' cast_expression
	| multiplicative_expression '%' cast_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression {check("计算_加法");}
	| additive_expression '-' multiplicative_expression
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression{printf(" [赋值表达式] ");}
	;

assignment_operator
	: '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression
	: assignment_expression
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression
	;

declaration
	: declaration_specifiers ';' {;}//声明前缀 类型 int 等
	| declaration_specifiers init_declarator_list ';' {;} //完整声明 包含变量名 或 初值
	;

declaration_specifiers
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier {;}//int long etc
	| type_specifier declaration_specifiers {;}
	| type_qualifier
	| type_qualifier declaration_specifiers
	| function_specifier
	| function_specifier declaration_specifiers
	;

init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
	;

init_declarator
	: declarator  {check("声明");}
	| declarator '=' initializer {check("声明_assign");}
	;

storage_class_specifier
	: TYPEDEF
	| EXTERN
	| STATIC
	| AUTO
	| REGISTER
	;

type_specifier
	: VOID
	| CHAR
	| SHORT
	| INT
	| LONG
	| FLOAT
	| DOUBLE
	| SIGNED
	| UNSIGNED
	| BOOL
	| COMPLEX
	| IMAGINARY
	| struct_or_union_specifier
	| enum_specifier
	| TYPE_NAME
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
	;

struct_or_union
	: STRUCT
	| UNION
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM '{' enumerator_list ',' '}'
	| ENUM IDENTIFIER '{' enumerator_list ',' '}'
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: IDENTIFIER
	| IDENTIFIER '=' constant_expression
	;

type_qualifier
	: CONST
	| RESTRICT
	| VOLATILE
	;

function_specifier
	: INLINE
	;

declarator
	: pointer direct_declarator
	| direct_declarator {
	$$ = $1;
	}
	;


direct_declarator
	: IDENTIFIER {check("identifier_declartor");
		$$ = new AST_declare;
		$$->set_name($1);
	}
	| '(' declarator ')' {check("1");}
	| direct_declarator '[' type_qualifier_list assignment_expression ']'{check("1");}
	| direct_declarator '[' type_qualifier_list ']'{check("1");}
	| direct_declarator '[' assignment_expression ']'{check("1");}
	| direct_declarator '[' STATIC type_qualifier_list assignment_expression ']'{check("1");}
	| direct_declarator '[' type_qualifier_list STATIC assignment_expression ']'{check("1");}
	| direct_declarator '[' type_qualifier_list '*' ']'{check("1");}
	| direct_declarator '[' '*' ']'{check("1");}
	| direct_declarator '[' ']'{check("1");}
	| direct_declarator '(' parameter_type_list ')' {check("函数_声明");
			$$ = new AST_proto($1->get_name(),$3->get_args());
	
	}
	| direct_declarator '(' identifier_list ')'{check("1");}
	| direct_declarator '(' ')'{check("1");}
	;

pointer
	: '*'
	| '*' type_qualifier_list
	| '*' pointer
	| '*' type_qualifier_list pointer
	;

type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;


parameter_type_list
	: parameter_list {
	$$ = $1;
	}
	| parameter_list ',' ELLIPSIS {$$ = $1;}
	;

parameter_list
	: parameter_declaration {
			$$ = new AST_args;
			$$->add_args($1->get_name());
	}
	
	| parameter_list ',' parameter_declaration{
			$$->add_args($3->get_name());
	}
	;

parameter_declaration
	: declaration_specifiers declarator{check("参数");// 类型 + 变量
	$$ =  $2;
			//$2[strlen($2)-1]='\0';
	}
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' assignment_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' assignment_expression ']'
	| '[' '*' ']'
	| direct_abstract_declarator '[' '*' ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;

initializer
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;

initializer_list
	: initializer
	| designation initializer
	| initializer_list ',' initializer
	| initializer_list ',' designation initializer
	;

designation
	: designator_list '='
	;

designator_list
	: designator
	| designator_list designator
	;

designator
	: '[' constant_expression ']'
	| '.' IDENTIFIER
	;

statement
	: labeled_statement  {check("块_标签");}
	| compound_statement
	| expression_statement {check("块_表达式");}
	| selection_statement  {check("块_选择IF/SWITCH");}
	| iteration_statement  {check("块_循环");}
	| jump_statement  {check("块_跳转");}
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

compound_statement
	: '{' '}'
	| '{' block_item_list '}'
	;

block_item_list
	: block_item
	| block_item_list block_item
	;

block_item
	: declaration
	| statement
	;

expression_statement
	: ';'
	| expression ';'
	;

selection_statement
	: IF '(' expression ')' statement
	| IF '(' expression ')' statement ELSE statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	| FOR '(' declaration expression_statement ')' statement
	| FOR '(' declaration expression_statement expression ')' statement
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;

translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition
	| declaration
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement{check("函数_define1");



 }
	| declaration_specifiers declarator compound_statement{check("函数_define2");

//	$2->code();

/*		std::vector<Fstring> my_args;
		my_args.push_back("first");
		my_args.push_back("second");
		AST_integer *vint = new AST_integer(456);
		AST_var *my_var   = new AST_var("first");
		AST_proto* my_proto = new AST_proto("my_func",my_args);

*/
		AST_var *my_var = new AST_var("first");
        AST_func * my_func  = new AST_func ((AST_proto*)$2,my_var);
		std::vector<AST_expr*> my_call_args;
		my_call_args.push_back(new AST_integer(456));
		my_call_args.push_back(new AST_integer(789));
		AST_call * my_call = new AST_call("test_func",my_call_args);
		my_func->code();

        $$ = my_func;
 } //修饰[类型] func(参数表)  语句 
	;

declaration_list
	: declaration
	| declaration_list declaration
	;


%%



extern "C"{
    int yywrap(void){

		return 1;
    }
    
	

}
