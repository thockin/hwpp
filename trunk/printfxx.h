#ifndef PP_PRINTFXX_H__
#define PP_PRINTFXX_H__

#include <string>
#include <iostream>
#include <sstream>
#include "boost/format.hpp"
#include "boost/preprocessor.hpp"

// convert any ostreamable object to a string
template <typename T>
inline std::string
to_string(const T &val)
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

// enable simple string manipulations of boost::format
inline std::string &
operator+=(std::string &str, const boost::format &fmt)
{
	// return the original lhs
	str += to_string(fmt);
	return str;
}
inline std::string
operator+(const std::string &str, const boost::format &fmt)
{
	// return a new string
	return std::string(str + to_string(fmt));
}
inline std::string
operator+(const boost::format &fmt, const std::string &str)
{
	// return a new string
	return std::string(to_string(fmt) + str);
}
inline bool
operator==(const std::string &str, const boost::format &fmt)
{
	return (str == to_string(fmt));
}
inline bool
operator==(const boost::format &fmt, const std::string &str)
{
	return (str == fmt);
}
inline bool
operator!=(const std::string &str, const boost::format &fmt)
{
	return !(str == fmt);
}
inline bool
operator!=(const boost::format &fmt, const std::string &str)
{
	return !(str == fmt);
}

// A lot of this is boost magic to generate a load of template
// definitions.  Not pretty, but useful.

#define GEN_ARG(Z, N, _) % BOOST_PP_CAT(a, N)

#define BOOST_PP_LOCAL_MACRO(N) \
template <BOOST_PP_ENUM_PARAMS(N, class T)> \
void \
printfxx(const std::string& fmt, \
         BOOST_PP_ENUM_BINARY_PARAMS(N, const T, &a)) { \
	std::cout << boost::format(fmt) BOOST_PP_REPEAT(N, GEN_ARG, _); \
} \
template <BOOST_PP_ENUM_PARAMS(N, class T)> \
void \
fprintfxx(std::ostream &out, const std::string& fmt, \
         BOOST_PP_ENUM_BINARY_PARAMS(N, const T, &a)) { \
	out << boost::format(fmt) BOOST_PP_REPEAT(N, GEN_ARG, _); \
} \
template <BOOST_PP_ENUM_PARAMS(N, class T)> \
std::string \
sprintfxx(const std::string& fmt, \
         BOOST_PP_ENUM_BINARY_PARAMS(N, const T, &a)) { \
	return to_string(boost::format(fmt) BOOST_PP_REPEAT(N, GEN_ARG, _)); \
}

// allow up to 20 args
#define BOOST_PP_LOCAL_LIMITS  (1, 16)
#include BOOST_PP_LOCAL_ITERATE()
#undef GEN_ARG

// no-args version is easier to write by hand instead of messing with
// BOOST_PP_EXPR_IF
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
	return to_string(boost::format(fmt));
}

#endif // PP_PRINTFXX_H__
