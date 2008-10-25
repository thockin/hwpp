//
// Copyright 2008 Tim Hockin.
//
#ifndef PP_RUNTIME_H__
#define PP_RUNTIME_H__

#include "pp.h"
#include "pp_context.h"
#include <vector>

extern pp_scope_ptr
pp_init();

// the global current context
extern pp_context_ptr
pp_current_context();

// get a read-only copy of the current context
extern pp_context_ptr
pp_context_snapshot();

// push a new context onto the stack
extern void
pp_context_push(const pp_context_ptr &new_ctxt);

// restore the previous context
extern void
pp_context_pop();

#endif // PP_RUNTIME_H__
