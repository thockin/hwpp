#include "syserror.h"
#include "pp_test.h"

void
test_syserror()
{
	try {
		syserr::throw_errno_error(EACCES);
	} catch (syserr::permission_denied &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EACCES)");
	}
	try {
		syserr::throw_errno_error(EPERM);
	} catch (syserr::operation_not_permitted &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EPERM)");
	}
	try {
		syserr::throw_errno_error(ENOENT);
	} catch (syserr::not_found &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENOENT)");
	}
	try {
		syserr::throw_errno_error(EIO);
	} catch (syserr::io_error &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EIO)");
	}
	try {
		syserr::throw_errno_error(EINVAL);
	} catch (syserr::invalid &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EINVAL)");
	}
	try {
		syserr::throw_errno_error(ENXIO);
	} catch (syserr::no_device &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENXIO)");
	}
	try {
		syserr::throw_errno_error(ENODEV);
	} catch (syserr::no_device &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENODEV)");
	}
	try {
		syserr::throw_errno_error(EBUSY);
	} catch (syserr::busy &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EBUSY)");
	}
	try {
		syserr::throw_errno_error(EEXIST);
	} catch (syserr::exists &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EEXIST)");
	}
	try {
		syserr::throw_errno_error(ENOTDIR);
	} catch (syserr::not_a_directory &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENOTDIR)");
	}
	try {
		syserr::throw_errno_error(EISDIR);
	} catch (syserr::is_a_directory &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EISDIR)");
	}
	try {
		syserr::throw_errno_error(ENOSPC);
	} catch (syserr::no_space &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENOSPC)");
	}
	try {
		syserr::throw_errno_error(ESPIPE);
	} catch (syserr::illegal_seek &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ESPIPE)");
	}
	try {
		syserr::throw_errno_error(EPIPE);
	} catch (syserr::broken_pipe &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EPIPE)");
	}
	try {
		syserr::throw_errno_error(ELOOP);
	} catch (syserr::too_many_symlinks &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ELOOP)");
	}
	try {
		syserr::throw_errno_error(ENAMETOOLONG);
	} catch (syserr::name_too_long &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENAMETOOLONG)");
	}
	try {
		syserr::throw_errno_error(ENOMEM);
	} catch (syserr::out_of_memory &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENOMEM)");
	}
	try {
		syserr::throw_errno_error(ENOSYS);
	} catch (syserr::not_implemented &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENOSYS)");
	}
	try {
		syserr::throw_errno_error(EFAULT);
	} catch (syserr::bad_address &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EFAULT)");
	}
	try {
		syserr::throw_errno_error(EBADF);
	} catch (syserr::bad_file_descriptor &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EBADF)");
	}
	try {
		syserr::throw_errno_error(ERANGE);
	} catch (syserr::out_of_range &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ERANGE)");
	}
	try {
		syserr::throw_errno_error(EAGAIN);
	} catch (syserr::try_again &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EAGAIN)");
	}
	try {
		syserr::throw_errno_error(EWOULDBLOCK);
	} catch (syserr::try_again &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EWOULDBLOCK)");
	}
	try {
		syserr::throw_errno_error(ECHILD);
	} catch (syserr::no_child &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ECHILD)");
	}
	try {
		syserr::throw_errno_error(ETXTBSY);
	} catch (syserr::text_busy &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ETXTBSY)");
	}
	try {
		syserr::throw_errno_error(EINTR);
	} catch (syserr::interrupted &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EINTR)");
	}
	try {
		syserr::throw_errno_error(EINPROGRESS);
	} catch (syserr::operation_in_progress &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EINPROGRESS)");
	}
	try {
		syserr::throw_errno_error(ENOTSUP);
	} catch (syserr::not_supported &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENOTSUP)");
	}
	try {
		syserr::throw_errno_error(EOPNOTSUPP);
	} catch (syserr::not_supported &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EOPNOTSUPP)");
	}
	try {
		syserr::throw_errno_error(ETIMEDOUT);
	} catch (syserr::operation_timed_out &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ETIMEDOUT)");
	}
	try {
		syserr::throw_errno_error(EALREADY);
	} catch (syserr::connection_in_progress &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EALREADY)");
	}
	try {
		syserr::throw_errno_error(ENOBUFS);
	} catch (syserr::no_buffer_space &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENOBUFS)");
	}
	try {
		syserr::throw_errno_error(EPROTONOSUPPORT);
	} catch (syserr::protocol_not_supported &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EPROTONOSUPPORT)");
	}
	try {
		syserr::throw_errno_error(ENOPROTOOPT);
	} catch (syserr::protocol_not_available &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENOPROTOOPT)");
	}
	try {
		syserr::throw_errno_error(EPROTO);
	} catch (syserr::protocol_error &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EPROTO)");
	}
	try {
		syserr::throw_errno_error(EAFNOSUPPORT);
	} catch (syserr::address_family_not_supported &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EAFNOSUPPORT)");
	}
	try {
		syserr::throw_errno_error(EADDRINUSE);
	} catch (syserr::address_in_use &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EADDRINUSE)");
	}
	try {
		syserr::throw_errno_error(ECONNREFUSED);
	} catch (syserr::connection_refused &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ECONNREFUSED)");
	}
	try {
		syserr::throw_errno_error(ECONNABORTED);
	} catch (syserr::connection_aborted &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ECONNABORTED)");
	}
	try {
		syserr::throw_errno_error(EISCONN);
	} catch (syserr::socket_is_connected &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EISCONN)");
	}
	try {
		syserr::throw_errno_error(ENOTCONN);
	} catch (syserr::socket_not_connected &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENOTCONN)");
	}
	try {
		syserr::throw_errno_error(ENOTSOCK);
	} catch (syserr::not_a_socket &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENOTSOCK)");
	}
	try {
		syserr::throw_errno_error(EHOSTUNREACH);
	} catch (syserr::host_unreachable &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(EHOSTUNREACH)");
	}
	try {
		syserr::throw_errno_error(ENETUNREACH);
	} catch (syserr::net_unreachable &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(ENETUNREACH)");
	}
	try {
		syserr::throw_errno_error(-1);
	} catch (syserr::unknown_error &e) {
	} catch (std::exception &e) {
		TEST_ERROR("syserr::errno_error(-1)");
	}
}

// Perform the tests
TEST_LIST(
	TEST(test_syserror),
);
