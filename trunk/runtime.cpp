/*
 * Copyright 2008 Google Inc. All Rights Reserved.
 */

#include "hwpp.h"
#include "runtime.h"
#include "context.h"

namespace hwpp {

Runtime *
global_runtime()
{
	static Runtime the_runtime;
	return &the_runtime;
}

// Initialize the HWPP runtime.
Runtime::Runtime()
{
	// The name of the root scope doesn't matter, we just need to retain
	// a pointer to it.
	m_root_scope = new_hwpp_scope();
	context_push(new_hwpp_context("hwpp", m_root_scope));
}

ContextPtr
Runtime::current_context() const
{
	DASSERT(m_context_stack.size() > 0);
	return m_context_stack.back();
 }

// get a read-only copy of the current context
ContextPtr
Runtime::context_snapshot() const
{
	return current_context()->snapshot();
}

// push a new context onto the stack
void
Runtime::context_push(const ContextPtr &new_ctxt)
{
	DASSERT(new_ctxt);
	DTRACE(TRACE_SCOPES, "setting context: " + new_ctxt->name());
	m_context_stack.push_back(new_ctxt);
}

// restore the previous context
void
Runtime::context_pop()
{
	DASSERT(m_context_stack.size() > 1);
	DTRACE(TRACE_SCOPES,
	       "restoring context: " + m_context_stack.back()->name());
	m_context_stack.pop_back();
}

}  // namespace hwpp
