#include "log.h"
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <string>

namespace log {

// Global control of logging.
static bool logging_is_enabled_flag = false;
bool logging_is_enabled() {
	return logging_is_enabled_flag;
}

static bool logging_verbosity_flag = false;
uint8_t
logging_verbosity()
{
	return logging_verbosity_flag;
}

void
enable_logging(uint8_t verbosity)
{
	logging_is_enabled_flag = true;
	logging_verbosity_flag = verbosity;
}

namespace internal {

static inline void
log_msg(char prefix, const char *file, int line, const char *func,
        const std::string &msg)
{
	std::cerr
	  << prefix << "[" << file << ":" << line << "]"
	  << " " << func << "(): "
	  << msg
	  << std::endl;
}

void
INFO(const char *file, int line, const char *func, const std::string &msg)
{
	log_msg('I', file, line, func, msg);
}

void
WARN(const char *file, int line, const char *func, const std::string &msg)
{
	log_msg('W', file, line, func, msg);
}

void
ERROR(const char *file, int line, const char *func, const std::string &msg)
{
	log_msg('E', file, line, func, msg);
}

}  // namespace internal

}  // namespace log
