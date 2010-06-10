#include <stdio.h>
#include <iostream>
#include "language/language.h"
#include "language/parsed_file.h"

int
main(void)
{
	pp::language::Parser parser;
	pp::language::ParsedFile parsed_file("stdin");
	parser.parse_file("<stdin>", stdin, &parsed_file);
	printf("%zd top-level nodes\n", parsed_file.n_statements());
	for (size_t i = 0; i < parsed_file.n_statements(); i++) {
		parsed_file.statement(i)->validate(0);
		std::cout << "[" << i << "] "
		          << parsed_file.statement(i)->to_string()
		          << std::endl;
	}
	return 0;
}
