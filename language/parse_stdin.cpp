#include <stdio.h>
#include <iostream>
#include <vector>
#include "language/language.h"
#include "language/parsed_file.h"

namespace pp_lang = pp::language;

int
main(void)
{
	pp_lang::Environment env;

	const pp_lang::ParsedFile *parsed_file = env.parse_file("/dev/stdin");
	printf("%zd top-level statements\n", parsed_file->n_statements());
	printf("%zd top-level symbols\n", parsed_file->n_symbols());

	env.validate(env.default_validate_options()
	                .set_warn_on_unresolved_symbols());

	for (size_t i = 0; i < parsed_file->n_statements(); i++) {
		std::cout << "[" << i << "] "
		          << parsed_file->statement(i)->to_string()
		          << std::endl;
	}

	return 0;
}

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
