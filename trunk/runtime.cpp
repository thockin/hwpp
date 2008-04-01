/*
 * Copyright 2008 Google Inc. All Rights Reserved.
 */

#include "pp.h"
#include "runtime.h"
#include "pp_context.h"

#if 0
class stack<pp_context> context_stack;
drop is_valid() crap once pp_platform is the main entrypoint for apps
#endif

//
// These comprise the current context within the PP tree.
//
pp_context current_context;
std::vector<pp_context> context_stack;

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
GET_CURRENT_CONTEXT()
{
	return current_context.snapshot();
}

// when this saved_context expires, the context will be restored
pp_saved_context
SET_CURRENT_CONTEXT(const pp_context &new_ctxt)
{
	DTRACE(TRACE_SCOPES, "setting scope: " + new_ctxt.name());
	pp_saved_context old_ctxt(new pp_saved_context_impl(current_context));
	current_context = new_ctxt;
	return old_ctxt;
}
