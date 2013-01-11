//
// Copyright 2008 Tim Hockin.
//
#ifndef HWPP_RUNTIME_H__
#define HWPP_RUNTIME_H__

#include "hwpp.h"
#include "context.h"
#include <vector>

namespace hwpp {

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

}  // namespace hwpp

#endif // HWPP_RUNTIME_H__
