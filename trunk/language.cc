/* Copyright (c) Tim Hockin, 2008 */
#include "hwpp.h"
#include "language.h"
#include "util/simple_regex.h"

namespace hwpp {

static const util::SimpleRegex c_token("^[A-Za-z_][A-Za-z0-9_]*$");

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

}  // namespace hwpp
