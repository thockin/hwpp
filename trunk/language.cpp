/* Copyright (c) Tim Hockin, 2008 */
#include "pp.h"
#include "language.h"
#include "simple_regex.h"

static const regex c_token("^[A-Za-z_][A-Za-z0-9_]*$");

bool
lang_valid_datatype_name(const string &name)
{
	return c_token.matches(name);
}

bool
lang_valid_bookmark_name(const string &name)
{
	return c_token.matches(name);
}
