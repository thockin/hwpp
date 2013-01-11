/* Copyright (c) Tim Hockin, 2009 */
#ifndef HWPP_VERSION_H__
#define HWPP_VERSION_H__

#include <stdint.h>

namespace hwpp {

//
// HWPP version constants - these need to be extern so they are always linked.
//
extern const uint8_t  ver_major;
extern const uint8_t  ver_minor;
extern const uint16_t ver_micro;
extern const uint32_t version;
extern const char *version_string;

inline uint32_t
MAKE_VERSION(uint8_t major, uint8_t minor, uint16_t micro)
{
	return ((major << 24) | (minor << 16) | (micro));
}

}  // namespace hwpp

#endif // HWPP_VERSION_H__
