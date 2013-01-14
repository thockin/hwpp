// Very rudimentary logging that can be turned off entirely.
//
// This does not use Google's logging library because I want to beable to turn
// logging off entirely most of the time, and only enable it when explicitly
// requested.
//
// Copyright (c) Tim Hockin, 2013

#ifndef HWPP_UTIL_LOG_H__
#define HWPP_UTIL_LOG_H__

#include <stdint.h>
#include <iostream>
#include "util/printfxx.h"

namespace log {

// Enable logging withing HWPP code.
//
// verbosity: Sets the minimum vlevel for VLOG() calls to be logged.
void
enable_logging(uint8_t verbosity);

// Test whether logging is enabled.
bool
logging_is_enabled();

// Return the current logging verbosity level.
uint8_t
logging_verbosity();

namespace internal {

// Unconditionally log a formatted INFO message.
void
INFO(const char *file, int line, const char *func,
     const std::string &msg);

// Unconditionally log a formatted WARN message.
void
WARN(const char *file, int line, const char *func,
     const std::string &msg);

// Unconditionally log a formatted ERROR message.
void
ERROR(const char *file, int line, const char *func,
      const std::string &msg);

}  // namespace internal

}  // namespace log

//
// These are in the global namespace.
//

// Log a message if logging is enabled globally)
//
// level: One of INFO, WARN, ERROR.
// remainder: Printf-style args (see util/printfxx.h)
#define LOG(level, ...) do { \
	if (::log::logging_is_enabled()) { \
		::log::internal::level(__FILE__, __LINE__, __func__, \
		                       ::printfxx::sprintfxx(__VA_ARGS__)); \
	} \
} while (0)

// Log an INFO message if logging is enabled and verbosity is sufficient.
#define VLOG(vlevel, ...) do { \
	if (::log::logging_is_enabled() \
	 && ::log::logging_verbosity() >= vlevel) { \
		::log::internal::INFO(__FILE__, __LINE__, __func__, \
		                      ::printfxx::sprintfxx(__VA_ARGS__)); \
	} \
} while (0)

#if DEBUG
#  define DLOG(level, ...)   LOG(level, __VA_ARGS__)
#  define DVLOG(vlevel, ...) VLOG(vlevel, __VA_ARGS__)
#else // !DEBUG
#  define DLOG(level, ...)
#  define DVLOG(vlevel, ...)
#endif // DEBUG

#endif // HWPP_UTIL_LOG_H__

