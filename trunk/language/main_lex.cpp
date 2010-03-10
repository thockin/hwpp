#include <stdio.h>
#include "pp/language/language.h"
#include "pp/language/identifier.h"  // required for auto.yacc.h
#include "pp/language/variable.h"    // required for auto.yacc.h
#include "auto.yacc.h"               // required for YYSTYPE and tokens

int
main(void)
{
	pp::language::Lexer lexer;
	YYSTYPE semantic_value;

	while (!feof(stdin)) {
		lexer.lex(&semantic_value);
	}

	return 0;
}
