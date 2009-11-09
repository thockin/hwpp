//
// Tim Hockin <thockin@hockin.org>
//
#ifndef PP_UTIL_SYSERROR_HPP__
#define PP_UTIL_SYSERROR_HPP__

#include <stdexcept>
#include <string>
#include <cerrno>
#include <cstring>

namespace syserr {

// a base class for errno-related errors
class ErrnoError: public std::runtime_error
{
    private:
	int m_error;

    protected:
	ErrnoError(int error, const std::string &msg)
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
	struct name: public ErrnoError \
	{ \
		name(int error, const std::string &str) \
		: ErrnoError(error, str) \
		{} \
	};

// define a class for each errno
DEFINE_ERROR(PermissionDenied);
DEFINE_ERROR(OperationNotPermitted);
DEFINE_ERROR(NotFound);
DEFINE_ERROR(IoError);
DEFINE_ERROR(Invalid);
DEFINE_ERROR(NoDevice);
DEFINE_ERROR(Busy);
DEFINE_ERROR(Exists);
DEFINE_ERROR(NotADirectory);
DEFINE_ERROR(IsADirectory);
DEFINE_ERROR(NoSpace);
DEFINE_ERROR(IllegalSeek);
DEFINE_ERROR(BrokenPipe);
DEFINE_ERROR(TooManySymlinks);
DEFINE_ERROR(NameTooLong);
DEFINE_ERROR(OutOfMemory);
DEFINE_ERROR(NotImplemented);
DEFINE_ERROR(BadAddress);
DEFINE_ERROR(BadFileDescriptor);
DEFINE_ERROR(OutOfRange);
DEFINE_ERROR(TryAgain);
DEFINE_ERROR(NoChild);
DEFINE_ERROR(TextBusy);
DEFINE_ERROR(Interrupted);
DEFINE_ERROR(OperationInProgress);
DEFINE_ERROR(NotSupported);
DEFINE_ERROR(OperationTimedOut);
DEFINE_ERROR(ConnectionInProgress);
DEFINE_ERROR(NoBufferSpace);
DEFINE_ERROR(ProtocolNotSupported);
DEFINE_ERROR(ProtocolNotAvailable);
DEFINE_ERROR(ProtocolError);
DEFINE_ERROR(AddressFamilyNotSupported);
DEFINE_ERROR(AddressInUse);
DEFINE_ERROR(ConnectionRefused);
DEFINE_ERROR(ConnectionAborted);
DEFINE_ERROR(SocketIsConnected);
DEFINE_ERROR(SocketNotConnected);
DEFINE_ERROR(NotASocket);
DEFINE_ERROR(HostUnreachable);
DEFINE_ERROR(NetUnreachable);
DEFINE_ERROR(UnknownError);

inline std::runtime_error
throw_specific_errno_error(int error, const std::string &msg)
{
	switch (error) {
	    case EACCES:
		throw PermissionDenied(error, msg);
	    case EPERM:
		throw OperationNotPermitted(error, msg);
	    case ENOENT:
		throw NotFound(error, msg);
	    case EIO:
		throw IoError(error, msg);
	    case EINVAL:
		throw Invalid(error, msg);
	    case ENXIO:
	    case ENODEV:
		throw NoDevice(error, msg);
	    case EBUSY:
		throw Busy(error, msg);
	    case EEXIST:
		throw Exists(error, msg);
	    case ENOTDIR:
		throw NotADirectory(error, msg);
	    case EISDIR:
		throw IsADirectory(error, msg);
	    case ENOSPC:
		throw NoSpace(error, msg);
	    case ESPIPE:
		throw IllegalSeek(error, msg);
	    case EPIPE:
		throw BrokenPipe(error, msg);
	    case ELOOP:
		throw TooManySymlinks(error, msg);
	    case ENAMETOOLONG:
		throw NameTooLong(error, msg);
	    case ENOMEM:
		throw OutOfMemory(error, msg);
	    case ENOSYS:
		throw NotImplemented(error, msg);
	    case EFAULT:
		throw BadAddress(error, msg);
	    case EBADF:
		throw BadFileDescriptor(error, msg);
	    case ERANGE:
		throw OutOfRange(error, msg);
	    case EAGAIN: // same as EWOULDBLOCK
		throw TryAgain(error, msg);
	    case ECHILD:
		throw NoChild(error, msg);
	    case ETXTBSY:
		throw TextBusy(error, msg);
	    case EINTR:
		throw Interrupted(error, msg);
	    case EINPROGRESS:
		throw OperationInProgress(error, msg);
	    case ENOTSUP: // same as EOPNOTSUPP
		throw NotSupported(error, msg);
	    case ETIMEDOUT:
		throw OperationTimedOut(error, msg);
	    case EALREADY:
		throw ConnectionInProgress(error, msg);
	    case ENOBUFS:
		throw NoBufferSpace(error, msg);
	    case EPROTONOSUPPORT:
		throw ProtocolNotSupported(error, msg);
	    case ENOPROTOOPT:
		throw ProtocolNotAvailable(error, msg);
	    case EPROTO:
		throw ProtocolError(error, msg);
	    case EAFNOSUPPORT:
		throw AddressFamilyNotSupported(error, msg);
	    case EADDRINUSE:
		throw AddressInUse(error, msg);
	    case ECONNREFUSED:
		throw ConnectionRefused(error, msg);
	    case ECONNABORTED:
		throw ConnectionAborted(error, msg);
	    case EISCONN:
		throw SocketIsConnected(error, msg);
	    case ENOTCONN:
		throw SocketNotConnected(error, msg);
	    case ENOTSOCK:
		throw NotASocket(error, msg);
	    case EHOSTUNREACH:
		throw HostUnreachable(error, msg);
	    case ENETUNREACH:
		throw NetUnreachable(error, msg);
	}
	// default
	throw UnknownError(error, msg);
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

#endif // PP_UTIL_SYSERROR_HPP__
