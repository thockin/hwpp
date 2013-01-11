#ifndef HWPP_LANGUAGE_PARSED_FILE_H__
#define HWPP_LANGUAGE_PARSED_FILE_H__

// The grammar.l and grammar.y files include this header, which will screw
// them up if they include their own auto-generated headers, so don't include
// auto.*.h here.
#include "hwpp.h"
#include <map>
#include <string>
#include <vector>
#include "language/syntax_tree.h"
#include "util/assert.h"

namespace hwpp {
namespace language {

class ParsedFile {
 public:
	//FIXME: move this out?
	struct Symbol {
		Symbol(const string &mod, const string &sym)
		    : module(mod), symbol(sym)
		{
		}
		bool
		operator<(const Symbol &that) const
		{
			if (module < that.module) {
				return true;
			}
			if (module == that.module) {
				if (symbol < that.symbol) {
					return true;
				}
			}
			return false;
		}
		string module;
		string symbol;
	};
	typedef std::map<Symbol, syntax::Definition*> SymbolMap;
	typedef SymbolMap::const_iterator SymbolIterator;

	ParsedFile()
	{
	}
	~ParsedFile()
	{
		for (size_t i = 0; i < m_statements.size(); i++) {
			delete m_statements[i];
		}
	}

	const string &
	module() const
	{
		return m_module;
	}
	void
	set_module(const string &name)
	{
		m_module = name;
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
	#if 0 // FIXME: handle symbols, handle "same as mine" module semantic
	syntax::Definition *
	public_symbol(const string &name) const
	{
		SymbolIterator it = m_public_symbols.find(name);
		if (it == m_public_symbols.end()) {
			return NULL;
		}
		return it->second;
	}
	syntax::Definition *
	private_symbol(const string &name) const
	{
		SymbolIterator it = m_private_symbols.find(name);
		if (it == m_private_symbols.end()) {
			return NULL;
		}
		return it->second;
	}
	#endif
	// Returns true if the symbol already existed.
	bool
	add_public_symbol(const string &name, syntax::Definition *definition)
	{
		Symbol sym(m_module, name);
		const syntax::Definition *def = m_public_symbols[sym];
		m_public_symbols[sym] = definition;
		return def;
	}
	// Returns true if the symbol already existed.
	bool
	add_private_symbol(const string &name, syntax::Definition *definition)
	{
		Symbol sym(m_module, name);
		const syntax::Definition *def = m_private_symbols[sym];
		m_private_symbols[sym] = definition;
		return def;
	}

 private:
	string m_module;
	std::vector<syntax::Statement*> m_statements;
	SymbolMap m_public_symbols;
	SymbolMap m_private_symbols;
};

}  // namespace language
}  // namespace hwpp

#endif  // HWPP_LANGUAGE_PARSED_FILE_H__

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
