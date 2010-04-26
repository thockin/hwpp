/* This file is written assuming that it will be compiled as C++ code */
%{

//
// This block goes into the yacc output .h file.
//

#include <string>
#include "pp/pp.h"
#include "pp/language/language.h"
#include "pp/language/syntax_tree.h"

%}

/* Define a prefix that should make this code hard to collide with. */
%name-prefix="pp__language__internal__"
/* Generate a pure parser */
%pure-parser
/* The lexer context gets passed in to yyparse() and through to yylex(). */
%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner}
/* The Parser object gets passed into yyparse and can be user therein. */
%parse-param {pp::language::Parser *parser}
%parse-param {pp::language::syntax::StatementList *parsed_file}

/* We use GLR parsing because we need more look-ahead to resolve some rules. */
%glr-parser
%expect 1

// We have to use full namespace qualification for types here, since this gets
// put into the yacc-generated header file, without the required includes or
// namespacing.  This also means that anyone who includes the yacc-generated
// header needs to first include the headers to satisfy this struct.
%union {
	bool bool_val;
	pp::Value *int_val;
	string *string_val;
	pp::language::syntax::Statement *stmt;
	pp::language::syntax::StatementList *stmt_list;
	pp::language::syntax::Identifier *ident;
	pp::language::syntax::InitializedIdentifier *init_ident;
	pp::language::syntax::InitializedIdentifierList *init_ident_list;
	pp::language::syntax::Expression *expr;
	pp::language::syntax::ExpressionList *expr_list;
	pp::language::syntax::UnaryExpression::Operator unary_op;
	pp::language::syntax::BinaryExpression::Operator binary_op;
	pp::language::syntax::Argument *arg;
	pp::language::syntax::ArgumentList *arg_list;
	pp::language::syntax::ExpressionStatement *expr_stmt;
	pp::language::syntax::DefinitionStatement *defn_stmt;
	pp::language::syntax::ParameterDeclaration *param_decl;
	pp::language::syntax::ParameterDeclarationList *param_decl_list;
	pp::language::Type *type;
	pp::language::syntax::TypeList *type_list;
}

%{

//
// This block goes into the yacc output .c file
//

#include <stdio.h>
#define YY_HEADER_EXPORT_START_CONDITIONS 1
#include "auto.lex.h"

// Act as if this code were in the pp::language::syntax namespace.
using namespace pp;
using namespace pp::language;
using namespace pp::language::syntax;

// This is called on a parse error.
static void
pp__language__internal__error(yyscan_t scanner, Parser *parser,
                              StatementList *parsed_file, const char *str);

// These are exposed from the lexer to the parser.
extern void
pp__language__internal__push_lexer_state(int new_state, yyscan_t scanner);
extern void
pp__language__internal__pop_lexer_state(yyscan_t scanner);

// Utility defines make code easier to read.
#define lex_lineno()      pp__language__internal__get_lineno(scanner)
#define lex_text()        pp__language__internal__get_text(scanner)
#define lex_push_state(x) pp__language__internal__push_lexer_state(x, scanner)
#define lex_pop_state()   pp__language__internal__pop_lexer_state(scanner)

// To make tracing the parser easier.
#define SYNTRACE(left, right) do { \
	fprintf(stderr, "%d: %s <- %s\n", lex_lineno(), left, right); \
} while (0)

%}

%token <string_val>      TOK_IDENTIFIER
%type  <ident>           identifier simple_identifier absolute_identifier
%type  <init_ident_list> initialized_identifier_list

%type  <init_ident>      initialized_identifier
%type  <expr>            expression primary_expression constant_expression
                         conditional_expression assignment_expression
                         or_or_expression and_and_expression shift_expression
                         and_expression or_expression xor_expression
                         relational_expression equality_expression
                         multiplicative_expression additive_expression
                         unary_expression postfix_expression
                         subscript_expression function_call_expression
                         function_literal list_literal
%type  <unary_op>        unary_operator
%type  <binary_op>       assignment_operator
%type  <arg>             argument
%type  <arg_list>        argument_list

%token <int_val>         TOK_INT_LITERAL
%token <bool_val>        TOK_BOOL_LITERAL
%token <string_val>      TOK_STRING_LITERAL
%type  <string_val>      string_literal

%type  <stmt>            file_scope_item
                         statement labeled_statement
                         discover_statement import_statement module_statement
                         compound_statement
                         branch_statement loop_statement jump_statement
%type  <defn_stmt>       definition_statement
                         variable_definition_statement
                         function_definition_statement
%type  <stmt_list>       non_empty_statement_list statement_list
%type  <expr_stmt>       expression_statement
%type  <type>            type_primitive type_specifier qualified_type_specifier
%type  <type_list>       qualified_type_specifier_list
%type  <param_decl>      parameter_declaration
%type  <param_decl_list> parameter_declaration_list

%token TOK_FUNC_LITERAL
%token TOK_INC TOK_DEC TOK_SHL TOK_SHR TOK_LE TOK_GE TOK_EQ TOK_NE
%token TOK_AND_AND TOK_OR_OR
%token TOK_MUL_ASSIGN TOK_DIV_ASSIGN TOK_MOD_ASSIGN
%token TOK_ADD_ASSIGN TOK_SUB_ASSIGN TOK_SHL_ASSIGN TOK_SHR_ASSIGN
%token TOK_AND_ASSIGN TOK_XOR_ASSIGN TOK_OR_ASSIGN

%token TOK_DISCOVER TOK_IMPORT TOK_MODULE TOK_PUBLIC
%token TOK_CONST
%token TOK_BOOL TOK_FUNC TOK_INT TOK_LIST TOK_STRING TOK_TUPLE
       TOK_FLDFMT TOK_VAR

%token TOK_SWITCH TOK_CASE TOK_DEFAULT
%token TOK_IF TOK_ELSE
%token TOK_WHILE TOK_DO TOK_FOR TOK_CONTINUE TOK_BREAK
%token TOK_GOTO TOK_RETURN

%token TOK_UNKNOWN

// Start parsing at the 'file_scope' rule.
%start file_scope

%%

// Allow relative or module-qualified identifiers.
identifier
	: simple_identifier {
		SYNTRACE("identifier", "simple_identifier");
		$$ = $1;
	}
	| absolute_identifier {
		SYNTRACE("identifier", "absolute_identifier");
		$$ = $1;
	}
	;

simple_identifier
	: TOK_IDENTIFIER {
		SYNTRACE("simple_identifier", "IDENTIFIER");
		$$ = new Identifier(*$1);
		delete $1;
	}
	;

absolute_identifier
	: TOK_IDENTIFIER '.' TOK_IDENTIFIER {
		SYNTRACE("absolute_identifier", "IDENTIFIER '.' IDENTIFIER");
		$$ = new Identifier(*$1, *$3);
		delete $1;
		delete $3;
	}
	;

primary_expression
	: identifier {
		SYNTRACE("primary_expression", "identifier");
		$$ = new IdentifierExpression($1);
	}
	| TOK_BOOL_LITERAL {
		SYNTRACE("primary_expression", "BOOL_LITERAL");
		$$ = new ValueExpression(
		    new Variable::Datum(new Type(Type::BOOL), $1));
	}
	| TOK_INT_LITERAL {
		SYNTRACE("primary_expression", "INT_LITERAL");
		$$ = new ValueExpression(
		    new Variable::Datum(new Type(Type::INT), *$1));
		delete $1;
	}
	| string_literal {
		SYNTRACE("primary_expression", "string_literal");
		$$ = new ValueExpression(
		    new Variable::Datum(new Type(Type::STRING), *$1));
		delete $1;
	}
	| list_literal {
		SYNTRACE("primary_expression", "list_literal");
		$$ = $1;
	}
	| function_literal {
		SYNTRACE("primary_expression", "function_literal");
		$$ = $1;
	}
	| '(' expression ')' {
		SYNTRACE("primary_expression", "'(' expression ')'");
		$$ = $2;
	}
	;

// Allow string literal joining.
string_literal
	: TOK_STRING_LITERAL {
		$$ = $1;
	}
	| string_literal TOK_STRING_LITERAL {
		*$1 += *$2;
		$$ = $1;
		delete $2;
	}
	;

list_literal
	: '[' ']' {
		SYNTRACE("list_literal", "'[' ']'");
		$$ = new ListLiteralExpression();
	}
	| '[' argument_list ']' {
		SYNTRACE("list_literal", "'[' argument_list ']'");
		$$ = new ListLiteralExpression($2);
	}
	;

function_literal
	: TOK_FUNC_LITERAL '{' statement_list '}' {
		SYNTRACE("function_literal",
		         "FUNC_LITERAL '{' statement_list '}'");
		$$ = new FunctionLiteralExpression($3);
	}
	| TOK_FUNC_LITERAL '(' ')' '{' statement_list '}' {
		SYNTRACE("function_literal",
		         "FUNC_LITERAL '(' ')' '{' statement_list '}'");
		$$ = new FunctionLiteralExpression($5);
	}
	| TOK_FUNC_LITERAL '(' parameter_declaration_list ')'
	  '{' statement_list '}' {
		SYNTRACE("function_literal",
		         "FUNC_LITERAL '(' " "parameter_declaration_list ')' "
		         "'{' statement_list '}'");
		$$ = new FunctionLiteralExpression($3, $6);
	}
	;

// TODO: '123()' parses to a function_call_expression.  Make sure to validate
//       that the called expression is actually callable.
function_call_expression
	: primary_expression {
		SYNTRACE("function_call_expression", "primary_expression");
		$$ = $1;
	}
	| subscript_expression '(' ')' {
		SYNTRACE("function_call_expression",
		         "function_call_expression '(' ')'");
		$$ = new FunctionCallExpression($1);
	}
	| subscript_expression '(' argument_list ')' {
		SYNTRACE("function_call_expression",
		         "function_call_expression '(' argument_list ')'");
		$$ = new FunctionCallExpression($1, $3);
	}
	;

argument_list
	: argument {
		SYNTRACE("argument_list", "argument");
		$$ = new ArgumentList();
		$$->push_back($1);
	}
	| argument_list ',' { // Allow dangling commas for convenience.
		SYNTRACE("argument_list", "argument_list ','");
		$$ = $1;
	}
	| argument_list ',' argument {
		SYNTRACE("argument_list", "argument_list ',' argument");
		$1->push_back($3);
		$$ = $1;
	}
	;

argument
	: assignment_expression {
		SYNTRACE("argument", "assignment_expression");
		$$ = new Argument($1);
	}
	| simple_identifier ':' assignment_expression {
		SYNTRACE("argument",
		         "simple_identifier ':' assignment_expression");
		$$ = new Argument($1, $3);
	}
	;

// TODO: '123[0]' parses to a subscript_expression.  Make sure to validate
//       that the indexed expression is actually indexable.
subscript_expression
	: function_call_expression {
		SYNTRACE("subscript_expression", "function_call_expression");
		$$ = $1;
	}
	| subscript_expression '[' expression ']' {
		SYNTRACE("subscript_expression",
		         "subscript_expression '[' expression ']'");
		$$ = new SubscriptExpression($1, $3);
	}
	;

postfix_expression
	: subscript_expression {
		SYNTRACE("postfix_expression", "subscript_expression");
		$$ = $1;
	}
	| postfix_expression TOK_INC {
		SYNTRACE("postfix_expression", "postfix_expression INC");
		$$ = new UnaryExpression(UnaryExpression::OP_POSTINC,
		                             $1);
	}
	| postfix_expression TOK_DEC {
		SYNTRACE("postfix_expression", "postfix_expression DEC");
		$$ = new UnaryExpression(UnaryExpression::OP_POSTDEC,
		                             $1);
	}
	;

unary_expression
	: postfix_expression {
		SYNTRACE("unary_expression", "postfix_expression");
		$$ = $1;
	}
	| unary_operator unary_expression {
		SYNTRACE("unary_expression", "unary_operator cast_expression");
		$$ = new UnaryExpression($1, $2);
	}
	;

unary_operator
	: '+' {
		SYNTRACE("unary_operator", "'+'");
		$$ = UnaryExpression::OP_POS;
	}
	| '-' {
		SYNTRACE("unary_operator", "'-'");
		$$ = UnaryExpression::OP_NEG;
	}
	| '!' {
		SYNTRACE("unary_operator", "'!'");
		$$ = UnaryExpression::OP_NOT;
	}
	| '~' {
		SYNTRACE("unary_operator", "'~'");
		$$ = UnaryExpression::OP_BITNOT;
	}
	| TOK_INC {
		SYNTRACE("unary_operator", "INC");
		$$ = UnaryExpression::OP_PREINC;
	}
	| TOK_DEC {
		SYNTRACE("unary_operator", "DEC");
		$$ = UnaryExpression::OP_PREDEC;
	}
	;

multiplicative_expression
	: unary_expression {
		SYNTRACE("multiplicative_expression", "unary_expression");
		$$ = $1;
	}
	| multiplicative_expression '*' unary_expression {
		SYNTRACE("multiplicative_expression",
		         "multiplicative_expression '*' unary_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_MUL,
		                              $1, $3);
	}
	| multiplicative_expression '/' unary_expression {
		SYNTRACE("multiplicative_expression",
		         "multiplicative_expression '/' unary_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_DIV,
		                              $1, $3);
	}
	| multiplicative_expression '%' unary_expression {
		SYNTRACE("multiplicative_expression",
		         "multiplicative_expression '%' unary_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_MOD,
		                              $1, $3);
	}
	;

additive_expression
	: multiplicative_expression {
		SYNTRACE("additive_expression", "multiplicative_expression");
		$$ = $1;
	}
	| additive_expression '+' multiplicative_expression {
		SYNTRACE("additive_expression",
		         "additive_expression '+' multiplicative_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_ADD,
		                              $1, $3);
	}
	| additive_expression '-' multiplicative_expression {
		SYNTRACE("additive_expression",
		         "additive_expression '-' multiplicative_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_SUB,
		                              $1, $3);
	}
	;

shift_expression
	: additive_expression {
		SYNTRACE("shift_expression", "additive_expression");
		$$ = $1;
	}
	| shift_expression TOK_SHL additive_expression {
		SYNTRACE("shift_expression",
		         "shift_expression SHL additive_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_SHL,
		                              $1, $3);
	}
	| shift_expression TOK_SHR additive_expression {
		SYNTRACE("shift_expression",
		         "shift_expression SHR additive_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_SHR,
		                              $1, $3);
	}
	;

relational_expression
	: shift_expression {
		SYNTRACE("relational_expression", "shift_expression");
		$$ = $1;
	}
	| relational_expression '<' shift_expression {
		SYNTRACE("relational_expression",
		         "relational_expression '<' shift_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_LT,
		                              $1, $3);
	}
	| relational_expression '>' shift_expression {
		SYNTRACE("relational_expression",
		         "relational_expression '>' shift_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_GT,
		                              $1, $3);
	}
	| relational_expression TOK_LE shift_expression {
		SYNTRACE("relational_expression",
		         "relational_expression LE shift_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_LE,
		                              $1, $3);
	}
	| relational_expression TOK_GE shift_expression {
		SYNTRACE("relational_expression",
		         "relational_expression GE shift_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_GE,
		                              $1, $3);
	}
	;

equality_expression
	: relational_expression {
		SYNTRACE("equality_expression", "relational_expression");
		$$ = $1;
	}
	| equality_expression TOK_EQ relational_expression
	{
		SYNTRACE("equality_expression",
		         "equality_expression EQ relational_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_EQ,
		                              $1, $3);
	}
	| equality_expression TOK_NE relational_expression
	{
		SYNTRACE("equality_expression",
		         "equality_expression NE relational_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_NEQ,
		                              $1, $3);
	}
	;

and_expression
	: equality_expression {
		SYNTRACE("and_expression", "equality_expression");
		$$ = $1;
	}
	| and_expression '&' equality_expression {
		SYNTRACE("and_expression",
		         "and_expression '&' equality_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_AND,
		                              $1, $3);
	}
	;

xor_expression
	: and_expression {
		SYNTRACE("xor_expression", "and_expression");
		$$ = $1;
	}
	| xor_expression '^' and_expression {
		SYNTRACE("xor_expression",
		         "xor_expression '^' and_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_XOR,
		                              $1, $3);
	}
	;

or_expression
	: xor_expression {
		SYNTRACE("or_expression", "xor_expression");
		$$ = $1;
	}
	| or_expression '|' xor_expression {
		SYNTRACE("or_expression", "or_expression '|' xor_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_OR,
		                              $1, $3);
	}
	;

and_and_expression
	: or_expression {
		SYNTRACE("and_and_expression", "or_expression");
		$$ = $1;
	}
	| and_and_expression TOK_AND_AND or_expression {
		SYNTRACE("and_and_expression",
		         "and_and_expression AND_AND or_expression");
		// (A && B)  =>  (A == true) ? B : false
		Expression *cond = new BinaryExpression(
		    BinaryExpression::OP_EQ, $1,
		    new ValueExpression(
		        new Variable::Datum(new Type(Type::BOOL), true)));
		$$ = new ConditionalExpression(cond, $3,
		    new ValueExpression(
		        new Variable::Datum(new Type(Type::BOOL), false)));
	}
	;

or_or_expression
	: and_and_expression {
		SYNTRACE("or_or_expression", "and_and_expression");
		$$ = $1;
	}
	| or_or_expression TOK_OR_OR and_and_expression {
		SYNTRACE("or_or_expression",
		         "or_or_expression OR_OR and_and_expression");
		// (A || B)  =>  (A == true) ? true : B
		Expression *cond = new BinaryExpression(
		    BinaryExpression::OP_EQ, $1,
		    new ValueExpression(
		        new Variable::Datum(new Type(Type::BOOL), true)));
		$$ = new ConditionalExpression(cond,
		    new ValueExpression(
		        new Variable::Datum(new Type(Type::BOOL), true)), $3);
	}
	;

conditional_expression
	: or_or_expression {
		SYNTRACE("conditional_expression", "or_or_expression");
		$$ = $1;
	}
	| or_or_expression '?' conditional_expression
	                   ':' conditional_expression {
		SYNTRACE("conditional_expression",
		         "or_or_expression '?' expression"
		         " ':' conditional_expression");
		$$ = new ConditionalExpression($1, $3, $5);
	}
	;

assignment_expression
	: conditional_expression {
		SYNTRACE("assignment_expression", "conditional_expression");
		$$ = $1;
	}
	| unary_expression assignment_operator assignment_expression {
		SYNTRACE("assignment_expression",
		"unary_expression assignment_operator assignment_expression");
		$$ = new BinaryExpression($2, $1, $3);
	}
	;

assignment_operator
	: '=' {
		SYNTRACE("assignment_operator", "'='");
		$$ = BinaryExpression::OP_ASSIGN;
	}
	| TOK_MUL_ASSIGN {
		SYNTRACE("assignment_operator", "MUL_ASSIGN");
		$$ = BinaryExpression::OP_MUL_ASSIGN;
	}
	| TOK_DIV_ASSIGN {
		SYNTRACE("assignment_operator", "DIV_ASSIGN");
		$$ = BinaryExpression::OP_DIV_ASSIGN;
	}
	| TOK_MOD_ASSIGN {
		SYNTRACE("assignment_operator", "MOD_ASSIGN");
		$$ = BinaryExpression::OP_MOD_ASSIGN;
	}
	| TOK_ADD_ASSIGN {
		SYNTRACE("assignment_operator", "ADD_ASSIGN");
		$$ = BinaryExpression::OP_ADD_ASSIGN;
	}
	| TOK_SUB_ASSIGN {
		SYNTRACE("assignment_operator", "SUB_ASSIGN");
		$$ = BinaryExpression::OP_SUB_ASSIGN;
	}
	| TOK_SHL_ASSIGN {
		SYNTRACE("assignment_operator", "SHL_ASSIGN");
		$$ = BinaryExpression::OP_SHL_ASSIGN;
	}
	| TOK_SHR_ASSIGN {
		SYNTRACE("assignment_operator", "SHR_ASSIGN");
		$$ = BinaryExpression::OP_SHR_ASSIGN;
	}
	| TOK_AND_ASSIGN {
		SYNTRACE("assignment_operator", "AND_ASSIGN");
		$$ = BinaryExpression::OP_AND_ASSIGN;
	}
	| TOK_XOR_ASSIGN {
		SYNTRACE("assignment_operator", "XOR_ASSIGN");
		$$ = BinaryExpression::OP_XOR_ASSIGN;
	}
	| TOK_OR_ASSIGN {
		SYNTRACE("assignment_operator", "OR_ASSIGN");
		$$ = BinaryExpression::OP_OR_ASSIGN;
	}
	;

expression
	: assignment_expression {
		SYNTRACE("expression", "assignment_expression");
		$$ = $1;
	}
	| expression ',' assignment_expression {
		SYNTRACE("expression", "expression ',' assignment_expression");
		$$ = new BinaryExpression(BinaryExpression::OP_COMMA,
		                              $1, $3);
	}
	;

constant_expression
	: conditional_expression {
		SYNTRACE("constant_expression", "conditional_expression");
		$$ = $1;
	}
	;

qualified_type_specifier
	: type_specifier {
		SYNTRACE("qualified_type_specifier", "type_specifier");
		$$ = $1;
	}
	| TOK_CONST type_specifier {
		SYNTRACE("qualified_type_specifier",
		         "type_qualifier_list type_specifier");
		$2->set_const();
		$$ = $2;
	}
	;

qualified_type_specifier_list
	: qualified_type_specifier {
		SYNTRACE("qualified_type_specifier_list",
		         "qualified_type_specifier");
		$$ = new TypeList();
		$$->push_back($1);
	}
	| qualified_type_specifier_list ',' qualified_type_specifier {
		SYNTRACE("qualified_type_specifier_list",
		         "qualified_type_specifier_list ',' "
		         "qualified_type_specifier");
		$1->push_back($3);
		$$ = $1;
	}
	;

type_primitive
	: TOK_BOOL {
		SYNTRACE("type_primitive", "BOOL");
		$$ = new Type(Type::BOOL);
	}
	| TOK_FUNC {
		SYNTRACE("type_primitive", "FUNC");
		$$ = new Type(Type::FUNC);
	}
	| TOK_INT {
		SYNTRACE("type_primitive", "INT");
		$$ = new Type(Type::INT);
	}
	| TOK_LIST {
		SYNTRACE("type_primitive", "LIST");
		$$ = new Type(Type::LIST);
	}
	| TOK_STRING {
		SYNTRACE("type_primitive", "STRING");
		$$ = new Type(Type::STRING);
	}
	| TOK_TUPLE {
		SYNTRACE("type_primitive", "TUPLE");
		$$ = new Type(Type::TUPLE);
	}
	| TOK_FLDFMT {
		SYNTRACE("type_primitive", "FLDFMT");
		$$ = new Type(Type::FLDFMT);
	}
	| TOK_VAR {
		SYNTRACE("type_primitive", "VAR");
		$$ = new Type(Type::VAR);
	}
	;

type_specifier
	: type_primitive {
		SYNTRACE("type_specifier", "type_primitive");
		$$ = $1;
	}
	| type_primitive '<' {
		lex_push_state(ST_TYPE_ARGS);
	} qualified_type_specifier_list '>' {
		lex_pop_state();
		SYNTRACE("type_specifier", "type_primitive "
		         "'<' qualified_type_specifier_list '>'");
		for (size_t i = 0; i < $4->size(); i++) {
			$1->add_argument($4->at(i));
		}
		delete $4;
		$$ = $1;
	}
	;

statement
	: labeled_statement {
		SYNTRACE("statement", "labeled_statement");
		$$ = $1;
	}
	| compound_statement {
		SYNTRACE("statement", "compound_statement");
		$$ = $1;
	}
	| expression_statement {
		SYNTRACE("statement", "expression_statement");
		$$ = $1;
	}
	| branch_statement {
		SYNTRACE("statement", "branch_statement");
		$$ = $1;
	}
	| loop_statement {
		SYNTRACE("statement", "loop_statement");
		$$ = $1;
	}
	| jump_statement {
		SYNTRACE("statement", "jump_statement");
		$$ = $1;
	}
	| definition_statement {
		SYNTRACE("statement", "definition_statement");
		$$ = $1;
	}
	;

labeled_statement
	: simple_identifier ':' statement {
		SYNTRACE("labeled_statement",
		         "simple_identifier ':' statement");
		//TODO: verify that $1 does not exist in this scope already
		$3->add_label($1);
		$$ = $3;
	}
	| TOK_CASE constant_expression ':' statement {
		SYNTRACE("labeled_statement",
		         "CASE constant_expression ':' statement");
		$$ = new CaseStatement($2, $4);
	}
	| TOK_DEFAULT ':' statement {
		SYNTRACE("labeled_statement", "DEFAULT ':' statement");
		$3->add_label(new Identifier("@switch_default"));
		$$ = $3;
	}
	;

compound_statement
	: '{' statement_list '}' {
		SYNTRACE("compound_statement", "'{' statement_list '}'");
		$$ = new CompoundStatement($2);
	}
	;

statement_list
	: {
		SYNTRACE("statement_list", "<nothing>");
		$$ = new StatementList();
	}
	| non_empty_statement_list {
		SYNTRACE("statement_list", "non_empty_statement_list");
		$$ = $1;
	}
	;

non_empty_statement_list
	: statement {
		SYNTRACE("non_empty_statement_list", "statement");
		$$ = new StatementList();
		$$->push_back($1);
	}
	| non_empty_statement_list statement {
		SYNTRACE("non_empty_statement_list",
		         "non_empty_statement_list statement");
		$1->push_back($2);
		$$ = $1;
	}
	;

expression_statement
	: ';' {
		SYNTRACE("expression_statement", "';'");
		$$ = new ExpressionStatement();
	}
	| expression ';' {
		SYNTRACE("expression_statement", "expression ';'");
		$$ = new ExpressionStatement($1);
	}
	;

branch_statement
	: TOK_IF '(' expression ')' compound_statement {
		SYNTRACE("branch_statement",
		         "IF '(' expression ')' compound_statement");
		$$ = new ConditionalStatement($3, $5);
	}
	| TOK_IF '(' expression ')' compound_statement
	  TOK_ELSE compound_statement {
		SYNTRACE("branch_statement",
		         "IF '(' expression ')' compound_statement "
		         "ELSE compound_statement");
		$$ = new ConditionalStatement($3, $5, $7);
	}
	| TOK_IF '(' expression ')' compound_statement
	  TOK_ELSE branch_statement {
		SYNTRACE("branch_statement",
		         "IF '(' expression ')' compound_statement "
		         "ELSE branch_statement");
		$$ = new ConditionalStatement($3, $5, $7);
	}
	| TOK_SWITCH '(' expression ')' compound_statement {
		SYNTRACE("branch_statement",
		         "SWITCH '(' expression ')' compound_statement");
		$$ = new SwitchStatement($3, $5);
	}
	;

// We add special labels around loops to make the implementation of break and
// continue be simpler.
loop_statement
	: TOK_WHILE '(' expression ')' compound_statement {
		SYNTRACE("loop_statement",
		         "WHILE '(' expression ')' compound_statement");
		// while (A) {B;}
		//    becomes
		// { @loop_continue: while(A) {B;} @loop_break: <nop> }
		StatementList *stmts = new StatementList();
		Statement *loop = new WhileLoopStatement($3, $5);
		loop->add_label(new Identifier("@loop_continue"));
		stmts->push_back(loop);
		Statement *nop = new NullStatement();
		nop->add_label(new Identifier("@loop_break"));
		stmts->push_back(nop);
		$$ = new CompoundStatement(stmts);
	}
	| TOK_DO compound_statement TOK_WHILE '(' expression ')' ';' {
		SYNTRACE("loop_statement",
		         "DO compound_statement WHILE '(' expression ')' ';'");
		// do {A;} while (B);
		//     becomes
		// {
		//   goto @do_while_inner;
		//   @loop_continue: while (B) {
		//       @do_while_inner: A;
		//   }
		//   @loop_break: <nop>
		// }
		StatementList *stmts = new StatementList();
		Statement *goto_inner
		    = new GotoStatement(new Identifier("@do_while_inner"));
		stmts->push_back(goto_inner);
		Statement *loop = new WhileLoopStatement($5, $2);
		loop->add_label(new Identifier("@loop_continue"));
		stmts->push_back(loop);
		Statement *nop = new NullStatement();
		nop->add_label(new Identifier("@loop_break"));
		stmts->push_back(nop);
		$$ = new CompoundStatement(stmts);
	}
	| TOK_FOR '(' expression_statement
	              expression_statement ')' compound_statement {
		SYNTRACE("loop_statement",
		         "FOR '(' expression_statement "
		         "expression_statement ')' compound_statement");
		// for (i=0; i<n;) {A;}
		//     becomes
		// { i=0; @loop_continue: while (i<n) {A;} @loop_break: <nop> }
		StatementList *stmts = new StatementList();
		stmts->push_back($3);
		Statement *loop = new WhileLoopStatement($4, $6);
		loop->add_label(new Identifier("@loop_continue"));
		stmts->push_back(loop);
		Statement *nop = new NullStatement();
		nop->add_label(new Identifier("@loop_break"));
		stmts->push_back(nop);
		$$ = new CompoundStatement(stmts);
	}
	| TOK_FOR '(' expression_statement
	              expression_statement
	              expression ')' compound_statement {
		SYNTRACE("loop_statement",
		         "FOR '(' expression_statement expression_statement "
		         "expression ')' compound_statement");
		// for (i=0; i<n; i++) {A;}
		//     becomes
		// {
		//   i=0;
		//   @loop_continue: while (i<n) {
		//     {A;}
		//     i++;
		//   }
		//   @loop_break: <nop>
		// }
		StatementList *stmts = new StatementList();
		stmts->push_back($3);
		StatementList *body_stmts = new StatementList();
		body_stmts->push_back($7);
		body_stmts->push_back(new ExpressionStatement($5));
		CompoundStatement *body = new CompoundStatement(body_stmts);
		Statement *loop = new WhileLoopStatement($4, body);
		loop->add_label(new Identifier("@loop_continue"));
		stmts->push_back(loop);
		Statement *nop = new NullStatement();
		nop->add_label(new Identifier("@loop_break"));
		stmts->push_back(nop);
		$$ = new CompoundStatement(stmts);
	}
	| TOK_FOR '(' variable_definition_statement
	              expression_statement ')' compound_statement {
		SYNTRACE("loop_statement",
		         "FOR '(' variable_definition_statement "
		         "expression_statement ')' compound_statement");
		// for (int i=0; i<n) {A;}
		//     becomes
		// { int i=0; while (i<n) {A;} }
		StatementList *stmts = new StatementList();
		stmts->push_back($3);
		Statement *loop = new WhileLoopStatement($4, $6);
		loop->add_label(new Identifier("@loop_continue"));
		stmts->push_back(loop);
		Statement *nop = new NullStatement();
		nop->add_label(new Identifier("@loop_break"));
		stmts->push_back(nop);
		$$ = new CompoundStatement(stmts);
	}
	| TOK_FOR '(' variable_definition_statement
	              expression_statement
	              expression ')' compound_statement {
		SYNTRACE("loop_statement",
		         "FOR '(' variable_definition_statement "
		         "expression_statement expression ')' "
		         "compound_statement");
		// for (int i=0; i<n; i++) {A;}
		//     becomes
		// {
		//   int i=0;
		//   @loop_continue: while (i<n) {
		//     {A;}
		//     i++;
		//   }
		//   @loop_break: <nop>
		// }
		StatementList *stmts = new StatementList();
		stmts->push_back($3);
		StatementList *body_stmts = new StatementList();
		body_stmts->push_back($7);
		body_stmts->push_back(new ExpressionStatement($5));
		CompoundStatement *body = new CompoundStatement(body_stmts);
		Statement *loop = new WhileLoopStatement($4, body);
		loop->add_label(new Identifier("@loop_continue"));
		stmts->push_back(loop);
		Statement *nop = new NullStatement();
		nop->add_label(new Identifier("@loop_break"));
		stmts->push_back(nop);
		$$ = new CompoundStatement(stmts);
	}
	;

jump_statement
	: TOK_BREAK ';' {
		SYNTRACE("jump_statement", "BREAK ';'");
		$$ = new GotoStatement(new Identifier("@loop_break"));
	}
	| TOK_CONTINUE ';' {
		SYNTRACE("jump_statement", "CONTINUE ';'");
		$$ = new GotoStatement(new Identifier("@loop_continue"));
	}
	| TOK_GOTO simple_identifier ';' {
		SYNTRACE("jump_statement", "GOTO simple_identifier ';'");
		$$ = new GotoStatement($2);
	}
	| TOK_RETURN ';' {
		SYNTRACE("jump_statement", "RETURN ';'");
		$$ = new ReturnStatement();
	}
	| TOK_RETURN expression ';' {
		SYNTRACE("jump_statement", "RETURN expression ';'");
		$$ = new ReturnStatement($2);
	}
	;

// Parsing starts here.
file_scope
	: file_scope_item {
		SYNTRACE("file_scope", "file_scope_item");
		parsed_file->push_back($1);
	}
	| file_scope file_scope_item {
		SYNTRACE("file_scope", "file_scope file_scope_item");
		parsed_file->push_back($2);
	}
	;

file_scope_item
	: definition_statement {
		SYNTRACE("file_scope_item", "definition_statement");
		$$ = $1;
	}
	| TOK_PUBLIC definition_statement {
		SYNTRACE("file_scope_item", "PUBLIC definition_statement");
		$2->set_public();
		$$ = $2;
	}
	| discover_statement {
		SYNTRACE("file_scope_item", "discover_statement");
		$$ = $1;
	}
	| import_statement {
		SYNTRACE("file_scope_item", "import_statement");
		$$ = $1;
	}
	| module_statement {
		SYNTRACE("file_scope_item", "module_statement");
		$$ = $1;
	}
	;

definition_statement
	: variable_definition_statement {
		SYNTRACE("definition_statement",
		         "variable_definition_statement");
		$$ = $1;
	}
	| function_definition_statement {
		SYNTRACE("definition_statement",
		         "function_definition_statement");
		$$ = $1;
	}
	;

variable_definition_statement
	: qualified_type_specifier initialized_identifier_list ';' {
		SYNTRACE("variable_definition_statement",
		         "qualified_type_specifier "
		         "initialized_identifier_list");
		$$ = new DefinitionStatement($1, $2);
	}
	;

initialized_identifier_list
	: initialized_identifier {
		SYNTRACE("initialized_identifier_list",
		         "initialized_identifier");
		$$ = new InitializedIdentifierList();
		$$->push_back($1);
	}
	| initialized_identifier_list ',' initialized_identifier {
		SYNTRACE("initialized_identifier_list",
		         "initialized_identifier_list "
		         "',' initialized_identifier");
		$1->push_back($3);
		$$ = $1;
	}
	;

initialized_identifier
	: simple_identifier {
		SYNTRACE("initialized_identifier", "simple_identifier");
		$$ = new InitializedIdentifier($1);
	}
	| simple_identifier '=' constant_expression {
		//TODO: this allows function calls, which is bad at file-scope
		SYNTRACE("initialized_identifier",
		         "simple_identifier '=' constant_expression");
		$$ = new InitializedIdentifier($1, $3);
	}
	;

function_definition_statement
	: simple_identifier '(' ')' '{' statement_list '}' {
		SYNTRACE("function_definition_statement",
		         "simple_identifier '(' ')' '{' statement_list '}'");
		// A(){B}  =>  func A = ${B}
		Type *type = new Type(Type::FUNC);
		InitializedIdentifierList *var_list
		    = new InitializedIdentifierList();
		Expression *body = new FunctionLiteralExpression($5);
		InitializedIdentifier *init_ident
		    = new InitializedIdentifier($1, body);
		var_list->push_back(init_ident);
		$$ = new DefinitionStatement(type, var_list);
	}
	| simple_identifier '(' parameter_declaration_list ')'
	  '{' statement_list '}' {
		// Similar to above, but with named parameters.
		Type *type = new Type(Type::FUNC);
		InitializedIdentifierList *var_list
		    = new InitializedIdentifierList();
		//FIXME: should I unroll $3 here?
		Expression *body = new FunctionLiteralExpression($3, $6);
		InitializedIdentifier *init_ident
		    = new InitializedIdentifier($1, body);
		var_list->push_back(init_ident);
		$$ = new DefinitionStatement(type, var_list);
	}
	;

parameter_declaration_list
	: parameter_declaration {
		SYNTRACE("parameter_declaration_list", "parameter_declaration");
		$$ = new ParameterDeclarationList();
		$$->push_back($1);
	}
	| parameter_declaration_list ',' parameter_declaration {
		SYNTRACE("parameter_declaration_list",
		         "parameter_declaration_list"
		         "',' parameter_declaration");
		$1->push_back($3);
		$$ = $1;
	}
	;

parameter_declaration
	: qualified_type_specifier simple_identifier {
		SYNTRACE("parameter_declaration",
		         "qualified_type_specifier simple_identifier");
		$$ = new ParameterDeclaration($1, $2);
	}
	;

import_statement
	: TOK_IMPORT string_literal ';' {
		SYNTRACE("import_statement", "IMPORT string_literal");
		$$ = new ImportStatement(*$2);
		delete $2;
	}
	;

module_statement
	: TOK_MODULE simple_identifier ';' {
		SYNTRACE("module_statement", "MODULE simple_identifier");
		$$ = new ModuleStatement($2->symbol());
		delete $2;
	}
	;

discover_statement
	: TOK_DISCOVER '(' argument_list ')' ';' {
		SYNTRACE("discover_statement",
		         "DISCOVER '(' argument_list ')' ';'");
		$$ = new DiscoverStatement($3);
	}
	;

%%

static void
pp__language__internal__error(yyscan_t scanner, Parser *parser,
                              StatementList *parsed_file, const char *str)
{
	(void)parser;
	(void)parsed_file;
	fflush(stdout);
	printf("error on line %d: %s\n", lex_lineno(), str);
}
