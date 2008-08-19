#include "syserror.h"
#include "pp_test.h"

int
test_syserror()
{
	int ret = 0;

	try {
		throw syserr::errno_error(EACCES);
	} catch (syserr::permission_denied &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EACCES)");
	}
	try {
		throw syserr::errno_error(EPERM);
	} catch (syserr::operation_not_permitted &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EPERM)");
	}
	try {
		throw syserr::errno_error(ENOENT);
	} catch (syserr::not_found &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENOENT)");
	}
	try {
		throw syserr::errno_error(EIO);
	} catch (syserr::io_error &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EIO)");
	}
	try {
		throw syserr::errno_error(EINVAL);
	} catch (syserr::invalid &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EINVAL)");
	}
	try {
		throw syserr::errno_error(ENXIO);
	} catch (syserr::no_device &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENXIO)");
	}
	try {
		throw syserr::errno_error(ENODEV);
	} catch (syserr::no_device &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENODEV)");
	}
	try {
		throw syserr::errno_error(EBUSY);
	} catch (syserr::busy &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EBUSY)");
	}
	try {
		throw syserr::errno_error(EEXIST);
	} catch (syserr::exists &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EEXIST)");
	}
	try {
		throw syserr::errno_error(ENOTDIR);
	} catch (syserr::not_a_directory &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENOTDIR)");
	}
	try {
		throw syserr::errno_error(EISDIR);
	} catch (syserr::is_a_directory &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EISDIR)");
	}
	try {
		throw syserr::errno_error(ENOSPC);
	} catch (syserr::no_space &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENOSPC)");
	}
	try {
		throw syserr::errno_error(ESPIPE);
	} catch (syserr::illegal_seek &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ESPIPE)");
	}
	try {
		throw syserr::errno_error(EPIPE);
	} catch (syserr::broken_pipe &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EPIPE)");
	}
	try {
		throw syserr::errno_error(ELOOP);
	} catch (syserr::too_many_symlinks &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ELOOP)");
	}
	try {
		throw syserr::errno_error(ENAMETOOLONG);
	} catch (syserr::name_too_long &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENAMETOOLONG)");
	}
	try {
		throw syserr::errno_error(ENOMEM);
	} catch (syserr::out_of_memory &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENOMEM)");
	}
	try {
		throw syserr::errno_error(ENOSYS);
	} catch (syserr::not_implemented &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENOSYS)");
	}
	try {
		throw syserr::errno_error(EFAULT);
	} catch (syserr::bad_address &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EFAULT)");
	}
	try {
		throw syserr::errno_error(EBADF);
	} catch (syserr::bad_file_descriptor &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EBADF)");
	}
	try {
		throw syserr::errno_error(ERANGE);
	} catch (syserr::out_of_range &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ERANGE)");
	}
	try {
		throw syserr::errno_error(EAGAIN);
	} catch (syserr::try_again &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EAGAIN)");
	}
	try {
		throw syserr::errno_error(EWOULDBLOCK);
	} catch (syserr::try_again &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EWOULDBLOCK)");
	}
	try {
		throw syserr::errno_error(ECHILD);
	} catch (syserr::no_child &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ECHILD)");
	}
	try {
		throw syserr::errno_error(ETXTBSY);
	} catch (syserr::text_busy &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ETXTBSY)");
	}
	try {
		throw syserr::errno_error(EINTR);
	} catch (syserr::interrupted &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EINTR)");
	}
	try {
		throw syserr::errno_error(EINPROGRESS);
	} catch (syserr::operation_in_progress &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EINPROGRESS)");
	}
	try {
		throw syserr::errno_error(ENOTSUP);
	} catch (syserr::not_supported &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENOTSUP)");
	}
	try {
		throw syserr::errno_error(EOPNOTSUPP);
	} catch (syserr::not_supported &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EOPNOTSUPP)");
	}
	try {
		throw syserr::errno_error(ETIMEDOUT);
	} catch (syserr::operation_timed_out &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ETIMEDOUT)");
	}
	try {
		throw syserr::errno_error(EALREADY);
	} catch (syserr::connection_in_progress &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EALREADY)");
	}
	try {
		throw syserr::errno_error(ENOBUFS);
	} catch (syserr::no_buffer_space &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENOBUFS)");
	}
	try {
		throw syserr::errno_error(EPROTONOSUPPORT);
	} catch (syserr::protocol_not_supported &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EPROTONOSUPPORT)");
	}
	try {
		throw syserr::errno_error(ENOPROTOOPT);
	} catch (syserr::protocol_not_available &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENOPROTOOPT)");
	}
	try {
		throw syserr::errno_error(EPROTO);
	} catch (syserr::protocol_error &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EPROTO)");
	}
	try {
		throw syserr::errno_error(EAFNOSUPPORT);
	} catch (syserr::address_family_not_supported &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EAFNOSUPPORT)");
	}
	try {
		throw syserr::errno_error(EADDRINUSE);
	} catch (syserr::address_in_use &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EADDRINUSE)");
	}
	try {
		throw syserr::errno_error(ECONNREFUSED);
	} catch (syserr::connection_refused &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ECONNREFUSED)");
	}
	try {
		throw syserr::errno_error(ECONNABORTED);
	} catch (syserr::connection_aborted &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ECONNABORTED)");
	}
	try {
		throw syserr::errno_error(EISCONN);
	} catch (syserr::socket_is_connected &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EISCONN)");
	}
	try {
		throw syserr::errno_error(ENOTCONN);
	} catch (syserr::socket_not_connected &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENOTCONN)");
	}
	try {
		throw syserr::errno_error(ENOTSOCK);
	} catch (syserr::not_a_socket &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENOTSOCK)");
	}
	try {
		throw syserr::errno_error(EHOSTUNREACH);
	} catch (syserr::host_unreachable &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(EHOSTUNREACH)");
	}
	try {
		throw syserr::errno_error(ENETUNREACH);
	} catch (syserr::net_unreachable &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(ENETUNREACH)");
	}
	try {
		throw syserr::errno_error(-1);
	} catch (syserr::unknown_error &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("syserr::errno_error(-1)");
	}


	return ret;
}

// Perform the tests
TEST_LIST(
	TEST(test_syserror),
);
