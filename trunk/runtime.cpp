/*
 * Copyright 2008 Google Inc. All Rights Reserved.
 */

#include "pp.h"
#include "runtime.h"
#include "pp_context.h"
#include "device_init.h"

//
// These comprise the current context within the PP tree.  The name
// of the root doesn't matter, it just has to be *something* valid.
//
pp_context current_context("pp", new_pp_scope());
static std::vector<pp_context> context_stack;

// Initialize the PP runtime.
pp_scope_ptr
pp_init()
{
	// platform is the top level, cur_scope must not exist
	DASSERT_MSG(context_stack.empty(),
			"context_stack must be empty");

	// FIXME: take these out when we have a real language
	global_datatypes_init();
	pci_datatypes_init();
	cpuid_datatypes_init();
	msr_datatypes_init();

	return current_context.scope();
}

// get a read-only copy of the current context
pp_context
pp_context_snapshot()
{
	return current_context.snapshot();
}

// push a new context onto the stack
void
pp_context_push(const pp_context &new_ctxt)
{
	DTRACE(TRACE_SCOPES, "setting context: " + new_ctxt.name());
	context_stack.push_back(current_context);
	current_context = new_ctxt;
}

// restore the previous context
void
pp_context_pop()
{
	// there had better be a parent context
	DASSERT_MSG(!context_stack.empty(), "invalid parent context");

	DTRACE(TRACE_SCOPES, "restoring context: " + context_stack.back().name());
	current_context = context_stack.back();
	context_stack.pop_back();
}
