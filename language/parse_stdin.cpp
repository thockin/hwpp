#include <stdio.h>
#include <iostream>
#include "pp/language/language.h"

int
main(void)
{
	pp::language::Parser parser;
	pp::language::syntax::StatementList file_statements;
	parser.parse_file(stdin, &file_statements);
	printf("%zd top-level nodes\n", file_statements.size());
	for (size_t i = 0; i < file_statements.size(); i++) {
		std::cout << "[" << i << "] "
		          << file_statements[i]->to_string()
		          << std::endl;
	}
	return 0;
}
