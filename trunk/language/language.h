// Language-related stuff that needs to be exposed, since I can't inject code
// into the lex and yacc generated headers.

#ifndef PP_LANGUAGE_LANGUAGE_H__
#define PP_LANGUAGE_LANGUAGE_H__

// The grammar.l and grammar.y files include this header, which will screw
// them up if they include their own auto-generated headers, so don't include
// auto.*.h here.
#include "pp.h"
#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include "language/parsed_file.h"
#include "language/syntax_tree.h"

// Forward declaration.
union YYSTYPE;

namespace pp {
namespace language {

// Wrap all of the lex logic.
class LexerImpl;
class Lexer {
    public:
	struct ExtraState {
	};

	Lexer();
	~Lexer();

	// Equivalent of yylex.
	int lex(YYSTYPE *semantic);

	// Equivalent of yylineno.
	int line_number() const;

	// Equivalent of yytext.
	string matched_string() const;

	// Equivalent of yyrestart.
	void restart(FILE *file);

	// Equivalent of yy_push_state
	void push_state(int state);

    protected:
	LexerImpl *internal_impl() const
	{
		return m_impl;
	}

    private:
	LexerImpl *m_impl;

	friend class ParserImpl;
};

// Wrap all of the yacc parser logic.
class ParserImpl;
class Parser {
    public:
	Parser();
	~Parser();

	// Equivalent of yyparse.
	int parse_file(FILE *file, ParsedFile *out_parsed_file);
	//FIXME: parse string,etc

    private:
	ParserImpl *m_impl;
};

}  // namespace language
}  // namespace pp

#endif  // PP_LANGUAGE_LANGUAGE_H__
