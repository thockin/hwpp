#ifndef PP_LANGUAGE_SYMBOL_TABLE_H__
#define PP_LANGUAGE_SYMBOL_TABLE_H__

#include <map>
#include <string>
#include <vector>
#include "variable.h" // FIXME: pp/language/variable

namespace pp {
namespace language {

class SymbolTable {
    public:
	SymbolTable()
	{
		// Create the top-most context.
		m_symbols.push_back(Context());
	}

	// Push a new context.
	void push_context()
	{
		m_symbols.push_back(Context());
	}

	// Pop the last context, always leaving 1.
	void pop_context()
	{
		if (m_symbols.size() > 1) {
			m_symbols.pop_back();
		}
	}

	// How many contexts deep are we?
	size_t context_depth()
	{
		return m_symbols.size();
	}

	// How many symbols in this context?
	size_t context_size()
	{
		return m_symbols.back().size();
	}

	// Add a symbol to the last context.  If the symbol already exists,
	// it is an error.
	// FIXME: Variable ownership?
	void add(const string &name, Variable *var)
	{
		Context::iterator it = m_symbols.back().find(name);
		if (it != m_symbols.back().end()) {
			//FIXME: error
			return;
		}
		// This avoids a default constructor call for Variable.
		m_symbols.back().insert(it, make_pair(name, var));
	}

	// Look up a symbol in all contexts.
	const Variable *lookup(const string &name) const
	{
		ContextVector::const_reverse_iterator sm;
		for (sm = m_symbols.rbegin(); sm != m_symbols.rend(); ++sm) {
			Context::const_iterator it = sm->find(name);
			if (it != sm->end()) {
				return (it->second);
			}
		}
		return NULL;
	}
	Variable *lookup(const string &name) {
		const SymbolTable *p = const_cast<const SymbolTable *>(this);
		return const_cast<Variable*>(p->lookup(name));
	}

	// Take a snapshot of this symbol table.
	SymbolTable snapshot() const
	{
		return SymbolTable(*this);
	}

    private:
	typedef std::map<string, Variable*> Context;
	typedef std::vector<Context> ContextVector;
	ContextVector m_symbols;
};

}  // namespace language
}  // namespace pp

#endif  // PP_LANGUAGE_SYMBOL_TABLE_H__
