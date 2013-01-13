#include "hwpp.h"
#include "version.h"
#include "util/log.h"

namespace hwpp {

void
hwpp_init(bool enable_logging, uint8_t log_verbosity)
{
	if (enable_logging) {
		log::enable_logging(log_verbosity);
	}
	LOG(INFO, "libhwpp version: %s", version_string);
}

}  // namespace hwpp
