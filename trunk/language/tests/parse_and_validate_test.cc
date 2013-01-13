//FIXME: fix include paths
#include "hwpp.h"
#include "language/environment.h"
#include "language/language.h"
#include "language/parsed_file.h"
#include "util/pipe_file.h"
#include "util/test.h"

static hwpp::language::Environment *env;
static util::PipeFile *pipef;
static const hwpp::language::ParsedFile *parsed_file;

static void reinit()
{
	delete env;
	env = new hwpp::language::Environment();

	delete pipef;
	pipef = new util::PipeFile();
	TEST_ASSERT(pipef->open() == true);

	parsed_file = NULL;
}

TEST_SETUP_EACH() {
	reinit();
}

static void
write_to_pipe(const string &str)
{
	size_t written = fwrite(str.data(), 1, str.size(), pipef->get_write_file());
	TEST_ASSERT(written == str.size());
	pipef->close_write_file();
}

static int
parse_and_validate(const string &str)
{
	write_to_pipe(str);
	parsed_file = env->parse_file("pipe", pipef->get_read_file());
	return env->validate(env->default_validate_options()
	                        .set_warn_on_unresolved_symbols());
}

TEST(test_basic_definitions) {
	TEST_ASSERT(parse_and_validate(
	    "bool b; \n"
	    "func f; \n"
	    "int i; \n"
	    "list<int> li; \n"
	    "string s; \n"
	    "tuple<int,string> tis; \n"
	    "var v; \n"
	) == 0);
	reinit();

	TEST_ASSERT(parse_and_validate(
	    "bool b = true; \n"
	    "func f = ${}; \n"
	    "int i = -1; \n"
	    "list<int> li = [93,76]; \n"
	    "string s = \"foo\"; \n"
	    "tuple<int,string> tis = [93, \"seven-six\"]; \n"
	    "var v = 12345; \n"
	) == 0);
	reinit();

	TEST_ASSERT(parse_and_validate(
	    "list l = [ 0, 0 ]; \n"
	    "tuple t = [ 93, \"seven-six\" ]; \n"
	) == 0);
	reinit();

	TEST_ASSERT(parse_and_validate(
	    "const tuple t = [ false ]; \n"
	    "bool b = t[0]; \n"
	) == 0);
	reinit();

	try {
		parse_and_validate(
		    "const list l = [true, true]; \n"
		    "int i = l[0]; \n"
		);
		TEST_FAIL();
	} catch (hwpp::language::SyntaxError &e) {
	} catch (...) {
		TEST_FAIL();
	}
	reinit();

	try {
		parse_and_validate(
		    "const tuple t = [ false ]; \n"
		    "int i = t[0]; \n"
		);
		TEST_FAIL();
	} catch (hwpp::language::SyntaxError &e) {
	} catch (...) {
		TEST_FAIL();
	}
	reinit();

	try {
		parse_and_validate(
		    "const tuple t = [ false ]; \n"
		    "var i = t[1]; \n"
		);
		TEST_FAIL();
	} catch (hwpp::language::SyntaxError &e) {
	} catch (...) {
		TEST_FAIL();
	}
	reinit();
}

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
