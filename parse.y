

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
anode top_ast_node;
extern "C" void yyerror(char const *s)
{
	fflush(stdout);
	printf("%d,\n%*s\n%*s\n",column, column, "^", column, s);
}
extern "C" void check(const char *msg, ...){
	return;
	va_list vp;
	char *buf = NULL;
	va_start(vp, msg);

	vprintf(msg, vp);
	puts("");

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
%type <ast_node> 	external_declaration
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
%type <ast_node>    iteration_statement
%type <ast_node>    expression_statement
%type <ast_node>    block_item
%type <ast_expr_list>    block_item_list

%type <ast_declaration>    declaration
%type <ast_type>	declaration_specifiers
%type <ast_type>	type_specifier
%type <ast_qual>	type_qualifier
%type <ast_qual>	type_qualifier_list
%type <ast_node>	type_name
%expect 26
%{

/* something help parsing */
anode global_space;
anode current_declspaces;
anode declspace_stack;


typedef void (*ANODE_DECL_CALLBACK)(anode);

void for_each_decl(ANODE_DECL_CALLBACK f){
	anode a;
	for (a = current_declspaces; a; a = ANODE_CHAIN(a)){
		f(ANODE_VALUE(a));
	}
	
}
anode get_declspace_identifier(anode l){
	if(anode_code_class(anode_code(l)) == 'd')
		return decl_name(l);
	return ANODE_CHECK(l, IDENTIFIER_NODE);
}
int compare_name(anode node, const char *name){
	char *p = IDENTIFIER_POINTER(get_declspace_identifier(node));
	return strcmp(p, name);
}
anode lookup_name(const char *name){
	FOR_EACH_DECL(decl){
		if (ANODE_VALUE(decl) == NULL){
			printf("null skip\n");
			continue;
		}
		if(compare_name(ANODE_VALUE(decl), name) == 0)
			return ANODE_VALUE(decl);
	}
	return NULL;
}
anode lookup_name_current_bb(const char *name){
	FOR_EACH_DECL(decl){
		if (ANODE_VALUE(decl) == NULL)
			break;
		if (strcmp(IDENTIFIER_POINTER(get_declspace_identifier(ANODE_VALUE(decl))), name) == 0)
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

	}
	| CONSTANT_INT {
		check("CONSTANT_INT %d", $1);
		$$ = new anode_int_cst($1);

	}
	| STRING_LITERAL{
		check("literal");
		$$ = new anode_string_cst($1);
		free($1);
	}
	| '(' expression ')'{
		check("括号");
		$$ = $2;
	}
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
	| postfix_expression INC_OP{
		/* second operands should be the sizeof first operand */
		$$ = build_stmt(POSTINCREMENT_EXPR, $1, TYPE_SIZE(ANODE_TYPE($1)));
	}
	| postfix_expression DEC_OP{
		$$ = build_stmt(POSTDECREMENT_EXPR, $1, TYPE_SIZE(ANODE_TYPE($1)));
	}
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
	| INC_OP unary_expression{
		$$ = build_stmt(PREINCREMENT_EXPR, $2, TYPE_SIZE(ANODE_TYPE($2)));
	}
	| DEC_OP unary_expression{
		$$ = build_stmt(PREDECREMENT_EXPR, $2, TYPE_SIZE(ANODE_TYPE($2)));
	}
	| unary_operator cast_expression
	| SIZEOF unary_expression{
		$$ = TYPE_SIZE(ANODE_TYPE($2));
	}
	| SIZEOF '(' type_name ')'{
		$$ = TYPE_SIZE(ANODE_TYPE($3));
	}
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
	| '(' type_name ')' cast_expression{
		ANODE_TYPE($4) = $2;
		$$ = $4;
	}
	;

multiplicative_expression
	: cast_expression{
		$$ = $1;
	}
	| multiplicative_expression '*' cast_expression {
		//$$ = new AST_bin('*', $1, $3);
		$$ = build_stmt(MULT_EXPR, $1, $3);
	}
	| multiplicative_expression '/' cast_expression {
		//$$ = new AST_bin('/', $1, $3);
		$$ = build_stmt(TRUNC_DIV_EXPR, $1, $3);
	}
	| multiplicative_expression '%' cast_expression {
		//$$ = new AST_bin('%', $1, $3);
		$$ = build_stmt(TRUNC_MOD_EXPR, $1, $3);
	}
	;

additive_expression
	: multiplicative_expression{
		$$ = $1;
	}
	| additive_expression '+' multiplicative_expression {
		$$ = build_stmt(PLUS_EXPR, $1, $3);
	}
	| additive_expression '-' multiplicative_expression {
		$$ = build_stmt(MINUS_EXPR, $1, $3);
	}
	;

shift_expression
	: additive_expression{
		$$ = $1;
	}
	| shift_expression LEFT_OP additive_expression{
		$$ = build_stmt(LSHIFT_EXPR, $1, $3);
	}
	| shift_expression RIGHT_OP additive_expression{
		$$ = build_stmt(RSHIFT_EXPR, $1, $3);
	}
	;

relational_expression
	: shift_expression{
		$$ = $1;
	}
	| relational_expression '<' shift_expression{
		$$ = build_stmt(LT_EXPR, $1, $3);
	}
	| relational_expression '>' shift_expression{
		$$ = build_stmt(GT_EXPR, $1, $3);
	}
	| relational_expression LE_OP shift_expression{
		$$ = build_stmt(LE_EXPR, $1, $3);
	}
	| relational_expression GE_OP shift_expression{
		$$ = build_stmt(GE_EXPR, $1, $3);
	}
	;

equality_expression
	: relational_expression{
		$$ = $1;
	}
	| equality_expression EQ_OP relational_expression{
		$$ = build_stmt(EQ_EXPR, $1, $3);
	}
	| equality_expression NE_OP relational_expression{
		$$ = build_stmt(NE_EXPR, $1, $3);
	}
	;

and_expression
	: equality_expression{
		$$ = $1;
	}
	| and_expression '&' equality_expression{
		$$ = build_stmt(BIT_AND_EXPR, $1, $3);
	}
	;

exclusive_or_expression
	: and_expression{
		$$ = $1;
	}
	| exclusive_or_expression '^' and_expression{
		$$ = build_stmt(BIT_XOR_EXPR, $1, $3);
	}
	;

inclusive_or_expression
	: exclusive_or_expression{
		$$ = $1;
	}
	| inclusive_or_expression '|' exclusive_or_expression{
		$$ = build_stmt(BIT_IOR_EXPR, $1, $3);
	}
	;

logical_and_expression
	: inclusive_or_expression{
		$$ = $1;
	}
	| logical_and_expression AND_OP inclusive_or_expression{
		$$ = build_stmt(TRUTH_ANDIF_EXPR, $1, $3);
	}
	;

logical_or_expression
	: logical_and_expression{
		$$ = $1;
	}
	| logical_or_expression OR_OP logical_and_expression{
		$$ = build_stmt(TRUTH_ORIF_EXPR, $1, $3);
	}
	;

conditional_expression
	: logical_or_expression {
		$$ = $1;
	}
	| logical_or_expression '?' expression ':' conditional_expression{
		$$ = build_stmt(COND_EXPR, $1, $3, $5);
	}
	;

assignment_expression
	: conditional_expression {
		check("赋值expr");
		$$ = $1;
	}
	| unary_expression assignment_operator assignment_expression {
		check("赋值表达式");
		$$ = build_stmt(MODIFY_EXPR, $1, $3);
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
	| expression ',' assignment_expression{
		check("not support yet");
	}
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
		
		
		anode p = ANODE_VALUE($2);
		ANODE_TYPE(p) = $1;
		push_decl(p);

		for (t = ANODE_CHAIN($2); t; t = ANODE_CHAIN(t)){
			anode d = ANODE_VALUE(t);

			ANODE_TYPE(d) = $1;
			push_decl(d);
			ANODE_CHAIN(p) = d;
			p = d;
		
			check("lower Full 声明 (%s %s = c)\n",
				anode_code_name(anode_code(ANODE_TYPE(ANODE_VALUE(t)))),
				IDENTIFIER_POINTER(decl_name(ANODE_VALUE(t))),current_declspaces);

		}
		/* 从 decl_specifiers里提取type_specifier */
		/* DECL_STMT may contain only one or many VAR_DECLs */
		$$ = build_stmt(DECL_STMT, ANODE_VALUE($2));


	} //完整声明 包含变量名 或 初值 、类型
	;


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

	}
	| declarator ASSIGN initializer {
		check("one declarator+assign");
		$$ = $1;
		build_stmt(INIT_EXPR, $1, $3);
		/* we will expand the INIT_EXPR in lower-step */
		ANODE_($$, anode_decl)->initial = $3;

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
		anode_type *t = new anode_type(INTEGER_TYPE);
		TYPE_SIZE(t) = new anode_int_cst(sizeof(HOST_WIDTH_INT));
		$$ = t;

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
		//$$ = new anode_identifier($1);
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
		$$ = build_func_decl($1, $3);
	
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
		$$ = build_list(NULL, $1);
			
	}
	
	| parameter_list ',' parameter_declaration{
		$$ = chain_cat($1, $3);

	}
	;

parameter_declaration
	: declaration_specifiers declarator{
		build_parm_decl($1, $2);
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
		$$ = build_stmt(COMPOUND_STMT, NULL);
	}
	| LEFT_BRACE 
	{	
		push_namespace();

		printf("push_namespace\n");

	
	}
	block_item_list RIGHT_BRACE {
		anode_expr *t = (anode_expr*)build_stmt(COMPOUND_STMT, $3);
		printf("c_c %s\n", anode_code_name(anode_code(t)));
		COMPOUND_DS_INTER(t) = current_declspaces;
		/* calculate the outer decl_space that in real need */
		COMPOUND_DS_OUTER(t) = ANODE_VALUE(declspace_stack);
		$$ = t;
		pop_namespace();

	}
	;

block_item_list
	: block_item  { 

		$$ = $1;
	}
	| block_item_list block_item{
		$$ = chain_cat($1, $2);
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
	: ';' {$$ = NULL;}
	| expression ';'  {
		check("expression");
		$$ = $1;

	}
	;

selection_statement
	: IF  '(' expression ')' statement{
		check("if then");
		anode_expr *p = (anode_expr*)$3;
		COMPOUND_DS_OUTER(p) = current_declspaces;
		$$ = build_stmt(IF_STMT, $3, $5, NULL);
	}
	| IF  '(' expression ')' statement ELSE statement{
		check("if then else");
		anode_expr *p = (anode_expr*)$3;
		COMPOUND_DS_OUTER(p) = current_declspaces;

		$$ = build_stmt(IF_STMT, $3, $5, $7);

	}
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement{
		$$ = build_stmt(WHILE_STMT, $3, $5);
	}
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
		top_ast_node = $1;
		check("finish all");
		/* parse the ast now */
		for_each_decl(print_decl);
		anode t = lookup_name("foo");
		print_decl(t);

	}
	| translation_unit external_declaration {
		top_ast_node = chain_cat(top_ast_node, $2);
		check("finish empty");
				/* parse the ast now */

//		anode t = lookup_name_current_bb("hello");

//		anode id = get_declspace_identifier(t);
		

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
		$$ = $3;


	}
	;

declaration_list
	: declaration
	| declaration_list declaration
	;


%%



extern "C"{

}
