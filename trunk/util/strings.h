// Simple string utils.
// Tim Hockin, 2013

#ifndef HWPP_UTIL_STRINGS_H__
#define HWPP_UTIL_STRINGS_H__

#include <string>
#include <sstream>

namespace strings {

// Convert any ostreamable object to a string.
template <typename T>
inline std::string
to_string(const T &val)
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

// Catenate strings (or char pointers, converted to strings) into a big
// string.  This could be more efficient.
inline std::string
string_cat(std::string arg1,
           const std::string &arg2)
{
  arg1.append(arg2);
  return arg1;
}
inline std::string
string_cat(std::string arg1,
           const std::string &arg2,
           const std::string &arg3)
{
  arg1.append(arg2);
  arg1.append(arg3);
  return arg1;
}
inline std::string
string_cat(std::string arg1,
           const std::string &arg2,
           const std::string &arg3,
           const std::string &arg4)
{
  arg1.append(arg2);
  arg1.append(arg3);
  arg1.append(arg4);
  return arg1;
}
inline std::string
string_cat(std::string arg1,
           const std::string &arg2,
           const std::string &arg3,
           const std::string &arg4,
           const std::string &arg5)
{
  arg1.append(arg2);
  arg1.append(arg3);
  arg1.append(arg4);
  arg1.append(arg5);
  return arg1;
}
inline std::string
string_cat(std::string arg1,
           const std::string &arg2,
           const std::string &arg3,
           const std::string &arg4,
           const std::string &arg5,
           const std::string &arg6)
{
  arg1.append(arg2);
  arg1.append(arg3);
  arg1.append(arg4);
  arg1.append(arg5);
  arg1.append(arg6);
  return arg1;
}

} // namespace strings

#endif // HWPP_UTIL_STRINGS_H__
