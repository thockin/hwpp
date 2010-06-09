#include "language/language.h"
#include <stdio.h>
#include <string>
#include "pp.h"
#include "language/parsed_file.h"
#include "language/syntax_tree.h"
// Leave these last.
#include "auto.yacc.h"
#include "auto.lex.h"  // depends on auto.yacc.h being first

// For some reason auto.yacc.h does not export this.
extern int pp__language__internal__parse(yyscan_t scanner,
    pp::language::Parser *parser,
    pp::language::ParsedFile *out_parsed_file);

// These are exposed from the lexer to the parser and for testing.
extern void pp__language__internal__push_lexer_state(int new_state,
                                                     yyscan_t scanner);
extern void pp__language__internal__pop_lexer_state(yyscan_t scanner);

namespace pp {
namespace language {

// This is the actual lexer class implementation.  It is entirely contained in
// this file.
class LexerImpl {
    public:
	LexerImpl()
	{
		pp__language__internal__lex_init_extra(&m_extra, &m_scanner);
	}

	~LexerImpl()
	{
		pp__language__internal__lex_destroy(m_scanner);
	}

	int lex(YYSTYPE *semantic)
	{
		return pp__language__internal__lex(semantic, m_scanner);
	}

	int line_number() const
	{
		return pp__language__internal__get_lineno(m_scanner);
	}

	string matched_string() const
	{
		return pp__language__internal__get_text(m_scanner);
	}

	void restart(FILE *file)
	{
		pp__language__internal__restart(file, m_scanner);
	}

	void push_state(int state)
	{
		pp__language__internal__push_lexer_state(state, m_scanner);
	}

	yyscan_t scanner() const
	{
		return m_scanner;
	}

    private:
	yyscan_t m_scanner;
	Lexer::ExtraState m_extra;
};

Lexer::Lexer()
{
	m_impl = new LexerImpl();
}

Lexer::~Lexer()
{
	delete m_impl;
}

int Lexer::lex(YYSTYPE *semantic)
{
	return m_impl->lex(semantic);
}

int Lexer::line_number() const
{
	return m_impl->line_number();
}

string Lexer::matched_string() const
{
	return m_impl->matched_string();
}

void Lexer::restart(FILE *file)
{
	return m_impl->restart(file);
}

void Lexer::push_state(int state)
{
	return m_impl->push_state(state);
}

//
// This is the actual parser class implementation.  It is entirely contained
// in this file.
//
class ParserImpl {
    public:
	ParserImpl(Parser *outer) : m_outer_parser(outer)
	{
	}

	~ParserImpl()
	{
	}

	int parse_file(const string &name, FILE *file,
	               ParsedFile *out_parsed_file)
	{
		m_name = name;
		m_lexer.restart(file);
		yyscan_t scanner = m_lexer.internal_impl()->scanner();
		return pp__language__internal__parse(scanner, m_outer_parser,
		                                     out_parsed_file);
	}

	int
	current_line()
	{
		yyscan_t scanner = m_lexer.internal_impl()->scanner();
		return pp__language__internal__get_lineno(scanner);
	}

	const string &
	current_file()
	{
		return m_name;
	}

    private:
	Parser *m_outer_parser;
	Lexer m_lexer;
	string m_name;
};

Parser::Parser()
{
	m_impl = new ParserImpl(this);
}

Parser::~Parser()
{
	delete m_impl;
}

int
Parser::parse_file(const string &name, FILE *file,
                       ParsedFile *out_parsed_file)
{
	return m_impl->parse_file(name, file, out_parsed_file);
}

int
Parser::current_line()
{
	return m_impl->current_line();
}

const string &
Parser::current_file()
{
	return m_impl->current_file();
}

Parser::Position
Parser::current_position()
{
	Position p;
	p.file = current_file();
	p.line = current_line();
	return p;
}

}  // namespace language
}  // namespace pp
