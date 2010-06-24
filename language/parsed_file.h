#ifndef PP_LANGUAGE_PARSED_FILE_H__
#define PP_LANGUAGE_PARSED_FILE_H__

// The grammar.l and grammar.y files include this header, which will screw
// them up if they include their own auto-generated headers, so don't include
// auto.*.h here.
#include "pp.h"
#include <map>
#include <string>
#include <vector>
#include "language/syntax_tree.h"
#include "util/assert.h"

namespace pp {
namespace language {

class ParsedFile {
 private:
	typedef std::map<string, syntax::DefinitionStatement*> SymbolMap;

 public:
	typedef SymbolMap::const_iterator SymbolIterator;

	explicit
	ParsedFile(const string &name)
	    : m_name(name)
	{
	}
	~ParsedFile()
	{
		for (size_t i = 0; i < m_statements.size(); i++) {
			delete m_statements[i];
		}
	}

	const string &
	name() const
	{
		return m_name;
	}

	size_t
	n_statements() const
	{
		return m_statements.size();
	}
	syntax::Statement *
	statement(size_t i) const
	{
		return m_statements[i];
	}
	void
	add_statement(syntax::Statement *statement)
	{
		m_statements.push_back(statement);
	}

	size_t
	n_symbols() const
	{
		return m_public_symbols.size() + m_private_symbols.size();
	}
	size_t
	n_public_symbols() const
	{
		return m_public_symbols.size();
	}
	size_t
	n_private_symbols() const
	{
		return m_private_symbols.size();
	}
	SymbolIterator
	public_symbols_begin() const
	{
		return m_public_symbols.begin();
	}
	SymbolIterator
	public_symbols_end() const
	{
		return m_public_symbols.end();
	}
	SymbolIterator
	private_symbols_begin() const
	{
		return m_private_symbols.begin();
	}
	SymbolIterator
	private_symbols_end() const
	{
		return m_private_symbols.end();
	}
	syntax::DefinitionStatement *
	public_symbol(const string &name) const
	{
		SymbolIterator it = m_public_symbols.find(name);
		if (it == m_public_symbols.end()) {
			return NULL;
		}
		return it->second;
	}
	syntax::DefinitionStatement *
	private_symbol(const string &name) const
	{
		SymbolIterator it = m_private_symbols.find(name);
		if (it == m_private_symbols.end()) {
			return NULL;
		}
		return it->second;
	}
	// Returns true if the symbol already existed.
	bool
	add_public_symbol(const string &name,
	                  syntax::DefinitionStatement *definition)
	{
		const syntax::DefinitionStatement *p = m_public_symbols[name];
		m_public_symbols[name] = definition;
		return p;
	}
	// Returns true if the symbol already existed.
	bool
	add_private_symbol(const string &name,
	                   syntax::DefinitionStatement *definition)
	{
		const syntax::DefinitionStatement *p = m_private_symbols[name];
		m_private_symbols[name] = definition;
		return p;
	}

 private:
	string m_name;
	std::vector<syntax::Statement*> m_statements;
	SymbolMap m_public_symbols;
	SymbolMap m_private_symbols;
};

}  // namespace language
}  // namespace pp

#endif  // PP_LANGUAGE_PARSED_FILE_H__

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
