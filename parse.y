%{

#include <stdio.h>
#include "common.h"
#include "ast.h"

//#define INT2VALUE(x) ConstantInt::get(getGlobalContext(),APInt(32,(x)))
extern "C" void A_EXP_(int,int,char*);
extern "C" int yyparse(void);
	


extern "C" int yylex(void);
extern char yytext[];
extern int column;
extern int lvc_yyleng;

static int parameter_list_num = 0;

extern "C" void yyerror(char const *s)
{
	fflush(stdout);
	printf("%d,\n%*s\n%*s\n",column, column, "^", column, s);
}

//extern "C" Fvalue* aaa(Fvalue*a,Fvalue*b,char*str);
%}


%union {
    int num;
    char* id;
    Fvalue* value;
    std::string *string;
    std::vector<Fstring> *parameter;
    ASTContext  *context;
    Node		*ast_node;
    ASTType		*ast_type;
    QualType	*ast_qual;
    StorType	*ast_stor;
    AST_expr_list    *ast_expr_list;
    AST_decl 	*ast_decl;
    AST_declarator *ast_declarator;
    Declaration *ast_declaration;
    AST_args    *ast_args;
    AST_func	*ast_func;  //func definition
    
    
}
%token CONSTANT STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token LEFT_BRACE RIGHT_BRACE

%token TYPEDEF EXTERN STATIC AUTO REGISTER INLINE RESTRICT
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token BOOL COMPLEX IMAGINARY
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN
%token ASSIGN

%start translation_unit

%token <num> CONSTANT_INT
%token <id> IDENTIFIER
%token <string> STRING_LITERAL
%token <context> LEFT_BRACE


%type <num> assignment_operator 
%type <ast_node> 	function_definition
%type <ast_declarator> 	declarator
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
%type <ast_declarator> init_declarator_list

%type <ast_node>    initializer
%type <ast_node>    statement
%type <ast_node>    expression_statement
%type <ast_node>    block_item
%type <ast_expr_list>    block_item_list

%type <ast_declaration>    declaration
%type <ast_type>	declaration_specifiers
%type <ast_type>	type_specifier
%type <ast_qual>	type_qualifier
%type <ast_qual>	type_qualifier_list


%%

primary_expression
	: IDENTIFIER {
		check("identifier");
		if(lvc_yyleng)
		$1[lvc_yyleng]='\0';
		$$ = new AST_var($1);
		//引用了一个变量(局部),note: AST_xx只是创建AST树,只是一个标记,实际的语义在visit()里
		//此处是往AST里加入一个变量引用节点
		lvc_yyleng = 0;
	}
	| CONSTANT_INT {
		check("CONSTANT_INT");
			
		$$ = new AST_integer($1);
		printf("%X ",$$);
	}
	| STRING_LITERAL{
		check("literal");

		$$ = new AST_literal($1);
		delete $1; /* free the lexer's std::string* */
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
		$$ = new AST_bin('*', $1, $3);
	}
	| multiplicative_expression '/' cast_expression {
		$$ = new AST_bin('/', $1, $3);
	}
	| multiplicative_expression '%' cast_expression {
		$$ = new AST_bin('%', $1, $3);
	}
	;

additive_expression
	: multiplicative_expression{
		$$ = $1;
	}
	| additive_expression '+' multiplicative_expression {
		$$ = new AST_bin('+', $1, $3);
	}
	| additive_expression '-' multiplicative_expression {
		$$ = new AST_bin('-', $1, $3);
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
	
		$$ = new AST_assignment($1,$3);
	
		
		printf("%X ",$$);
	}
	;

assignment_operator
	: ASSIGN { $$ = '=';
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
		printf("%X ",$1);
	}
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression
	;

declaration
	: declaration_specifiers ';' {
		$$ = new Declaration($1, NULL);
	}//声明前缀 类型 int 等
	| declaration_specifiers init_declarator_list ';' {
		/* 从 decl_specifiers里提取type_specifier */
		
		$$ = new Declaration($1, $2);
		std::cout << "Create Declaration " << $2->decl_id << std::endl;
	//	$$->type= $1->ast_type;
	
	;} //完整声明 包含变量名 或 初值
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
		std::cout << "isConst: " << $1->hasConst() << std::endl;
	}
	| type_qualifier declaration_specifiers{

		$2->addQualifier(*$1);
		$$ = $2;

		delete $1;
	}
	| function_specifier
	| function_specifier declaration_specifiers
	;

init_declarator_list	
	: init_declarator {
		$$ = $1;  //ast_declare
	}
	| init_declarator_list ',' init_declarator{
		check("不支持的语法");

	}
	;

init_declarator
	: declarator  {
		check("声明");
		//$$ = $1; //ast_declare	
		$$ = new AST_decl_var($1, NULL);
	}
	| declarator ASSIGN initializer {
		check("声明_assign");
		// if initializer is constant, just store within the Fvalue $->ir;
		$$ = new AST_decl_var($1, $3);
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
		$$ = new BuiltinType(BuiltinType::Char);
	}
	| SHORT
	| INT {
		$$ = new BuiltinType(BuiltinType::Integer);
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
		$$ = new QualType(QualType::Qualifier_Const);
	}
	| RESTRICT{
		$$ = new QualType(QualType::Qualifier_Restrict);
	}
	| VOLATILE{
		$$ = new QualType(QualType::Qualifier_Volatile);
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
		check("identifier_declartor");
		$$ = new AST_var($1);
		
	}
	| '(' declarator ')' {
		check("函数指针_声明");
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
		check("函数_声明");
	
		// AST_proto 复杂添加符号表
		$$ = new AST_proto($1->get_name(),$3->get_args());
		$$->context = $3->context;

	
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
		parameter_list_num = 0;
	}
	| parameter_list ',' ELLIPSIS {$$ = $1;}
	;

parameter_list
	: parameter_declaration {
	check("Push SymbolTable 函数参数>>>");
			
			$$ = new AST_args;
			$$->add_args($1->declarator->get_name());
			parameter_list_num++;
	}
	
	| parameter_list ',' parameter_declaration{
			$$->add_args($3->declarator->get_name());
			parameter_list_num++;
	}
	;

parameter_declaration
	: declaration_specifiers declarator{
			check("参数");// 类型 + 变量
			if(parameter_list_num == 0){
				ASTContext *c = new ASTContext();
				c->Push();
			}
			$$ =  new Declaration($1, $2);
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
	| compound_statement {check("块_{}"); $$ = $1;}
	| expression_statement {check("块_表达式");
		$$ = $1;
	}
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
	: LEFT_BRACE RIGHT_BRACE
	| LEFT_BRACE block_item_list RIGHT_BRACE {

		$$ = $2;
		$$->context = $1;

	}
	;

block_item_list
	: block_item  { 
		$$ = new AST_expr_list($1);

		printf("%X",$1);
	}
	| block_item_list block_item{check("block_item append");
		AST_expr_list *t = new AST_expr_list($2);
		$1->append_expr(t);	
		std::cout << "append " << $2 << " " << $1->expr << std::endl;;
		//$$ = 
	}
	;

block_item
	: declaration  {
		check("declaraton from block_item");
		$$ = $1;
	
	}
	| statement   {check("statement form block_item");
		$$ =$1;
		printf("%X ",$1);
	}
	;

expression_statement
	: ';'
	| expression ';'  {check("expression");
		$$ = $1;
		printf("%X",$1);
	}
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
	| RETURN expression ';'{
			

	}
	;

translation_unit	/*  everything starts here  */
	: external_declaration
	| translation_unit external_declaration
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

//	$2->code();

/*		std::vector<Fstring> my_args;
		my_args.push_back("first");
		my_args.push_back("second");
		AST_integer *vint = new AST_integer(456);
		AST_var *my_var   = new AST_var("first");
		AST_proto* my_proto = new AST_proto("my_func",my_args);

		std::vector<AST_expr*> my_call_args;
		my_call_args.push_back(new AST_integer(456));
		my_call_args.push_back(new AST_integer(789));
		AST_call * my_call = new AST_call("test_func",my_call_args);
*/


        AST_func* p = new AST_func ($2,$3);
        CodegenVisitor v;
        //p->accept(&v);
        $$ = p;
        //$$->context = $2->context;
//		((AST_func*)$$)->code();
        $2->context->Pop();
        check("Pop SymbolTable 函数参数<<<");
        dumpAllContext();
       // $$ = my_func;
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
