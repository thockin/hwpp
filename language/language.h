// Language-related stuff that needs to be exposed, since I can't inject code
// into the lex and yacc generated headers.

#ifndef HWPP_LANGUAGE_LANGUAGE_H__
#define HWPP_LANGUAGE_LANGUAGE_H__

// The grammar.l and grammar.y files include this header, which will screw
// them up if they include their own auto-generated headers, so don't include
// auto.*.h here.
#include "hwpp.h"
#include <stdio.h>
#include <map>
#include <string>
#include <vector>

// Forward declaration.
union YYSTYPE;

namespace hwpp {
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
	// This can not be a scoped_ptr because it is an incomplete type.
	LexerImpl *m_impl;

	friend class ParserImpl;
};

// Wrap all of the yacc parser logic.
class ParserImpl;
class ParsedFile;
class Parser {
 public:
	Parser();
	~Parser();

	// Equivalent of yyparse.
	int parse_file(const string &name, FILE *file,
	               ParsedFile *out_parsed_file);
	//FIXME: parse string,etc

	// Get the current line number.
	int current_line();

	// Get the current file name.
	const string &current_file();

	// This represents the current position.
	struct Position {
		string file;
		int line;
	};
	Position current_position();

 private:
	// This can not be a scoped_ptr because it is an incomplete type.
	ParserImpl *m_impl;
};

}  // namespace language
}  // namespace hwpp

#endif  // HWPP_LANGUAGE_LANGUAGE_H__

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
