// Copyright (c) Tim Hockin, 2009
#ifndef HWPP_VERSION_H__
#define HWPP_VERSION_H__

#include <stdint.h>

namespace hwpp {

// HWPP version constants - these need to be extern so they are always linked.
//
extern const uint16_t ver_major;
extern const uint16_t ver_minor;
extern const uint32_t version;
extern const char *version_string;

}  // namespace hwpp

#endif // HWPP_VERSION_H__
