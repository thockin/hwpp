/* This file is written assuming that it will be compiled as C++ code */
DEC			[0-9]
HEX			[a-fA-F0-9]
ALPHA			[a-zA-Z_]
SPACE			[ \t\n\v\f\r]

/* Let flex track line numbers. */
%option yylineno
/* No need for yywrap. */
%option noyywrap
/* Enable start condition stacks. */
%option stack
/* Define a prefix that should make this code hard to collide with. */
%option prefix="pp__language__internal__"
/* This is intended to be used with bison. */
%option bison-bridge
/* Remarks in the flex 'info' page and the generated code have scared me away
 * from using the C++ scanner option. */
%option reentrant
/* Keep extra state for the lexer. */
%option extra-type="pp::language::Lexer::ExtraState *"

%{

#include <ctype.h>
#include <stdio.h>
#include <stdexcept>
#include <string>
#include "pp/pp.h"
#include "pp/language/language.h"
#include "pp/language/syntax_tree.h"
#include "pp/language/auto.yacc.h"
#include "pp/util/printfxx.h"

using namespace pp;
using namespace pp::language;

static void dump(yyscan_t scanner);
static int int_literal(yyscan_t scanner);
static int string_literal(yyscan_t scanner);
static int bool_literal(yyscan_t scanner);
static int identifier(yyscan_t scanner);

void pp__language__internal__push_lexer_state(int new_state, yyscan_t scanner);
void pp__language__internal__pop_lexer_state(yyscan_t scanner);
#define push_state pp__language__internal__push_lexer_state
#define pop_state pp__language__internal__pop_lexer_state

%}

/* Handle C-style comments. */
%x ST_COMMENT

/* Handles line comments. */
%x ST_LINE_COMMENT

/*
 * This state manages the case of template-style types (list, tuple) being
 * lexed.  They are really only necessary to enable proper parsing of nested
 * declarations, so that list<list<int>> doesn't require a stupid blank space
 * between the closing brackets.  It is inclusive since most match rules have
 * explicit start conditions.
 */
%s ST_TYPE_ARGS

%%

	/* Support C-style comments. */
"/*"			{ push_state(ST_COMMENT, yyscanner); }
<ST_COMMENT>"*/"		{ pop_state(yyscanner); }
<ST_COMMENT>[^*]+		{ /* eat comments */ }
<ST_COMMENT>"*"		{ /* eat comments */ }

	/* Support C++-style line comments. */
"//"			{ push_state(ST_LINE_COMMENT, yyscanner); }
<ST_LINE_COMMENT>\n	{ pop_state(yyscanner); }
<ST_LINE_COMMENT>[^\n]+	{ /* eat comments */ }

	/* Whitespace is not significant. */
{SPACE}+		{ /* ignore whitespace */ }

	/* Literals. The actual digits are validated in int_literal(). */
0[bB]{DEC}+		{ dump(yyscanner); return int_literal(yyscanner); }
0[xX]{HEX}+		{ dump(yyscanner); return int_literal(yyscanner); }
{DEC}+			{ dump(yyscanner); return int_literal(yyscanner); }

\"(\\.|[^\\"])*\"	{ dump(yyscanner); return string_literal(yyscanner); }

true			{ dump(yyscanner); return bool_literal(yyscanner); }
false			{ dump(yyscanner); return bool_literal(yyscanner); }

<INITIAL,ST_TYPE_ARGS>
const			{ dump(yyscanner); return TOK_CONST; }
<INITIAL,ST_TYPE_ARGS>
bool			{ dump(yyscanner); return TOK_BOOL; }
<INITIAL,ST_TYPE_ARGS>
func			{ dump(yyscanner); return TOK_FUNC; }
<INITIAL,ST_TYPE_ARGS>
int			{ dump(yyscanner); return TOK_INT; }
<INITIAL,ST_TYPE_ARGS>
list			{ dump(yyscanner); return TOK_LIST; }
<INITIAL,ST_TYPE_ARGS>
string			{ dump(yyscanner); return TOK_STRING; }
<INITIAL,ST_TYPE_ARGS>
tuple			{ dump(yyscanner); return TOK_TUPLE; }
<INITIAL,ST_TYPE_ARGS>
fldfmt			{ dump(yyscanner); return TOK_FLDFMT; }
<INITIAL,ST_TYPE_ARGS>
var			{ dump(yyscanner); return TOK_VAR; }

<INITIAL>
import			{ dump(yyscanner); return TOK_IMPORT; }
<INITIAL>
discover		{ dump(yyscanner); return TOK_DISCOVER; }
<INITIAL>
module			{ dump(yyscanner); return TOK_MODULE; }
<INITIAL>
public			{ dump(yyscanner); return TOK_PUBLIC; }

<INITIAL>
if			{ dump(yyscanner); return TOK_IF; }
<INITIAL>
else			{ dump(yyscanner); return TOK_ELSE; }

<INITIAL>
do			{ dump(yyscanner); return TOK_DO; }
<INITIAL>
while			{ dump(yyscanner); return TOK_WHILE; }
<INITIAL>
for			{ dump(yyscanner); return TOK_FOR; }
<INITIAL>
break			{ dump(yyscanner); return TOK_BREAK; }
<INITIAL>
continue		{ dump(yyscanner); return TOK_CONTINUE; }

<INITIAL>
return			{ dump(yyscanner); return TOK_RETURN; }

<INITIAL>
switch			{ dump(yyscanner); return TOK_SWITCH; }
<INITIAL>
case			{ dump(yyscanner); return TOK_CASE; }
<INITIAL>
default			{ dump(yyscanner); return TOK_DEFAULT; }

<INITIAL>
{ALPHA}({ALPHA}|{DEC})* { dump(yyscanner); return identifier(yyscanner); }

<INITIAL>
"$"			{ dump(yyscanner); return TOK_FUNC_LITERAL; }
<INITIAL>
">>="			{ dump(yyscanner); return TOK_SHR_ASSIGN; }
<INITIAL>
"<<="			{ dump(yyscanner); return TOK_SHL_ASSIGN; }
<INITIAL>
"+="			{ dump(yyscanner); return TOK_ADD_ASSIGN; }
<INITIAL>
"-="			{ dump(yyscanner); return TOK_SUB_ASSIGN; }
<INITIAL>
"*="			{ dump(yyscanner); return TOK_MUL_ASSIGN; }
<INITIAL>
"/="			{ dump(yyscanner); return TOK_DIV_ASSIGN; }
<INITIAL>
"%="			{ dump(yyscanner); return TOK_MOD_ASSIGN; }
<INITIAL>
"&="			{ dump(yyscanner); return TOK_AND_ASSIGN; }
<INITIAL>
"^="			{ dump(yyscanner); return TOK_XOR_ASSIGN; }
<INITIAL>
"|="			{ dump(yyscanner); return TOK_OR_ASSIGN; }
<INITIAL>
">>"			{ dump(yyscanner); return TOK_SHR; }
<INITIAL>
"<<"			{ dump(yyscanner); return TOK_SHL; }
<INITIAL>
"++"			{ dump(yyscanner); return TOK_INC; }
<INITIAL>
"--"			{ dump(yyscanner); return TOK_DEC; }
<INITIAL>
"&&"			{ dump(yyscanner); return TOK_AND_AND; }
<INITIAL>
"||"			{ dump(yyscanner); return TOK_OR_OR; }
<INITIAL>
"<="			{ dump(yyscanner); return TOK_LE; }
<INITIAL>
">="			{ dump(yyscanner); return TOK_GE; }
<INITIAL>
"=="			{ dump(yyscanner); return TOK_EQ; }
<INITIAL>
"!="			{ dump(yyscanner); return TOK_NE; }
<INITIAL>
";"			{ dump(yyscanner); return ';'; }
<INITIAL>
"{"			{ dump(yyscanner); return '{'; }
<INITIAL>
"}"			{ dump(yyscanner); return '}'; }
<INITIAL,ST_TYPE_ARGS>
","			{ dump(yyscanner); return ','; }
<INITIAL>
":"			{ dump(yyscanner); return ':'; }
<INITIAL>
"="			{ dump(yyscanner); return '='; }
<INITIAL>
"("			{ dump(yyscanner); return '('; }
<INITIAL>
")"			{ dump(yyscanner); return ')'; }
<INITIAL>
"["			{ dump(yyscanner); return '['; }
<INITIAL>
"]"			{ dump(yyscanner); return ']'; }
<INITIAL>
"."			{ dump(yyscanner); return '.'; }
<INITIAL>
"&"			{ dump(yyscanner); return '&'; }
<INITIAL>
"!"			{ dump(yyscanner); return '!'; }
<INITIAL>
"~"			{ dump(yyscanner); return '~'; }
<INITIAL>
"-"			{ dump(yyscanner); return '-'; }
<INITIAL>
"+"			{ dump(yyscanner); return '+'; }
<INITIAL>
"*"			{ dump(yyscanner); return '*'; }
<INITIAL>
"/"			{ dump(yyscanner); return '/'; }
<INITIAL>
"%"			{ dump(yyscanner); return '%'; }
<INITIAL,ST_TYPE_ARGS>
"<"			{ dump(yyscanner); return '<'; }
<INITIAL,ST_TYPE_ARGS>
">"			{ dump(yyscanner); return '>'; }
<INITIAL>
"^"			{ dump(yyscanner); return '^'; }
<INITIAL>
"|"			{ dump(yyscanner); return '|'; }
<INITIAL>
"?"			{ dump(yyscanner); return '?'; }

<*>.			{ dump(yyscanner); return TOK_UNKNOWN; }

%%

//
// Code here can use the various lex-provided symbols.
//

// Debug printing.
static void
dump(yyscan_t scanner)
{
	struct yyguts_t *yyg = (struct yyguts_t*)scanner;
	fprintf(stderr, "DBG: lex(%d): %s\n", YY_START, yyget_text(scanner));
}

static bool validate_decimal_literal(const char *text)
{
	size_t len = strlen(text);
	for (size_t i = 0; i < len; i++) {
		if (!isdigit(text[i])) {
			return false;
		}
	}
	return true;
}

bool validate_binary_literal(const char *text)
{
	size_t len = strlen(text);
	for (size_t i = 0; i < len; i++) {
		if (text[i] != '0' && text[i] != '1') {
			return false;
		}
	}
	return true;
}

bool validate_hex_literal(const char *text)
{
	size_t len = strlen(text);
	for (size_t i = 0; i < len; i++) {
		if (!isxdigit(text[i])) {
			return false;
		}
	}
	return true;
}

bool validate_octal_literal(const char *text)
{
	size_t len = strlen(text);
	for (size_t i = 0; i < len; i++) {
		if (!(text[i] >= '0' && text[i] <= '7')) {
			return false;
		}
	}
	return true;
}

// Process a numeric literal.
static int int_literal(yyscan_t scanner)
{
	const char *text = yyget_text(scanner);

	if (strlen(text) == 1 || text[0] != '0') {
		if (!validate_decimal_literal(text)) {
			WARN(sprintfxx("invalid int literal: \"%s\"", text));
			return TOK_UNKNOWN;
		}
	} else if (isdigit(text[1])) {
		if (!validate_octal_literal(text)) {
			WARN(sprintfxx("invalid octal literal: \"%s\"", text));
			return TOK_UNKNOWN;
		}
	} else if (strlen(text) == 2) {
		// We should not be able to get here.
		WARN(sprintfxx("invalid int literal: \"%s\"", text));
		return TOK_UNKNOWN;
	} else if (tolower(text[1]) == 'b') {
		if (!validate_binary_literal(text+2)) {
			WARN(sprintfxx("invalid binary literal: \"%s\"", text));
			return TOK_UNKNOWN;
		}
	} else if (tolower(text[1]) == 'x') {
		if (!validate_hex_literal(text+2)) {
			WARN(sprintfxx("invalid hex literal: \"%s\"", text));
			return TOK_UNKNOWN;
		}
	} else {
		WARN(sprintfxx("invalid int literal: \"%s\"", text));
		return TOK_UNKNOWN;
	}

	Value *val = new Value();
	try {
		// Despite our checks above, paranoia says this might fail.
		*val = text;
	} catch (std::exception &e) {
		WARN(sprintfxx("invalid numeric literal: \"%s\"", text));
		return TOK_UNKNOWN;
	}
	yyget_lval(scanner)->int_val = val;
	return TOK_INT_LITERAL;
}

// Un-escape a quoted string.
static string unescape_qstr(const string &in)
{
	string out;

	// Sanity checks.
	if (in.size() < 2) {
		// Too short to possibly be useful.
		return in;
	}
	if (*(in.begin()) != '"' || *(in.end()-1) != '"') {
		// Not quoted.
		return in;
	}

	for (size_t i = 1; i < in.size()-1; i++) {
		char c = in[i];
		if (c == '\\') {
			c = in[++i];
			switch (c) {
				case 'a':
					out += '\a';
					break;
				case 'b':
					out += '\b';
					break;
				case 'f':
					out += '\f';
					break;
				case 'n':
					out += '\n';
					break;
				case 'r':
					out += '\r';
					break;
				case 't':
					out += '\t';
					break;
				case 'v':
					out += '\v';
					break;
				case 'x': {
					// Handle \xHH hex codes.
					c = in[i+1];  // peek ahead
					if (!isxdigit(c)) {
						// Fall back on literal 'x'.
						out += 'x';
						break;
					}
					i++;  // keep this char, advance i
					char n = 0;
					if (c >= '0' && c <= '9') {
						n = c - '0';
					} else {
						n = 10 + (tolower(c) - 'a');
					}
					c = in[i+1];  // peek ahead again
					if (!isxdigit(c)) {
						out += n;
						break;
					}
					i++;  // keep this char, advance i
					if (c >= '0' && c <= '9') {
						n *= 16;
						n += c - '0';
					} else {
						n *= 16;
						n += 10 + (tolower(c) - 'a');
					}
					out += n;
					break;
				}
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7': {
					// Handle \xOOO octal codes.
					char n = c - '0';
					c = in[i+1];  // peek ahead
					if (c >= '0' && c <= '7') {
						i++;  // keep this char
						n *= 8;
						n += c - '0';
					} else {
						out += n;
						break;
					}
					c = in[i+1];  // peek ahead again
					if (c >= '0' && c <= '7') {
						i++;  // keep this char
						n *= 8;
						n += c - '0';
					} else {
						out += n;
						break;
					}
					out += n;
					break;
				}
				default:
					out += c;
					break;
			}
		} else {
			out += c;
		}
	}

	return out;
}

// Process a string literal.
static int string_literal(yyscan_t scanner)
{
	yyget_lval(scanner)->string_val
	    = new string(unescape_qstr(yyget_text(scanner)));
	return TOK_STRING_LITERAL;
}

// Process a boolean literal.
static int bool_literal(yyscan_t scanner)
{
	if (string(yyget_text(scanner)) == "true") {
		yyget_lval(scanner)->bool_val = true;
	} else {
		yyget_lval(scanner)->bool_val = false;
	}
	return TOK_BOOL_LITERAL;
}

// Process an identifier.
static int identifier(yyscan_t scanner)
{
	yyget_lval(scanner)->string_val = new string(yyget_text(scanner));
	return TOK_IDENTIFIER;
}

//
// Expose some functions.
//

// Push/pop a lexer state.  These are exposed so that the parser, which knows
// more about the actual language states, can give the lexer clues.  For
// example, ">>" is ambiguous.  In any sort of expression state, it means
// right-shift.  When parsing template-args right-shift does not apply, but
// ">>" can mean the end of a nested template-arg declaration.
void pp__language__internal__push_lexer_state(int new_state, yyscan_t scanner)
{
	//struct yyguts_t *yyg = (struct yyguts_t*)scanner;
	//printf("DBG: PUSH %d -> %d\n", YY_START, new_state);
	yy_push_state(new_state, scanner);
}
void pp__language__internal__pop_lexer_state(yyscan_t scanner)
{
	yy_pop_state(scanner);
	//struct yyguts_t *yyg = (struct yyguts_t*)scanner;
	//printf("DBG: POP -> %d\n", YY_START);
}
