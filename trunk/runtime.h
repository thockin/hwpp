//
// Copyright 2008 Tim Hockin.
//
#ifndef PP_RUNTIME_H__
#define PP_RUNTIME_H__

#include "pp/pp.h"
#include "pp/context.h"
#include <vector>

namespace pp {

class Runtime {
 public:
	Runtime();
	~Runtime()
	{
	}

	// the global current context
	ContextPtr
	current_context() const;

	// get a read-only copy of the current context
	ContextPtr
	context_snapshot() const;

	// push a new context onto the stack
	void
	context_push(const ContextPtr &new_ctxt);

	// restore the previous context
	void
	context_pop();

 private:
	ScopePtr m_root_scope;
	std::vector<ContextPtr> m_context_stack;
};

// FIXME: One of these should be passed around from very early.
Runtime *
global_runtime();

}  // namespace pp

#endif // PP_RUNTIME_H__
