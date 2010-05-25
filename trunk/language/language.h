// Language-related stuff that needs to be exposed, since I can't inject code
// into the lex and yacc generated headers.

#ifndef PP_LANGAUGE_LANGUAGE_H__
#define PP_LANGAUGE_LANGUAGE_H__

// The grammar.l and grammar.y files include this header, which will screw
// them up if they include their own auto-generated headers, so don't include
// auto.*.h here.
#include "pp/pp.h"
#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include "pp/language/symbol_table.h"
#include "pp/language/syntax_tree.h"

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

// FIXME: this should probably move to it's own header
#if 0
class Runtime;
class ParsedFile {
    public:
	ParsedFile()
	    : m_imports(),
	      m_public_symbols(),
	      m_private_symbols()
	{
	}
	~ParsedFile()
	{
	}

	// Get the public symbol table for this file.
	const SymbolTable *
	public_symbols() const
	{
		return &m_public_symbols;
	}
	SymbolTable *
	public_symbols()
	{
		return &m_public_symbols;
	}

	// Get the private symbol table for this file.
	const SymbolTable *
	private_symbols() const
	{
		return &m_private_symbols;
	}
	SymbolTable *
	private_symbols()
	{
		return &m_private_symbols;
	}

	bool
	set_module_name(const string &name)
	{
		if (!m_module_name.empty()) {
			WARN("module names can only be set once");
			return false;
		}
		m_module_name = name;
		return true;
	}

	void
	add_import(const string &name)
	{
		for (size_t i = 0; i < m_imports.size(); i++) {
			if (m_imports[i] == name) {
				WARN(sprintfxx(
				    "repeated import of \"%s\"", name));
				return;
			}
		}
		m_imports.push_back(name);
	}

	// Look up all the imported files and import their public symbols into
	// this file's symbol table.
	bool
	resolve_imports(const Runtime *runtime)
	{
		for (std::vector<string>::iterator it = m_imports.begin();
		     it != m_imports.end(); ++it) {
			// ParsedFile *pf = runtime->lookup_file(*it);
			// if (!pf) {
			// 	//FIXME: error message
			// 	return false;
			// }
			// m_private_symbols.merge_from(pf->public_symbols());
		}
		return true;
	}

    private:
	// All the files that this file imports.
	std::vector<string> m_imports;
	// All the public symbols in this file.
	SymbolTable m_public_symbols;
	// All the private symbols in this file.
	SymbolTable m_private_symbols;
	// The current module name.
	string m_module_name;
};

// All the runtime state of the interpreter.
// FIXME: this should probably move to it's own header
class Runtime {
    public:
	Runtime()
	{
	}

	~Runtime()
	{
	}

	ParsedFile *new_file(const string &file_name)
	{
		return &m_files[file_name];
	}

    private:
	std::map<string, ParsedFile> m_files;
};
#endif

// Wrap all of the yacc parser logic.
class ParserImpl;
class Parser {
    public:
	Parser();
	~Parser();

	// Equivalent of yyparse.
	int parse_file(FILE *file, syntax::StatementList *out_statements);
	//FIXME: parse string,etc

    private:
	ParserImpl *m_impl;
};

}  // namespace language
}  // namespace pp

#endif  // PP_LANGAUGE_LANGUAGE_H__
