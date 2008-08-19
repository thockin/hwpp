//
// Tim Hockin <thockin@hockin.org>
//
#ifndef SYSERROR_HPP__
#define SYSERROR_HPP__

#include <stdexcept>
#include <string>
#include <cerrno>
#include <cstring>

namespace syserr {

// a base class for errno-related errors
class errno_error: public std::runtime_error
{
    private:
	int m_error;

    protected:
	errno_error(int error, const std::string &msg)
	: runtime_error(msg), m_error(error)
	{
	}

	int
	error()
	{
		return m_error;
	}
};

#define DEFINE_ERROR(name) \
	struct name: public errno_error \
	{ \
		name(int error, const std::string &str) \
		: errno_error(error, str) \
		{} \
	};

// define a class for each errno
DEFINE_ERROR(permission_denied);
DEFINE_ERROR(operation_not_permitted);
DEFINE_ERROR(not_found);
DEFINE_ERROR(io_error);
DEFINE_ERROR(invalid);
DEFINE_ERROR(no_device);
DEFINE_ERROR(busy);
DEFINE_ERROR(exists);
DEFINE_ERROR(not_a_directory);
DEFINE_ERROR(is_a_directory);
DEFINE_ERROR(no_space);
DEFINE_ERROR(illegal_seek);
DEFINE_ERROR(broken_pipe);
DEFINE_ERROR(too_many_symlinks);
DEFINE_ERROR(name_too_long);
DEFINE_ERROR(out_of_memory);
DEFINE_ERROR(not_implemented);
DEFINE_ERROR(bad_address);
DEFINE_ERROR(bad_file_descriptor);
DEFINE_ERROR(out_of_range);
DEFINE_ERROR(try_again);
DEFINE_ERROR(no_child);
DEFINE_ERROR(text_busy);
DEFINE_ERROR(interrupted);
DEFINE_ERROR(operation_in_progress);
DEFINE_ERROR(not_supported);
DEFINE_ERROR(operation_timed_out);
DEFINE_ERROR(connection_in_progress);
DEFINE_ERROR(no_buffer_space);
DEFINE_ERROR(protocol_not_supported);
DEFINE_ERROR(protocol_not_available);
DEFINE_ERROR(protocol_error);
DEFINE_ERROR(address_family_not_supported);
DEFINE_ERROR(address_in_use);
DEFINE_ERROR(connection_refused);
DEFINE_ERROR(connection_aborted);
DEFINE_ERROR(socket_is_connected);
DEFINE_ERROR(socket_not_connected);
DEFINE_ERROR(not_a_socket);
DEFINE_ERROR(host_unreachable);
DEFINE_ERROR(net_unreachable);
DEFINE_ERROR(unknown_error);

inline std::runtime_error
throw_specific_errno_error(int error, const std::string &msg)
{
	switch (error) {
	    case EACCES:
		throw permission_denied(error, msg);
	    case EPERM:
		throw operation_not_permitted(error, msg);
	    case ENOENT:
		throw not_found(error, msg);
	    case EIO:
		throw io_error(error, msg);
	    case EINVAL:
		throw invalid(error, msg);
	    case ENXIO:
	    case ENODEV:
		throw no_device(error, msg);
	    case EBUSY:
		throw busy(error, msg);
	    case EEXIST:
		throw exists(error, msg);
	    case ENOTDIR:
		throw not_a_directory(error, msg);
	    case EISDIR:
		throw is_a_directory(error, msg);
	    case ENOSPC:
		throw no_space(error, msg);
	    case ESPIPE:
		throw illegal_seek(error, msg);
	    case EPIPE:
		throw broken_pipe(error, msg);
	    case ELOOP:
		throw too_many_symlinks(error, msg);
	    case ENAMETOOLONG:
		throw name_too_long(error, msg);
	    case ENOMEM:
		throw out_of_memory(error, msg);
	    case ENOSYS:
		throw not_implemented(error, msg);
	    case EFAULT:
		throw bad_address(error, msg);
	    case EBADF:
		throw bad_file_descriptor(error, msg);
	    case ERANGE:
		throw out_of_range(error, msg);
	    case EAGAIN: // same as EWOULDBLOCK
		throw try_again(error, msg);
	    case ECHILD:
		throw no_child(error, msg);
	    case ETXTBSY:
		throw text_busy(error, msg);
	    case EINTR:
		throw interrupted(error, msg);
	    case EINPROGRESS:
		throw operation_in_progress(error, msg);
	    case ENOTSUP: // same as EOPNOTSUPP
		throw not_supported(error, msg);
	    case ETIMEDOUT:
		throw operation_timed_out(error, msg);
	    case EALREADY:
		throw connection_in_progress(error, msg);
	    case ENOBUFS:
		throw no_buffer_space(error, msg);
	    case EPROTONOSUPPORT:
		throw protocol_not_supported(error, msg);
	    case ENOPROTOOPT:
		throw protocol_not_available(error, msg);
	    case EPROTO:
		throw protocol_error(error, msg);
	    case EAFNOSUPPORT:
		throw address_family_not_supported(error, msg);
	    case EADDRINUSE:
		throw address_in_use(error, msg);
	    case ECONNREFUSED:
		throw connection_refused(error, msg);
	    case ECONNABORTED:
		throw connection_aborted(error, msg);
	    case EISCONN:
		throw socket_is_connected(error, msg);
	    case ENOTCONN:
		throw socket_not_connected(error, msg);
	    case ENOTSOCK:
		throw not_a_socket(error, msg);
	    case EHOSTUNREACH:
		throw host_unreachable(error, msg);
	    case ENETUNREACH:
		throw net_unreachable(error, msg);
	}
	// default
	throw unknown_error(error, msg);
}

// callers can simply say "syserr::throw_errno_error(errno);"
inline std::runtime_error
throw_errno_error(int error)
{
	std::string msg(::strerror(error));
	return throw_specific_errno_error(error, msg);
}

inline std::runtime_error
throw_errno_error(int error, std::string msg)
{
	msg += std::string(": ") + ::strerror(error);
	return throw_specific_errno_error(error, msg);
}

} // namespace syserr

#endif // SYSERROR_HPP__
