#include "sys_error.h"
#include "pp_test.h"

int
test_sys_error()
{
	int ret = 0;

	try {
		throw sys_error(EACCES, "EACCES");
	} catch (sys_error::permission_denied &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(EACCES)");
	}
	try {
		throw sys_error(EPERM, "EPERM");
	} catch (sys_error::operation_not_permitted &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(EPERM)");
	}
	try {
		throw sys_error(ENOENT, "ENOENT");
	} catch (sys_error::not_found &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ENOENT)");
	}
	try {
		throw sys_error(EIO, "EIO");
	} catch (sys_error::io_error &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(EIO)");
	}
	try {
		throw sys_error(EINVAL, "EINVAL");
	} catch (sys_error::invalid &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(EINVAL)");
	}
	try {
		throw sys_error(ENXIO, "ENXIO");
	} catch (sys_error::no_device &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ENXIO)");
	}
	try {
		throw sys_error(ENODEV, "ENODEV");
	} catch (sys_error::no_device &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ENODEV)");
	}
	try {
		throw sys_error(EBUSY, "EBUSY");
	} catch (sys_error::busy &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(EBUSY)");
	}
	try {
		throw sys_error(EEXIST, "EEXIST");
	} catch (sys_error::exists &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(EEXIST)");
	}
	try {
		throw sys_error(ENOTDIR, "ENOTDIR");
	} catch (sys_error::not_a_directory &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ENOTDIR)");
	}
	try {
		throw sys_error(EISDIR, "EISDIR");
	} catch (sys_error::is_a_directory &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(EISDIR)");
	}
	try {
		throw sys_error(ENOSPC, "ENOSPC");
	} catch (sys_error::no_space &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ENOSPC)");
	}
	try {
		throw sys_error(ESPIPE, "ESPIPE");
	} catch (sys_error::illegal_seek &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ESPIPE)");
	}
	try {
		throw sys_error(EPIPE, "EPIPE");
	} catch (sys_error::broken_pipe &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(EPIPE)");
	}
	try {
		throw sys_error(ELOOP, "ELOOP");
	} catch (sys_error::too_many_symlinks &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ELOOP)");
	}
	try {
		throw sys_error(ENAMETOOLONG, "ENAMETOOLONG");
	} catch (sys_error::name_too_long &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ENAMETOOLONG)");
	}
	try {
		throw sys_error(ENOMEM, "ENOMEM");
	} catch (sys_error::out_of_memory &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ENOMEM)");
	}
	try {
		throw sys_error(ENOSYS, "ENOSYS");
	} catch (sys_error::not_implemented &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ENOSYS)");
	}
	try {
		throw sys_error(EFAULT, "EFAULT");
	} catch (sys_error::bad_address &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(EFAULT)");
	}
	try {
		throw sys_error(EBADF, "EBADF");
	} catch (sys_error::bad_file_descriptor &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(EBADF)");
	}
	try {
		throw sys_error(ERANGE, "ERANGE");
	} catch (sys_error::out_of_range &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ERANGE)");
	}
	try {
		throw sys_error(EAGAIN, "EAGAIN");
	} catch (sys_error::try_again &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(EAGAIN)");
	}
	try {
		throw sys_error(ECHILD, "ECHILD");
	} catch (sys_error::no_child &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ECHILD)");
	}
	try {
		throw sys_error(ETXTBSY, "ETXTBSY");
	} catch (sys_error::text_busy &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(ETXTBSY)");
	}
	try {
		throw sys_error(-1, "negative_one");
	} catch (sys_error::unknown_error &e) {
	} catch (std::exception &e) {
		ret += TEST_ERROR("sys_error(-1)");
	}

	return ret;
}

// Perform the tests
TEST_LIST(
	TEST(test_sys_error),
);
