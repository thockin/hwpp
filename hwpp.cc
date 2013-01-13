#include "hwpp.h"
#include "util/log.h"

namespace hwpp {

void
hwpp_init(const char *argv0, bool enable_logging, uint8_t log_verbosity)
{
	if (enable_logging) {
		log::enable_logging(log_verbosity);
	}
	LOG(INFO, "argv0: %s", argv0);
}

}  // namespace hwpp
