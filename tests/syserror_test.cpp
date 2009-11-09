#include "pp/util/syserror.h"
#include "pp_test.h"

TEST(test_syserror)
{
	try {
		syserr::throw_errno_error(EACCES);
	} catch (syserr::PermissionDenied &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EACCES)");
	}
	try {
		syserr::throw_errno_error(EPERM);
	} catch (syserr::OperationNotPermitted &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EPERM)");
	}
	try {
		syserr::throw_errno_error(ENOENT);
	} catch (syserr::NotFound &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENOENT)");
	}
	try {
		syserr::throw_errno_error(EIO);
	} catch (syserr::IoError &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EIO)");
	}
	try {
		syserr::throw_errno_error(EINVAL);
	} catch (syserr::Invalid &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EINVAL)");
	}
	try {
		syserr::throw_errno_error(ENXIO);
	} catch (syserr::NoDevice &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENXIO)");
	}
	try {
		syserr::throw_errno_error(ENODEV);
	} catch (syserr::NoDevice &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENODEV)");
	}
	try {
		syserr::throw_errno_error(EBUSY);
	} catch (syserr::Busy &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EBUSY)");
	}
	try {
		syserr::throw_errno_error(EEXIST);
	} catch (syserr::Exists &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EEXIST)");
	}
	try {
		syserr::throw_errno_error(ENOTDIR);
	} catch (syserr::NotADirectory &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENOTDIR)");
	}
	try {
		syserr::throw_errno_error(EISDIR);
	} catch (syserr::IsADirectory &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EISDIR)");
	}
	try {
		syserr::throw_errno_error(ENOSPC);
	} catch (syserr::NoSpace &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENOSPC)");
	}
	try {
		syserr::throw_errno_error(ESPIPE);
	} catch (syserr::IllegalSeek &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ESPIPE)");
	}
	try {
		syserr::throw_errno_error(EPIPE);
	} catch (syserr::BrokenPipe &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EPIPE)");
	}
	try {
		syserr::throw_errno_error(ELOOP);
	} catch (syserr::TooManySymlinks &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ELOOP)");
	}
	try {
		syserr::throw_errno_error(ENAMETOOLONG);
	} catch (syserr::NameTooLong &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENAMETOOLONG)");
	}
	try {
		syserr::throw_errno_error(ENOMEM);
	} catch (syserr::OutOfMemory &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENOMEM)");
	}
	try {
		syserr::throw_errno_error(ENOSYS);
	} catch (syserr::NotImplemented &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENOSYS)");
	}
	try {
		syserr::throw_errno_error(EFAULT);
	} catch (syserr::BadAddress &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EFAULT)");
	}
	try {
		syserr::throw_errno_error(EBADF);
	} catch (syserr::BadFileDescriptor &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EBADF)");
	}
	try {
		syserr::throw_errno_error(ERANGE);
	} catch (syserr::OutOfRange &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ERANGE)");
	}
	try {
		syserr::throw_errno_error(EAGAIN);
	} catch (syserr::TryAgain &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EAGAIN)");
	}
	try {
		syserr::throw_errno_error(EWOULDBLOCK);
	} catch (syserr::TryAgain &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EWOULDBLOCK)");
	}
	try {
		syserr::throw_errno_error(ECHILD);
	} catch (syserr::NoChild &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ECHILD)");
	}
	try {
		syserr::throw_errno_error(ETXTBSY);
	} catch (syserr::TextBusy &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ETXTBSY)");
	}
	try {
		syserr::throw_errno_error(EINTR);
	} catch (syserr::Interrupted &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EINTR)");
	}
	try {
		syserr::throw_errno_error(EINPROGRESS);
	} catch (syserr::OperationInProgress &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EINPROGRESS)");
	}
	try {
		syserr::throw_errno_error(ENOTSUP);
	} catch (syserr::NotSupported &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENOTSUP)");
	}
	try {
		syserr::throw_errno_error(EOPNOTSUPP);
	} catch (syserr::NotSupported &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EOPNOTSUPP)");
	}
	try {
		syserr::throw_errno_error(ETIMEDOUT);
	} catch (syserr::OperationTimedOut &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ETIMEDOUT)");
	}
	try {
		syserr::throw_errno_error(EALREADY);
	} catch (syserr::ConnectionInProgress &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EALREADY)");
	}
	try {
		syserr::throw_errno_error(ENOBUFS);
	} catch (syserr::NoBufferSpace &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENOBUFS)");
	}
	try {
		syserr::throw_errno_error(EPROTONOSUPPORT);
	} catch (syserr::ProtocolNotSupported &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EPROTONOSUPPORT)");
	}
	try {
		syserr::throw_errno_error(ENOPROTOOPT);
	} catch (syserr::ProtocolNotAvailable &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENOPROTOOPT)");
	}
	try {
		syserr::throw_errno_error(EPROTO);
	} catch (syserr::ProtocolError &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EPROTO)");
	}
	try {
		syserr::throw_errno_error(EAFNOSUPPORT);
	} catch (syserr::AddressFamilyNotSupported &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EAFNOSUPPORT)");
	}
	try {
		syserr::throw_errno_error(EADDRINUSE);
	} catch (syserr::AddressInUse &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EADDRINUSE)");
	}
	try {
		syserr::throw_errno_error(ECONNREFUSED);
	} catch (syserr::ConnectionRefused &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ECONNREFUSED)");
	}
	try {
		syserr::throw_errno_error(ECONNABORTED);
	} catch (syserr::ConnectionAborted &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ECONNABORTED)");
	}
	try {
		syserr::throw_errno_error(EISCONN);
	} catch (syserr::SocketIsConnected &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EISCONN)");
	}
	try {
		syserr::throw_errno_error(ENOTCONN);
	} catch (syserr::SocketNotConnected &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENOTCONN)");
	}
	try {
		syserr::throw_errno_error(ENOTSOCK);
	} catch (syserr::NotASocket &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENOTSOCK)");
	}
	try {
		syserr::throw_errno_error(EHOSTUNREACH);
	} catch (syserr::HostUnreachable &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(EHOSTUNREACH)");
	}
	try {
		syserr::throw_errno_error(ENETUNREACH);
	} catch (syserr::NetUnreachable &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(ENETUNREACH)");
	}
	try {
		syserr::throw_errno_error(-1);
	} catch (syserr::UnknownError &e) {
	} catch (std::exception &e) {
		TEST_FAIL("syserr::ErrnoError(-1)");
	}
}
