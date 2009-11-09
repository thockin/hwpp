/* Copyright (c) Tim Hockin, 2008 */
#include "pp/pp.h"
#include "language.h"
#include "pp/util/simple_regex.h"

namespace pp {

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

}  // namespace pp
