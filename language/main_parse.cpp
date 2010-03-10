#include <stdio.h>
#include "pp/language/language.h"

int
main(void)
{
	pp::language::Parser parser;
	pp::language::syntax::ParsedFile parsed_file;
	parser.parse_file(stdin, &parsed_file);
	printf("%d top-level nodes\n", parsed_file.size());
	return 0;
}
