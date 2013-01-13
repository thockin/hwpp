#include "version.h"
#include <stdint.h>

namespace hwpp {

static uint32_t
make_version(uint16_t major, uint16_t minor)
{
	return ((major << 16) | minor);
}

// Version constants.
const uint16_t ver_major = PRJ_VER_MAJOR;
const uint16_t ver_minor = PRJ_VER_MINOR;
const uint32_t version = make_version(PRJ_VER_MAJOR, PRJ_VER_MINOR);
#define MKSTR(x) MKSTR2(x)
#define MKSTR2(x) #x
const char *version_string = MKSTR(PRJ_VER_MAJOR) "." MKSTR(PRJ_VER_MINOR);
#undef MKSTR
#undef MKSTR2

}  // namespace hwpp
