// Some string-enabled wrappers for C calls like system().
// Tim Hockin

#ifndef HWPP_UTIL_EXECXX_H__
#define HWPP_UTIL_EXECXX_H__

#include <stdlib.h>
#include <string>

// Note: this is in the top-level namespace.
namespace execxx {

inline int
systemxx(const std::string &cmd)
{
	return system(cmd.c_str());
}

} // namespace execxx

#endif // HWPP_UTIL_EXECXX_H__
