#include <stdint.h>
#include "version.h"

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x

namespace pp {

// version constants
const uint8_t  ver_major = PRJ_VER_MAJOR;
const uint8_t  ver_minor = PRJ_VER_MINOR;
const uint16_t ver_micro = PRJ_VER_MICRO;
const uint32_t version
		= MAKE_VERSION(PRJ_VER_MAJOR, PRJ_VER_MINOR, PRJ_VER_MICRO);
const char *version_string = STRINGIFY(PRJ_VER_MAJOR) "."
                             STRINGIFY(PRJ_VER_MINOR) "."
		             STRINGIFY(PRJ_VER_MICRO);
#undef STRINGIFY
#undef STRINGIFY2

}  // namespace pp
