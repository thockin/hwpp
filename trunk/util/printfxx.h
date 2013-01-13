#ifndef HWPP_UTIL_PRINTFXX_H__
#define HWPP_UTIL_PRINTFXX_H__

#include <string>
#include <iostream>
#include <sstream>
#include <boost/format.hpp>
#include <boost/preprocessor.hpp>
#include "strings.h"

// Note: this is in the top-level namespace.
namespace printfxx {

// A lot of this is boost magic to generate a load of template
// definitions.  Not pretty, but useful.

#define GEN_ARG(Z, N, _) % BOOST_PP_CAT(a, N)

#define BOOST_PP_LOCAL_MACRO(N) \
template <BOOST_PP_ENUM_PARAMS(N, class T)> \
void \
printfxx(const std::string& fmt, \
         BOOST_PP_ENUM_BINARY_PARAMS(N, const T, &a)) \
{ \
	std::cout << boost::format(fmt) BOOST_PP_REPEAT(N, GEN_ARG, _); \
} \
template <BOOST_PP_ENUM_PARAMS(N, class T)> \
void \
fprintfxx(std::ostream &out, const std::string& fmt, \
          BOOST_PP_ENUM_BINARY_PARAMS(N, const T, &a)) \
{ \
	out << boost::format(fmt) BOOST_PP_REPEAT(N, GEN_ARG, _); \
} \
template <BOOST_PP_ENUM_PARAMS(N, class T)> \
std::string \
sprintfxx(const std::string& fmt, \
          BOOST_PP_ENUM_BINARY_PARAMS(N, const T, &a)) \
{ \
	return ::strings::to_string(boost::format(fmt) \
	                            BOOST_PP_REPEAT(N, GEN_ARG, _)); \
}

// allow up to 16 args
#define BOOST_PP_LOCAL_LIMITS  (1, 16)
#include BOOST_PP_LOCAL_ITERATE()
#undef GEN_ARG

// The no-args version is easier to write by hand instead of messing with
// BOOST_PP_EXPR_IF.
inline void
printfxx(const std::string &fmt)
{
	std::cout << boost::format(fmt);
}
inline void
fprintfxx(std::ostream &out, const std::string &fmt)
{
	out << boost::format(fmt);
}
inline std::string
sprintfxx(const std::string &fmt)
{
	return ::strings::to_string(boost::format(fmt));
}

} // namespace printfxx

#endif // HWPP_UTIL_PRINTFXX_H__
