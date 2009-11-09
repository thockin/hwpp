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
	return runtime::init();
}
	
namespace runtime {

//
// These comprise the current context within the PP tree.  The name
// of the root doesn't matter, it just has to be *something* valid.
//
static std::vector<ContextPtr> context_stack;

// Initialize the PP runtime.
ScopePtr
init()
{
	// platform is the top level, cur_scope must not exist
	DASSERT_MSG(context_stack.empty(),
			"context_stack must be empty");

	ScopePtr root = new_pp_scope();
	context_push(new_pp_context("pp", root));

	// FIXME: take these out when we have a real language
	pp::device::global_datatypes_init();
	pp::device::pci_datatypes_init();
	pp::device::cpuid_datatypes_init();
	pp::device::msr_datatypes_init();

	return root;
}

ContextPtr
current_context()
{
	if (context_stack.empty()) {
		return ContextPtr();
	}
	return context_stack.back();
 }

// get a read-only copy of the current context
ContextPtr
context_snapshot()
{
	return current_context()->snapshot();
}

// push a new context onto the stack
void
context_push(const ContextPtr &new_ctxt)
{
	DTRACE(TRACE_SCOPES, "setting context: " + new_ctxt->name());
	context_stack.push_back(new_ctxt);
}

// restore the previous context
void
context_pop()
{
	// there had better be a parent context
	DASSERT_MSG(!context_stack.empty(), "invalid parent context");

	DTRACE(TRACE_SCOPES,
	       "restoring context: " + context_stack.back()->name());
	context_stack.pop_back();
}

}  // namespace runtime
}  // namespace pp
