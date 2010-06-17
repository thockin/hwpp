#ifndef PP_UTIL_SYMBOL_TABLE_H__
#define PP_UTIL_SYMBOL_TABLE_H__

#include <map>
#include <vector>
#include "util/assert.h"

namespace util {

// This class provides a stack of maps, which behave like a symbol table in a
// language like C.  Each new context can have keys that override keys in
// prior contexts.
template<typename Tkey, typename Tvalue>
class SymbolTable {
 public:
	// Ths type represents a single layer of the stack.
	typedef std::map<Tkey, Tvalue> Context;

	SymbolTable()
	{
		// Create the root context.
		m_stack.push_back(Context());
	}

	// Push a new context.
	void
	push_context()
	{
		m_stack.push_back(Context());
	}

	// Pop the last context, always leaving 1.  Returns false when the
	// stack is at the root.
	bool
	pop_context()
	{
		if (m_stack.size() > 1) {
			m_stack.pop_back();
			return true;
		}
		return false;
	}

	// How many contexts deep are we?
	size_t
	depth() const
	{
		return m_stack.size();
	}

	// How many keys in the current context?
	size_t
	context_size() const
	{
		return current_context()->size();
	}

	// Add a value to the current context.  If the key already exists,
	// replace the value.
	void
	add(const Tkey &key, const Tvalue &val)
	{
		typename Context::iterator it = current_context()->find(key);
		if (it != current_context()->end()) {
			it->second = val;
		} else {
			// This avoids a default constructor call for Tvalue.
			current_context()->insert(it, make_pair(key, val));
		}
	}

	// Add a value to the current context.  If the key already exists,
	// return false.
	bool
	add_unique(const Tkey &key, const Tvalue &val)
	{
		typename Context::iterator it = current_context()->find(key);
		if (it != current_context()->end()) {
			return false;
		}
		// This avoids a default constructor call for Tvalue.
		current_context()->insert(it, make_pair(key, val));
		return true;
	}

	// Look up a key in all contexts, backwards from the current. Returns
	// NULL if not found.
	const Tvalue *
	lookup(const Tkey &key) const
	{
		typename ContextStack::const_reverse_iterator ctxt;
		for (ctxt = m_stack.rbegin(); ctxt != m_stack.rend(); ++ctxt) {
			typename Context::const_iterator it = ctxt->find(key);
			if (it != ctxt->end()) {
				return &it->second;
			}
		}
		return NULL;
	}

	// Take a snapshot of this symbol table.
	SymbolTable
	snapshot() const
	{
		return SymbolTable(*this);
	}

 private:
	Context *
	current_context()
	{
		// This is safe because we never let the stack get empty.
		DASSERT(m_stack.size() > 0);
		return &m_stack.back();
	}
	const Context *
	current_context() const
	{
		// This is safe because we never let the stack get empty.
		DASSERT(m_stack.size() > 0);
		return &m_stack.back();
	}

	typedef std::vector<Context> ContextStack;
	ContextStack m_stack;
};

}  // namespace util

#endif  // PP_UTIL_SYMBOL_TABLE_H__
