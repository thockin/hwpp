#ifndef PP_LANGUAGE_PARSED_FILE_H__
#define PP_LANGUAGE_PARSED_FILE_H__

// The grammar.l and grammar.y files include this header, which will screw
// them up if they include their own auto-generated headers, so don't include
// auto.*.h here.
#include "pp.h"
#include <string>
#include <vector>
#include "language/syntax_tree.h"
#include "util/assert.h"

namespace pp {
namespace language {

class ParsedFile {
 public:
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
	const syntax::Statement *
	statement(size_t i) const
	{
		return m_statements[i];
	}
	void
	add_statement(const syntax::Statement *statement)
	{
		m_statements.push_back(statement);
	}

	enum SymbolScope {
		SCOPE_PUBLIC = 1,
		SCOPE_PRIVATE = 2,
	};

	size_t
	n_symbols() const
	{
		return m_public_symbols.size() + m_private_symbols.size();
	}
	size_t
	n_symbols(SymbolScope scope) const
	{
		check_symbol_scope(scope);
		if (scope == SCOPE_PUBLIC) {
			return m_public_symbols.size();
		}
		return m_private_symbols.size();
	}
	const syntax::Identifier *
	symbol(SymbolScope scope, size_t i) const
	{
		check_symbol_scope(scope);
		if (scope == SCOPE_PUBLIC) {
			return m_public_symbols[i];
		}
		return m_private_symbols[i];
	}
	void
	add_symbol(SymbolScope scope, const syntax::Identifier *identifier)
	{
		check_symbol_scope(scope);
		if (scope == SCOPE_PUBLIC) {
			m_public_symbols.push_back(identifier);
			return;
		}
		m_private_symbols.push_back(identifier);
	}

 private:
	void
	check_symbol_scope(SymbolScope scope) const
	{
		switch(scope) {
		 case SCOPE_PUBLIC:
		 case SCOPE_PRIVATE:
			return;
		 default:
			ASSERT_MSG(false,
			           sprintfxx("unknown SymbolScope: %d", scope));
		}
	}

	string m_name;
	std::vector<const syntax::Statement*> m_statements;
	std::vector<const syntax::Identifier*> m_public_symbols;
	std::vector<const syntax::Identifier*> m_private_symbols;
};

}  // namespace language
}  // namespace pp

#endif  // PP_LANGUAGE_PARSED_FILE_H__
