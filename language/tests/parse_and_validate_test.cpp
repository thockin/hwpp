//FIXME: fix include paths
#include "pp.h"
#include "language/environment.h"
#include "language/language.h"
#include "language/parsed_file.h"
#include "util/pipe_file.h"
#include "util/test.h"

static pp::language::Environment *env;
static util::PipeFile *pipef;
static const pp::language::ParsedFile *parsed_file;

TEST_SETUP_EACH() {
	delete env;
	env = new pp::language::Environment();

	delete pipef;
	pipef = new util::PipeFile();
	TEST_ASSERT(pipef->open() == true);

	delete parsed_file;
	parsed_file = NULL;
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
	parsed_file = env->parse_file(pipef->get_read_file(), "pipe");
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
}

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
