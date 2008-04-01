//
// Copyright 2008 Tim Hockin.
//
#ifndef PP_RUNTIME_H__
#define PP_RUNTIME_H__

#include "pp.h"
#include "pp_context.h"
#include <vector>

extern pp_context current_context;
extern std::vector<pp_context> context_stack;

class pp_saved_context_impl;
typedef boost::shared_ptr<pp_saved_context_impl> pp_saved_context;

//
// Get/set the current active context.
//
extern pp_context
GET_CURRENT_CONTEXT();

extern pp_saved_context
SET_CURRENT_CONTEXT(const pp_context &new_context);

#endif // PP_RUNTIME_H__
