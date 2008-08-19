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

// we could make all these classes inherit from a base class, if we needed
#define DEFINE_ERROR(name) \
	struct name: public std::runtime_error \
	{ \
		explicit name(const std::string &str): runtime_error(str) \
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

// callers can simply "throw syserr::errno_error(errno);"
class errno_error
{
    public:
	// throw a specific error
	errno_error(int error)
	{
		std::string msg(::strerror(error));
		throw_real_error(error, msg);
	}
	errno_error(int error, std::string msg)
	{
		msg += std::string(": ") + ::strerror(error);
		throw_real_error(error, msg);
	}

    private:
	void
	throw_real_error(int error, const std::string &msg)
	{
		switch (error) {
		    case EACCES:
			throw permission_denied(msg);
		    case EPERM:
			throw operation_not_permitted(msg);
		    case ENOENT:
			throw not_found(msg);
		    case EIO:
			throw io_error(msg);
		    case EINVAL:
			throw invalid(msg);
		    case ENXIO:
		    case ENODEV:
			throw no_device(msg);
		    case EBUSY:
			throw busy(msg);
		    case EEXIST:
			throw exists(msg);
		    case ENOTDIR:
			throw not_a_directory(msg);
		    case EISDIR:
			throw is_a_directory(msg);
		    case ENOSPC:
			throw no_space(msg);
		    case ESPIPE:
			throw illegal_seek(msg);
		    case EPIPE:
			throw broken_pipe(msg);
		    case ELOOP:
			throw too_many_symlinks(msg);
		    case ENAMETOOLONG:
			throw name_too_long(msg);
		    case ENOMEM:
			throw out_of_memory(msg);
		    case ENOSYS:
			throw not_implemented(msg);
		    case EFAULT:
			throw bad_address(msg);
		    case EBADF:
			throw bad_file_descriptor(msg);
		    case ERANGE:
			throw out_of_range(msg);
		    case EAGAIN: // same as EWOULDBLOCK
			throw try_again(msg);
		    case ECHILD:
			throw no_child(msg);
		    case ETXTBSY:
			throw text_busy(msg);
		    case EINTR:
			throw interrupted(msg);
		    case EINPROGRESS:
			throw operation_in_progress(msg);
		    case ENOTSUP: // same as EOPNOTSUPP
			throw not_supported(msg);
		    case ETIMEDOUT:
			throw operation_timed_out(msg);
		    case EALREADY:
			throw connection_in_progress(msg);
		    case ENOBUFS:
			throw no_buffer_space(msg);
		    case EPROTONOSUPPORT:
			throw protocol_not_supported(msg);
		    case ENOPROTOOPT:
			throw protocol_not_available(msg);
		    case EPROTO:
			throw protocol_error(msg);
		    case EAFNOSUPPORT:
			throw address_family_not_supported(msg);
		    case EADDRINUSE:
			throw address_in_use(msg);
		    case ECONNREFUSED:
			throw connection_refused(msg);
		    case ECONNABORTED:
			throw connection_aborted(msg);
		    case EISCONN:
			throw socket_is_connected(msg);
		    case ENOTCONN:
			throw socket_not_connected(msg);
		    case ENOTSOCK:
			throw not_a_socket(msg);
		    case EHOSTUNREACH:
			throw host_unreachable(msg);
		    case ENETUNREACH:
			throw net_unreachable(msg);
		    default:
			throw unknown_error(msg);
		}
	}
};

} // namespace syserr

#endif // SYSERROR_HPP__
