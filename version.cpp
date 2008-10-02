#include <stdint.h>
#include "pp.h"

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x

// version constants
const uint8_t  pp_ver_major = PRJ_VER_MAJOR;
const uint8_t  pp_ver_minor = PRJ_VER_MINOR;
const uint16_t pp_ver_micro = PRJ_VER_MICRO;
const uint32_t pp_version
		= PP_MKVERSION(PRJ_VER_MAJOR, PRJ_VER_MINOR, PRJ_VER_MICRO);
const char *pp_version_string
		= STRINGIFY(PRJ_VER_MAJOR) "."
		  STRINGIFY(PRJ_VER_MINOR) "."
		  STRINGIFY(PRJ_VER_MICRO);
