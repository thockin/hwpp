/*
 * Copyright 2008 Google Inc. All Rights Reserved.
 */

#include "pp/pp.h"
#include "runtime.h"
#include "pp/context.h"
#include "device_init.h"

namespace pp {

ScopePtr
initialize_device_tree() {
	return global_runtime()->current_context()->scope();
}

Runtime *
global_runtime()
{
	static Runtime the_runtime;
	return &the_runtime;
}

// Initialize the PP runtime.
Runtime::Runtime()
{
	// The name of the root scope doesn't matter.
	context_push(new_pp_context("pp", new_pp_scope()));

	// FIXME: take these out when we have a real language
	pp::device::global_datatypes_init();
	pp::device::pci_datatypes_init();
	pp::device::cpuid_datatypes_init();
	pp::device::msr_datatypes_init();
}

ContextPtr
Runtime::current_context()
{
	DASSERT(m_context_stack.size() > 0);
	return m_context_stack.back();
 }

// get a read-only copy of the current context
ContextPtr
Runtime::context_snapshot()
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

}  // namespace pp
