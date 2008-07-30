/*
 * Copyright 2008 Google Inc. All Rights Reserved.
 */

#include "pp.h"
#include "runtime.h"
#include "pp_context.h"
#include "device_init.h"

//
// These comprise the current context within the PP tree.  The name
// doesn't matter, it just has to be *something* valid.
//
pp_context current_context("pp", new_pp_scope());
std::vector<pp_context> context_stack;

// Initialize the PP runtime.
pp_scope_ptr
pp_init()
{
	// platform is the top level, cur_scope must not exist
	DASSERT_MSG(context_stack.empty(),
			"context_stack must be empty");

	// FIXME: take these out when we have a real language
	platform_global_init();
	pci_datatypes_init();
	cpuid_datatypes_init();
	msr_datatypes_init();

	return current_context.scope();
}

//
// This is a transaction-management helper.
//
class pp_saved_context_impl
{
    private:
	pp_context m_context;

    public:
	pp_saved_context_impl(const pp_context &context)
	    : m_context(context)
	{
	}
	~pp_saved_context_impl()
	{
		DTRACE(TRACE_SCOPES, "restoring scope: " + m_context.name());
		current_context = m_context;
	}
};

// get a read-only copy of the current context
pp_context
pp_get_current_context()
{
	return current_context.snapshot();
}

// when this saved_context expires, the context will be restored
pp_saved_context
pp_set_current_context(const pp_context &new_ctxt)
{
	DTRACE(TRACE_SCOPES, "setting scope: " + new_ctxt.name());
	pp_saved_context old_ctxt(new pp_saved_context_impl(current_context));
	current_context = new_ctxt;
	return old_ctxt;
}
