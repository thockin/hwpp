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

namespace runtime {

extern ScopePtr
init();

// the global current context
extern ContextPtr
current_context();

// get a read-only copy of the current context
extern ContextPtr
context_snapshot();

// push a new context onto the stack
extern void
context_push(const ContextPtr &new_ctxt);

// restore the previous context
extern void
context_pop();

}  // namespace runtime
}  // namespace pp

#endif // PP_RUNTIME_H__
