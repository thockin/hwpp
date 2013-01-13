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

} // namespace strings

#endif // HWPP_UTIL_STRINGS_H__
