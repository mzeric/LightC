#include "anode.h"

anode current_function_decl;
static int next_decl_uid;
static int next_type_uid = 1;
//location_t input_location;

anode anode_null = NULL;

int in_system_header = 0;
#define DEFTREECODE(sym, name, type, length) name,
const char* _anode_code_name[] = {
        #include "c-tree.def"
};
#undef DEFTREECODE
/*
   Indexed by enum tree_code, contains a character which is
   `<' for a comparison expression, `1', for a unary arithmetic
   expression, `2' for a binary arithmetic expression, `e' for
   other types of expressions, `r' for a reference, `c' for a
   constant, `d' for a decl, `t' for a type, `s' for a statement,
   and `x' for anything else (TREE_LIST, IDENTIFIER, etc).  
*/

#define DEFTREECODE(sym, name, type, length) type,
const char anode_code_type[] = {
        #include "c-tree.def"
};
#undef DEFTREECODE


#define DEFTREECODE(sym, name, type, length) length,
const unsigned char _anode_code_length[] = {
        #include "c-tree.def"
};
#undef DEFTREECODE
//int anode_type(anode node){ return node->type; }
int anode_code(anode node){ return node->code; }

int anode_code_length(int code) { return _anode_code_length[(int)code]; }
void anode_set_code(anode node, int value) { node->code = value; }
int anode_code_class(int code) { return anode_code_type[(int)code]; }
const char *anode_code_name(int code) { return _anode_code_name[(int)code]; }


//anode global_trees[TI_MAX];
//anode integer_types[itk_none];
void anode_check_failed(const anode node, int code, const char*file,
		int line, const char* function){
	      printf
        ("tree check: expected %s, have %s in %s:%d\n",
                anode_code_name(code), anode_code_name(anode_code(node)),
                function, line);
}
void anode_class_check_failed(const anode node, int cl, const char *file,
                int line, const char* function){
        printf
        ("tree check: expected class '%c', have '%c' (%s) in %s, at %s:%d\n",
                cl, anode_code_class(anode_code(node)),
                anode_code_name(node->code), function, file, line);
}
void anode_operand_check_failed (int idx, int code, const char *file,
                                int line, const char *function){
        printf
        ("tree check: accessed operand %d of %s with %d operands in %s, at %s:%d",
                idx + 1, anode_code_name(code), anode_code_length (code), function,
                file, line);
}
const char *progname = "llc";
int have_error = 0;
void warning (const char *format, ...)
{
        va_list ap; 

        va_start (ap, format);
        fprintf (stderr, "%s: warning: ", progname);
        vfprintf (stderr, format, ap);
        va_end (ap);
        fputc('\n', stderr);
}
void error (const char *format, ...)
{
        va_list ap; 

        va_start (ap, format);
        fprintf (stderr, "%s: ", progname);
        vfprintf (stderr, format, ap);
        va_end (ap);
        fputc('\n', stderr);

        have_error = 1;
}
void fatal (const char *format, ...)
{
        va_list ap;

        va_start (ap, format);
        fprintf (stderr, "%s: ", progname);
        vfprintf (stderr, format, ap);
        va_end (ap);
        fputc('\n', stderr);
        exit (EXIT_FAILURE);
}
void internal_error (const char *format, ...)
{
        va_list ap;

        va_start (ap, format);
        fprintf (stderr, "%s: Internal error: ", progname);
        vfprintf (stderr, format, ap);
        va_end (ap);
        fputc ('\n', stderr);
        exit (EXIT_FAILURE);
}

/**

*/
anode alloc_anode(size_t length){
	      return (anode)malloc(length);
}
size_t anode_size(anode node){
	      return anode_code_length(node->code);
}
anode build_list(anode p, anode v){
        anode_list * l = new anode_list();
        ANODE_VALUE(l) = v;
        l->purpose = p;
        return l;
}
anode build_decl(enum anode_code code, anode name, anode type){
        anode_decl *t = new anode_decl();
        ANODE_CODE(t) = code;
        ANODE_DECL_NAME(t) = name;
        ANODE_TYPE(t) = type;

        return t;
}
anode build_pointer_type(anode type)
{
	anode_type *t = new anode_type(POINTER_TYPE);
	ANODE_TYPE(t) = type;
	return t;
}
anode build_addr_expr(anode expr)
{
	if (ANODE_CODE(expr) == INDIRECT_REF){
		anode ti = ANODE_OPERAND(expr, 0);
	}else if (ANODE_CODE(expr) == MEM_REF) {
		anode t = ANODE_OPERAND(expr, 1);
		if (ANODE_CODE(t) == INTEGER_CST && ((anode_int_cst*)t)->cst() == 0)
			return ANODE_OPERAND(expr, 0);

	}
	anode t = build_stmt(ADDR_EXPR, expr);
	ANODE_TYPE(t) = build_pointer_type(ANODE_TYPE(expr));
	return t;
}
anode build1(int code, anode type, anode expr)
{
	anode t = build_stmt(code, expr);
	ANODE_TYPE(t) = type;
	return t;
}
anode chain_cat(anode a, anode b){
        anode t;
        if(!a)
            return b;
        if(!b)
            return a;
        for (t = a; ANODE_CHAIN(t); t = ANODE_CHAIN(t))
            continue;
        ANODE_CHAIN(t) = b;
        return a;
}
anode chain_append_value(anode chain, anode value){
	
	anode list = build_list(NULL, value);
	return chain_cat(chain, list);

}

void chain_insert_after(anode chain, anode value){
	if (chain == NULL || value == NULL)
		return;
	anode list = build_list(NULL, value);
	anode next_p = ANODE_CHAIN(chain);
	ANODE_CHAIN(chain) = list;
	ANODE_CHAIN(list) = next_p;

}

anode chain_cons(anode purpose, anode value, anode chain){
        anode_list *l = new anode_list();
        l->purpose = purpose;
        ANODE_VALUE(l) = value;
        ANODE_CHAIN(l) = chain;
        return l;
}
anode anode_cons(anode purpose, anode value, anode chain){
        anode_list *l = new anode_list();
        l->purpose = purpose;
        ANODE_VALUE(l) = value;
        ANODE_CHAIN(l) = chain;
        return l;
}


/* the redefine value stored in anode_list->purpose */



anode chain_last(anode chain){
        anode t = chain;
        while(ANODE_CHAIN(t))
            t = ANODE_CHAIN(t);
        return t;
}

