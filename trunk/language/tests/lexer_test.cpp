#include <string>
using std::string;
#include "language/language.h"
#include "language/identifier.h"
#include "language/variable.h"
#include "auto.yacc.h"  // for token definitions
#define YY_HEADER_EXPORT_START_CONDITIONS 1
#include "auto.lex.h"  // for states
#include "language/string_file.h"
using pp::language::StringFile;
#include "util/test.h"

// These are used throughout the test.  Wrapped to make them simpler to use.
static pp::language::Lexer *lexer;
static YYSTYPE semantic;

static int lex()
{
	return lexer->lex(&semantic);
}
static string lexed_string()
{
	return lexer->matched_string();
}
static void restart_lexer(FILE *file)
{
	lexer->restart(file);
}
static void push_lexer_state(int state)
{
	lexer->push_state(state);
}

TEST_SETUP_EACH() {
	lexer = new pp::language::Lexer();
}

TEST_CLEANUP_EACH() {
	delete lexer;
}

TEST(test_unknown) {
	StringFile sf;
	TEST_ASSERT(sf.open("\x01"));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), UNKNOWN);
	TEST_ASSERT_EQ(lexed_string()[0], '\1');
}

TEST(test_whitespace) {
	StringFile sf;
	TEST_ASSERT(sf.open(" \t\n\v\f\r\x01"));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), UNKNOWN);
	TEST_ASSERT_EQ(lexed_string()[0], '\1');
}

TEST(test_comments) {
	StringFile sf;
	TEST_ASSERT(sf.open("/* comment */\n"
	                    "/* multi-line\n"
	                    " * comment\n"
	                    " */\n"
	                    "/* a /* nested comment */\n"
	                    "\x01"));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), UNKNOWN);
	TEST_ASSERT_EQ(lexed_string()[0], '\1');
}

TEST(test_line_comments) {
	StringFile sf;
	TEST_ASSERT(sf.open("// comment\n"
	                    "// more comments\n"
	                    "// \x01\n"
	                    "\x01"));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), UNKNOWN);
	TEST_ASSERT_EQ(lexed_string()[0], '\1');
}

TEST(test_binary_numeric_literals) {
	StringFile sf;
	TEST_ASSERT(sf.open("0b0 "
	                    "0B0 "
	                    "0b00 "
	                    "0b00000000 "
	                    "0b1 "
	                    "0b01 "
	                    "0b10 "
	                    "0b10000000 "
	                    "0b11111111 "));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0b0");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0B0");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0b00");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0b00000000");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0b1");
	TEST_ASSERT_EQ(semantic.var->int_value(), 1);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0b01");
	TEST_ASSERT_EQ(semantic.var->int_value(), 1);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0b10");
	TEST_ASSERT_EQ(semantic.var->int_value(), 2);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0b10000000");
	TEST_ASSERT_EQ(semantic.var->int_value(), 128);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0b11111111");
	TEST_ASSERT_EQ(semantic.var->int_value(), 255);

	TEST_ASSERT(sf.open("0b2"));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), UNKNOWN);
}

TEST(test_octal_numeric_literals) {
	StringFile sf;
	TEST_ASSERT(sf.open("00 "
	                    "000 "
	                    "000000000 "
	                    "01 "
	                    "001 "
	                    "010 "
	                    "010000000 "
	                    "011111111 "
	                    "001234567 "));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "00");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "000");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "000000000");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "01");
	TEST_ASSERT_EQ(semantic.var->int_value(), 1);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "001");
	TEST_ASSERT_EQ(semantic.var->int_value(), 1);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "010");
	TEST_ASSERT_EQ(semantic.var->int_value(), 8);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "010000000");
	TEST_ASSERT_EQ(semantic.var->int_value(), 010000000);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "011111111");
	TEST_ASSERT_EQ(semantic.var->int_value(), 011111111);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "001234567");
	TEST_ASSERT_EQ(semantic.var->int_value(), 001234567);

	TEST_ASSERT(sf.open("08"));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), UNKNOWN);
}

TEST(test_decimal_numeric_literals) {
	StringFile sf;
	TEST_ASSERT(sf.open("1234567890"));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "1234567890");
	TEST_ASSERT_EQ(semantic.var->int_value(), 1234567890);
}

TEST(test_hexadecimal_numeric_literals) {
	StringFile sf;
	TEST_ASSERT(sf.open("0x0 "
	                    "0X0 "
	                    "0x00 "
	                    "0x00000000 "
	                    "0x1 "
	                    "0x01 "
	                    "0x10 "
	                    "0x10000000 "
	                    "0x11111111 "
	                    "0x123456789abcdef0 "));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0x0");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0X0");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0x00");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0x00000000");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0x1");
	TEST_ASSERT_EQ(semantic.var->int_value(), 1);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0x01");
	TEST_ASSERT_EQ(semantic.var->int_value(), 1);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0x10");
	TEST_ASSERT_EQ(semantic.var->int_value(), 16);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0x10000000");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0x10000000);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0x11111111");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0x11111111);
	TEST_ASSERT_EQ(lex(), INT_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "0x123456789abcdef0");
	TEST_ASSERT_EQ(semantic.var->int_value(), 0x123456789abcdef0ULL);
}

TEST(test_string_literals) {
	StringFile sf;
	TEST_ASSERT(sf.open(
	    "\"abcde 12345 !@#$% \\\"\\x30\\060\\60\\n\\t\\\" vwxyz\""));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), STRING_LITERAL);
	TEST_ASSERT_EQ(lexed_string(),
	    "\"abcde 12345 !@#$% \\\"\\x30\\060\\60\\n\\t\\\" vwxyz\"");
	TEST_ASSERT_EQ(semantic.var->string_value(),
	    "abcde 12345 !@#$% \"\x30\060\60\n\t\" vwxyz");
}

TEST(test_boolean_literals) {
	StringFile sf;
	TEST_ASSERT(sf.open("true false"));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), BOOL_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "true");
	TEST_ASSERT_EQ(semantic.var->bool_value(), true);
	TEST_ASSERT_EQ(lex(), BOOL_LITERAL);
	TEST_ASSERT_EQ(lexed_string(), "false");
	TEST_ASSERT_EQ(semantic.var->bool_value(), false);
}

TEST(test_types) {
	StringFile sf;
	TEST_ASSERT(sf.open("const "
	                    "bool "
	                    "func "
	                    "int "
	                    "list "
	                    "string "
	                    "tuple "
	                    "type "
	                    "var "));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), CONST);
	TEST_ASSERT_EQ(lexed_string(), "const");
	TEST_ASSERT_EQ(lex(), BOOL);
	TEST_ASSERT_EQ(lexed_string(), "bool");
	TEST_ASSERT_EQ(lex(), FUNC);
	TEST_ASSERT_EQ(lexed_string(), "func");
	TEST_ASSERT_EQ(lex(), INT);
	TEST_ASSERT_EQ(lexed_string(), "int");
	TEST_ASSERT_EQ(lex(), LIST);
	TEST_ASSERT_EQ(lexed_string(), "list");
	TEST_ASSERT_EQ(lex(), STRING);
	TEST_ASSERT_EQ(lexed_string(), "string");
	TEST_ASSERT_EQ(lex(), TUPLE);
	TEST_ASSERT_EQ(lexed_string(), "tuple");
	TEST_ASSERT_EQ(lex(), TYPE);
	TEST_ASSERT_EQ(lexed_string(), "type");
	TEST_ASSERT_EQ(lex(), VAR);
	TEST_ASSERT_EQ(lexed_string(), "var");
}

TEST(test_type_args)
{
	StringFile sf;

	// Each piece of this test simulates having just parsed something
	// like "list<", where it expects typ-arguments next.

	TEST_ASSERT(sf.open("int>\x01\n"));
	restart_lexer(sf.get_read_file());
	push_lexer_state(TYPE_ARGS);
	TEST_ASSERT_EQ(lex(), INT);
	TEST_ASSERT_EQ(lex(), '>');
	TEST_ASSERT_EQ(lex(), UNKNOWN);
	TEST_ASSERT_EQ(lexed_string()[0], '\1');

	TEST_ASSERT(sf.open("not_a_type>\x01\n"));
	restart_lexer(sf.get_read_file());
	push_lexer_state(TYPE_ARGS);
	TEST_ASSERT_EQ(lex(), UNKNOWN);
	TEST_ASSERT_EQ(lexed_string(), "n");

	TEST_ASSERT(sf.open("int, int>\x01\n"));
	restart_lexer(sf.get_read_file());
	push_lexer_state(TYPE_ARGS);
	TEST_ASSERT_EQ(lex(), INT);
	TEST_ASSERT_EQ(lex(), ',');
	TEST_ASSERT_EQ(lex(), INT);
	TEST_ASSERT_EQ(lex(), '>');
	TEST_ASSERT_EQ(lex(), UNKNOWN);
	TEST_ASSERT_EQ(lexed_string()[0], '\1');

	TEST_ASSERT(sf.open("list<int,int>>\x01\n"));
	restart_lexer(sf.get_read_file());
	push_lexer_state(TYPE_ARGS);
	TEST_ASSERT_EQ(lex(), LIST);
	TEST_ASSERT_EQ(lex(), '<');
	TEST_ASSERT_EQ(lex(), INT);
	TEST_ASSERT_EQ(lex(), ',');
	TEST_ASSERT_EQ(lex(), INT);
	TEST_ASSERT_EQ(lex(), '>');
	TEST_ASSERT_EQ(lex(), '>');
	TEST_ASSERT_EQ(lex(), UNKNOWN);
	TEST_ASSERT_EQ(lexed_string()[0], '\1');

	TEST_ASSERT(sf.open("list<int>, list<list<int>>>\x01\n"));
	restart_lexer(sf.get_read_file());
	push_lexer_state(TYPE_ARGS);
	TEST_ASSERT_EQ(lex(), LIST);
	TEST_ASSERT_EQ(lex(), '<');
	TEST_ASSERT_EQ(lex(), INT);
	TEST_ASSERT_EQ(lex(), '>');
	TEST_ASSERT_EQ(lex(), ',');
	TEST_ASSERT_EQ(lex(), LIST);
	TEST_ASSERT_EQ(lex(), '<');
	TEST_ASSERT_EQ(lex(), LIST);
	TEST_ASSERT_EQ(lex(), '<');
	TEST_ASSERT_EQ(lex(), INT);
	TEST_ASSERT_EQ(lex(), '>');
	TEST_ASSERT_EQ(lex(), '>');
	TEST_ASSERT_EQ(lex(), '>');
	TEST_ASSERT_EQ(lex(), UNKNOWN);
	TEST_ASSERT_EQ(lexed_string()[0], '\1');
}

TEST(test_file_scope_keywords)
{
	StringFile sf;
	TEST_ASSERT(sf.open("import "
	                    "module "
	                    "public "));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), IMPORT);
	TEST_ASSERT_EQ(lexed_string(), "import");
	TEST_ASSERT_EQ(lex(), MODULE);
	TEST_ASSERT_EQ(lexed_string(), "module");
	TEST_ASSERT_EQ(lex(), PUBLIC);
	TEST_ASSERT_EQ(lexed_string(), "public");
}

TEST(test_if_else) {
	StringFile sf;
	TEST_ASSERT(sf.open("if else"));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), IF);
	TEST_ASSERT_EQ(lexed_string(), "if");
	TEST_ASSERT_EQ(lex(), ELSE);
	TEST_ASSERT_EQ(lexed_string(), "else");
}

TEST(test_loops) {
	StringFile sf;
	TEST_ASSERT(sf.open("do "
	                    "while "
	                    "for "
	                    "break "
	                    "continue "));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), DO);
	TEST_ASSERT_EQ(lexed_string(), "do");
	TEST_ASSERT_EQ(lex(), WHILE);
	TEST_ASSERT_EQ(lexed_string(), "while");
	TEST_ASSERT_EQ(lex(), FOR);
	TEST_ASSERT_EQ(lexed_string(), "for");
	TEST_ASSERT_EQ(lex(), BREAK);
	TEST_ASSERT_EQ(lexed_string(), "break");
	TEST_ASSERT_EQ(lex(), CONTINUE);
	TEST_ASSERT_EQ(lexed_string(), "continue");
}

TEST(test_return) {
	StringFile sf;
	TEST_ASSERT(sf.open("return"));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), RETURN);
	TEST_ASSERT_EQ(lexed_string(), "return");
}

TEST(test_switch) {
	StringFile sf;
	TEST_ASSERT(sf.open("switch case default"));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), SWITCH);
	TEST_ASSERT_EQ(lexed_string(), "switch");
	TEST_ASSERT_EQ(lex(), CASE);
	TEST_ASSERT_EQ(lexed_string(), "case");
	TEST_ASSERT_EQ(lex(), DEFAULT);
	TEST_ASSERT_EQ(lexed_string(), "default");
}

TEST(test_identifiers) {
	StringFile sf;
	TEST_ASSERT(sf.open("abc123 _ABC_123_"));
	restart_lexer(sf.get_read_file());
	TEST_ASSERT_EQ(lex(), IDENTIFIER);
	TEST_ASSERT_EQ(lexed_string(), "abc123");
	TEST_ASSERT_EQ(lex(), IDENTIFIER);
	TEST_ASSERT_EQ(lexed_string(), "_ABC_123_");
}
