#include <stdio.h>
#include <iostream>
#include "pp/language/language.h"

int
main(void)
{
	pp::language::Parser parser;
	pp::language::syntax::StatementList parsed_file;
	parser.parse_file(stdin, &parsed_file);
	printf("%zd top-level nodes\n", parsed_file.size());
	for (size_t i = 0; i < parsed_file.size(); i++) {
		std::cout << "[" << i << "] "
		          << parsed_file[i]->to_string()
		          << std::endl;
	}
	return 0;
}
