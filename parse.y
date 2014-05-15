

%{

#include "anode.h"
#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <vector>
#include <string>

#define YYDEBUG 1
extern "C" void A_EXP_(int,int,char*);
extern int yyparse(void);
extern "C" char *yytext();
	
typedef anode ast_node;
using namespace std;

extern "C" int yylex(void);

static int parameter_list_num = 0;

extern "C" void yyerror(char const *s)
{
	fflush(stdout);
	printf("%d,\n%*s\n%*s\n",column, column, "^", column, s);
}
extern "C" void check(const char *msg, ...){
	va_list vp;
	char *buf = NULL;
	va_start(vp, msg);
	asprintf(&buf, "%s\n", msg);
	vprintf(buf, vp);
	free(buf);
	va_end(vp);
}

#define YYERROR1 { yyerror("syntax error"); YERROR;}
%}


%union {
    int num;
    double fnum;
    char* id;
    long lhook;
    std::string *str;
    std::vector<std::string> *parameter;
    anode		ast_node;
    anode		ast_args;
    anode		ast_declarator;
    anode		ast_declaration;
    anode		ast_type;
    anode		ast_qual;
    anode 		ast_expr_list;
    anode 		ast_list;
/*
    ASTType		*ast_type;
    QualType	*ast_qual;
    StorType	*ast_stor;
    AST_expr_list    *ast_expr_list;
    AST_decl 	*ast_decl;
    AST_declarator *ast_declarator;
    Declaration *ast_declaration;
    AST_args    *ast_args;
    AST_func	*ast_func;  //func definition
*/
    
    
}
%token CONSTANT SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token LEFT_BRACE RIGHT_BRACE

%token TYPEDEF EXTERN STATIC AUTO REGISTER INLINE RESTRICT
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token BOOL COMPLEX IMAGINARY
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN_ID
%token ASSIGN

%start translation_unit

%token <num> CONSTANT_INT
%token <id> IDENTIFIER
%token <id> STRING_LITERAL


%type <num> assignment_operator 
%type <ast_node> 	function_definition
%type <ast_node> 	declarator
%type <ast_declarator> 	direct_declarator
%type <ast_args>   	parameter_type_list
%type <ast_args>   	parameter_list
%type <ast_declaration>	parameter_declaration
%type <ast_node>    compound_statement 

%type <ast_node>    primary_expression
%type <ast_node>    postfix_expression 
%type <ast_node>    argument_expression_list 
%type <ast_node>    unary_expression 
%type <ast_node>    cast_expression 
%type <ast_node>    multiplicative_expression
%type <ast_node>    additive_expression
%type <ast_node>    shift_expression
%type <ast_node>    relational_expression
%type <ast_node>    equality_expression
%type <ast_node>    and_expression
%type <ast_node>    exclusive_or_expression
%type <ast_node>    inclusive_or_expression
%type <ast_node>    logical_and_expression
%type <ast_node>    logical_or_expression
%type <ast_node>    conditional_expression
%type <ast_node>    assignment_expression
%type <ast_node>    expression
%type <ast_node>    constant_expression

%type <ast_declarator> init_declarator
%type <ast_list> init_declarator_list

%type <ast_node>    initializer
%type <ast_node>    statement
%type <ast_node>    selection_statement
%type <ast_node>    expression_statement
%type <ast_node>    block_item
%type <ast_expr_list>    block_item_list

%type <ast_declaration>    declaration
%type <ast_type>	declaration_specifiers
%type <ast_type>	type_specifier
%type <ast_qual>	type_qualifier
%type <ast_qual>	type_qualifier_list
%expect 26
%{

/* something help parsing */
static anode global_space;
static anode current_declspaces;
static anode declspace_stack;

void push_namespace(void){
	declspace_stack = anode_cons(NULL, current_declspaces, declspace_stack);
}
void pop_namespace(void){
	current_declspaces = ANODE_VALUE(declspace_stack);
	declspace_stack = ANODE_CHAIN(declspace_stack);
}
void push_decl(anode decl){
	/* only need to move the current_decl */
	anode_decl *d = ANODE_(ANODE_CLASS_CHECK(decl, 'd'), anode_decl);
	current_declspaces = anode_cons(NULL, d, current_declspaces);
}
typedef void (*ANODE_DECL_CALLBACK)(anode);

void for_each_decl(ANODE_DECL_CALLBACK f){
	anode a;
	for (a = current_declspaces; a; a = ANODE_CHAIN(a)){
		f(ANODE_VALUE(a));
	}
	
}
#define FOR_EACH_DECL(T) 		\
	for (anode T = current_declspaces; T; T = ANODE_CHAIN(T))
int compare_name(anode node, const char *name){
	char *p = IDENTIFIER_POINTER(decl_name((node)));
	return strcmp(p, name);
}
void print_decl(anode node){
	if(!node)
		printf("type: null\tname: null\n");
	else
	printf("type: %s\tname: %s\n",
		anode_code_name(anode_code(ANODE_DECL_TYPE(node))),
		IDENTIFIER_POINTER(decl_name(node)));
}
anode lookup_name(const char *name){
	FOR_EACH_DECL(decl){
		if(compare_name(ANODE_VALUE(decl), name) == 0)
			return ANODE_VALUE(decl);
	}
	return NULL;
}
int is_type_or_identifier(const char *str){
	/* return 1 if is type */
	return 0;
}
void c_parse_init(void){

	global_space = new anode_decl();
	current_declspaces = NULL;
	push_namespace(); /* install the top-global namespace */

}
%}

%%

primary_expression /* operand of a expression */
	: IDENTIFIER {
		check("(identifier %s)", $1);
		$$ = new anode_identifier($1);
		/*  should be a ref */
		free($1);
		//引用了一个变量(局部),
		//此处是往AST里加入一个变量引用节点
	}
	| CONSTANT_INT {
		check("CONSTANT_INT %d", $1);
		$$ = new anode_int_cst($1);

		//printf("%X ",atoi(yytext));
	}
	| STRING_LITERAL{
		check("literal");

		//$$ = new AST_literal($1);
		//delete $1; /* free the lexer's std::string* */
	}
	| '(' expression ')'{check("括号");}
	;

postfix_expression
	: primary_expression {
		$$ = $1;
	}
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	| '(' type_name ')' LEFT_BRACE initializer_list RIGHT_BRACE
	| '(' type_name ')' LEFT_BRACE initializer_list ',' RIGHT_BRACE
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression{
		$$ = $1;
	}
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
	: unary_expression{
		$$ = $1;
	}
	| '(' type_name ')' cast_expression
	;

multiplicative_expression
	: cast_expression{
		$$ = $1;
	}
	| multiplicative_expression '*' cast_expression {
		//$$ = new AST_bin('*', $1, $3);
	}
	| multiplicative_expression '/' cast_expression {
		//$$ = new AST_bin('/', $1, $3);
	}
	| multiplicative_expression '%' cast_expression {
		//$$ = new AST_bin('%', $1, $3);
	}
	;

additive_expression
	: multiplicative_expression{
		$$ = $1;
	}
	| additive_expression '+' multiplicative_expression {
		//$$ = new AST_bin('+', $1, $3);
	}
	| additive_expression '-' multiplicative_expression {
		//$$ = new AST_bin('-', $1, $3);
	}
	;

shift_expression
	: additive_expression{
		$$ = $1;
	}
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression{
		$$ = $1;
	}
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression
	: relational_expression{
		$$ = $1;
	}
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;

and_expression
	: equality_expression{
		$$ = $1;
	}
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression{
		$$ = $1;
	}
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression{
		$$ = $1;
	}
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression{
		$$ = $1;
	}
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression{
		$$ = $1;
	}
	| logical_or_expression OR_OP logical_and_expression
	;

conditional_expression
	: logical_or_expression {
		$$ = $1;
	}
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression
	: conditional_expression {
		check("赋值expr");
		$$ = $1;
	}
	| unary_expression assignment_operator assignment_expression {
		check("赋值表达式");
	
		//$$ = new AST_assignment($1,$3);
	
		
		//printf("%X ",$$);
	}
	;

assignment_operator
	: ASSIGN { 
	$$ = '=';
	}
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
	: assignment_expression  {
		$$ = $1;

	}
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression
	;

declaration
	: declaration_specifiers ';' {
		//$$ = new Declaration($1, NULL);
	}//声明前缀 类型 int 等
	| declaration_specifiers  init_declarator_list ';' {
		anode t;
		for (t = $2; t; t = ANODE_CHAIN(t)){
			ANODE_TYPE(ANODE_VALUE(t)) = $1;
			anode d = build_decl($1, ANODE_VALUE(t));
			push_decl(d);
			check("Full 声明 (%s %s = c)",	
				anode_code_name(anode_code(ANODE_TYPE(ANODE_VALUE(t)))),
				IDENTIFIER_POINTER(decl_name(ANODE_VALUE(t))),current_declspaces);

		}
		/* 从 decl_specifiers里提取type_specifier */
		
	} //完整声明 包含变量名 或 初值 、类型
	;
hook_set :{printf("hook_set\n");};

declaration_specifiers   // 各种声明类型
	: storage_class_specifier{

	}
	| storage_class_specifier declaration_specifiers
	| type_specifier {
		/* int var 里的 int */
		$$ = $1;
		
	}//int long etc or int const
	| type_specifier declaration_specifiers {
		//$2->type = $1;
		//$$ = $2;
		std::cout << "hello int ***" << std::endl;
	}
	| type_qualifier {
		//$$ = new QualType($1);
		//std::cout << "isConst: " << $1->hasConst() << std::endl;
	}
	| type_qualifier declaration_specifiers{

		//$2->addQualifier(*$1);
		$$ = $2;

		//delete $1;
	}
	| function_specifier
	| function_specifier declaration_specifiers
	;

init_declarator_list	
	: init_declarator {
		$$ = build_list(NULL, $1);


	}
	| init_declarator_list ',' init_declarator{
		anode t = build_list(NULL, $3);

		$$ = chain_cat($1, t);

	}
	;

init_declarator
	: declarator  {
		$$ = $1;
		ANODE_($$, anode_decl)->initial = NULL;
		check("one declarator");
		//$$ = new AST_decl_var($1, NULL);
	}
	| declarator ASSIGN initializer {
		check("one declarator+assign");
		$$ = $1;
		ANODE_($$, anode_decl)->initial = $3;
		// if initializer is constant, just store within the Fvalue $->ir;
		//$$ = new AST_decl_var($1, $3);
	}
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
	| CHAR {
		//$$ = new BuiltinType(BuiltinType::Char);
	}
	| SHORT
	| INT {
		$$ = new anode_type(INTEGER_TYPE);
		//$$ = new BuiltinType(BuiltinType::Integer);
		//$$->ast_type = llvm::Type::getInt32Ty(*llvm_context);
		//std::cout << "hello type_specifier" << std::endl;
	}
	| LONG
	| FLOAT {
//		$$ = new ASTType();
//		$$->ast_type = llvm::Type::getFloatTy(*llvm_context);
	}
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
	: struct_or_union IDENTIFIER LEFT_BRACE struct_declaration_list RIGHT_BRACE
	| struct_or_union LEFT_BRACE struct_declaration_list RIGHT_BRACE
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
	: ENUM LEFT_BRACE enumerator_list RIGHT_BRACE
	| ENUM IDENTIFIER LEFT_BRACE enumerator_list RIGHT_BRACE
	| ENUM LEFT_BRACE enumerator_list ',' RIGHT_BRACE
	| ENUM IDENTIFIER LEFT_BRACE enumerator_list ',' RIGHT_BRACE
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: IDENTIFIER
	| IDENTIFIER ASSIGN constant_expression
	;

type_qualifier
	: CONST{
		//$$ = new QualType(QualType::Qualifier_Const);
	}
	| RESTRICT{
		//$$ = new QualType(QualType::Qualifier_Restrict);
	}
	| VOLATILE{
		//$$ = new QualType(QualType::Qualifier_Volatile);
	}
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


direct_declarator /* 代表所有声明语句中的变量 */
	: IDENTIFIER {
		anode_decl *t = new anode_decl();
		t->name = new anode_identifier($1);
		t->code = VAR_DECL;
		$$ = t;
		check("(identifier_declartor %s)", $1);
		free($1);

	}
	| '(' declarator ')' {
		check("函数指针_declarator");
	}/* 下面都是数组声明 */
	| direct_declarator '[' type_qualifier_list assignment_expression ']'{check("1");}
	| direct_declarator '[' type_qualifier_list ']'{check("1");}
	| direct_declarator '[' assignment_expression ']'{check("1");}
	| direct_declarator '[' STATIC type_qualifier_list assignment_expression ']'{check("1");}
	| direct_declarator '[' type_qualifier_list STATIC assignment_expression ']'{check("1");}
	| direct_declarator '[' type_qualifier_list '*' ']'{check("1");}
	| direct_declarator '[' '*' ']'{check("1");}
	| direct_declarator '[' ']'{check("1");}
	| direct_declarator '(' parameter_type_list ')' {
		check("函数_declarator");
	
		// AST_proto 复杂添加符号表
		//$$ = new AST_proto($1->get_name(),$3->get_args());
		//$$->context = $3->context;

	
	}
	| direct_declarator '(' identifier_list ')'{check("1");}
	| direct_declarator '(' ')'{
	
		check("1");
	}
	;

pointer
	: '*'
	| '*' type_qualifier_list
	| '*' pointer
	| '*' type_qualifier_list pointer
	;

type_qualifier_list
	: type_qualifier{
		$$ = $1;
	}
	| type_qualifier_list type_qualifier{
		/*
			Only Support Last One Qualifier
		*/
		$$ = $2;
	}
	;


parameter_type_list
	: parameter_list {
		$$ = $1;
		//$$->context = $1->context;
		//parameter_list_num = 0;
	}
	| parameter_list ',' ELLIPSIS {$$ = $1;}
	;

parameter_list
	: parameter_declaration {
	check("Push SymbolTable 函数参数>>>");
			
			/*
			$$ = new AST_args;
			$$->add_args($1->declarator->get_name(), $1->getInfo());
			parameter_list_num++;
			$$->context = $1->context;
			*/
	}
	
	| parameter_list ',' parameter_declaration{
	/*
			$$->add_args($3->declarator->get_name(), $3->getInfo());
			parameter_list_num++;
			$$->context = $1->context;
			*/
	}
	;

parameter_declaration
	: declaration_specifiers declarator{
			check("参数");// 类型 + 变量
		/*
			ASTContext *c = NULL;
			if(parameter_list_num == 0){
				c = new ASTContext();
				c->Push();
			}
			$$ =  new Declaration($1, $2);
			if(parameter_list_num == 0 ){
				assert(c && "proto-paramlist always avaliable");
				$$->context = c;
			}
		*/
			//$2[strlen($2)-1]='\0';
	}
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name /* 去掉变量的声明，仅剩下类型信息 */
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator /* 就是* */
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

initializer /* =的右端，或struct内的初始化 */
	: assignment_expression{
		$$ = $1;
	}
	| LEFT_BRACE initializer_list RIGHT_BRACE
	| LEFT_BRACE initializer_list ',' RIGHT_BRACE
	;

initializer_list
	: initializer
	| designation initializer
	| initializer_list ',' initializer
	| initializer_list ',' designation initializer
	;

designation
	: designator_list ASSIGN
	;

designator_list
	: designator
	| designator_list designator
	;

designator 
	: '[' constant_expression ']'
	| '.' IDENTIFIER /* C++ 已经废弃了这种.name = xx的赋值方式 */
	;

statement
	: labeled_statement  {check("块_标签");}
	| compound_statement {
		check("块_{}"); 
		$$ = $1;
	}
	| expression_statement {check("块_表达式");
		$$ = $1;
	}
	| selection_statement  {
		check("块_选择IF/SWITCH");
		$$ = $1;
	}
	| iteration_statement  {check("块_循环");}
	| jump_statement  {check("块_跳转");}
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

compound_statement
	: LEFT_BRACE RIGHT_BRACE{
		check(" { }");
	}
	| LEFT_BRACE {
			printf("push_namespace\n");
			push_namespace();
	
		}
		block_item_list RIGHT_BRACE {

		$$ = $3;
		//$$->context = $1;
		//for_each_decl(print_decl);
		pop_namespace();

	}
	;

block_item_list
	: block_item  { 

		$$ = build_list(NULL, $1);
	}
	| block_item_list block_item{

		$$ = anode_cons(NULL, $2, $1);
	}
	;

block_item
	: declaration  {
		check("declaraton from block_item");
		$$ = $1;
	
	}
	| statement   {
		check("statement form block_item");
		$$ =$1;

	}
	;

expression_statement
	: ';'
	| expression ';'  {
		check("expression");
		$$ = $1;

	}
	;

selection_statement
	: IF  '(' expression ')' statement{
		check("if then");
		$$ = build_stmt(IF_STMT, $3, $5, NULL);
	}
	| IF  '(' expression ')' statement ELSE statement{
		check("if then else");
		$$ = build_stmt(IF_STMT, $3, $5, $7);

	}
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
	| RETURN_ID ';'
	| RETURN_ID expression ';'{
			

	}
	;

translation_unit	/*  everything starts here  */
	: {check("empty source file");}
	| external_declaration {
		check("finish all");
		/* parse the ast now */
		for_each_decl(print_decl);
		anode t = lookup_name("foo");
		print_decl(t);

	}
	| translation_unit external_declaration {
		check("finish empty");
				/* parse the ast now */
		for_each_decl(print_decl);

	}
	;

external_declaration
	: function_definition
	| declaration
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement{
		check("function_define_legacy");



	}
	| declaration_specifiers declarator compound_statement{
		check("function_define");
		check("此时SymbolTable在proto(declrator)==$2里");


	}
	;

declaration_list
	: declaration
	| declaration_list declaration
	;


%%



extern "C"{

}
