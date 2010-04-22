//
// Copyright 2008 Tim Hockin.
//
#ifndef PP_RUNTIME_H__
#define PP_RUNTIME_H__

#include "pp/pp.h"
#include "pp/context.h"
#include <vector>

namespace pp {

// apps should call this to bootstrap themselves
ScopePtr
initialize_device_tree();

class Runtime {
 public:
	Runtime();
	virtual ~Runtime()
	{
	}

	// the global current context
	virtual ContextPtr
	current_context();

	// get a read-only copy of the current context
	virtual ContextPtr
	context_snapshot();

	// push a new context onto the stack
	virtual void
	context_push(const ContextPtr &new_ctxt);

	// restore the previous context
	virtual void
	context_pop();

 private:
	std::vector<ContextPtr> m_context_stack;
};

// FIXME: One of these should be passed around from very early.
Runtime *
global_runtime();

}  // namespace pp

#endif // PP_RUNTIME_H__
