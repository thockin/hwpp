//
// Tim Hockin <thockin@hockin.org>
//
#ifndef SYS_ERROR_HPP__
#define SYS_ERROR_HPP__

#include <string>
#include <stdexcept>
#include <errno.h>
#include <string.h>

#define DEFINE_ERROR(name) \
	struct name: public std::runtime_error \
	{ \
		explicit name(const std::string &str): runtime_error(str) \
		{} \
	};

// callers can simply "throw sys_error(errno);"
struct sys_error
{
	// define a class for each errno
	DEFINE_ERROR(permission_denied);
	DEFINE_ERROR(not_permitted);
	DEFINE_ERROR(not_found);
	DEFINE_ERROR(io_error);
	DEFINE_ERROR(invalid);
	DEFINE_ERROR(no_device);
	DEFINE_ERROR(busy);
	DEFINE_ERROR(exists);
	DEFINE_ERROR(not_directory);
	DEFINE_ERROR(is_directory);
	DEFINE_ERROR(no_space);
	DEFINE_ERROR(illegal_seek);
	DEFINE_ERROR(broken_pipe);
	DEFINE_ERROR(too_many_symlinks);
	DEFINE_ERROR(name_too_long);
	DEFINE_ERROR(out_of_memory);
	DEFINE_ERROR(not_implemented);
	DEFINE_ERROR(bad_address);
	DEFINE_ERROR(bad_file);
	DEFINE_ERROR(out_of_range);
	DEFINE_ERROR(try_again);
	DEFINE_ERROR(no_child);
	DEFINE_ERROR(text_busy);
	DEFINE_ERROR(unknown_error);

	// throw a specific error
	sys_error(int error, const std::string &msg)
	{
		const std::string &errstr(msg + ": " + ::strerror(error));
		switch (error) {
		    case EACCES:
			throw permission_denied(errstr);
		    case EPERM:
			throw not_permitted(errstr);
		    case ENOENT:
			throw not_found(errstr);
		    case EIO:
			throw io_error(errstr);
		    case EINVAL:
			throw invalid(errstr);
		    case ENXIO:
		    case ENODEV:
			throw no_device(errstr);
		    case EBUSY:
			throw busy(errstr);
		    case EEXIST:
			throw exists(errstr);
		    case ENOTDIR:
			throw not_directory(errstr);
		    case EISDIR:
			throw is_directory(errstr);
		    case ENOSPC:
			throw no_space(errstr);
		    case ESPIPE:
			throw illegal_seek(errstr);
		    case EPIPE:
			throw broken_pipe(errstr);
		    case ELOOP:
			throw too_many_symlinks(errstr);
		    case ENAMETOOLONG:
			throw name_too_long(errstr);
		    case ENOMEM:
			throw out_of_memory(errstr);
		    case ENOSYS:
			throw not_implemented(errstr);
		    case EFAULT:
			throw bad_address(errstr);
		    case EBADF:
			throw bad_file(errstr);
		    case ERANGE:
			throw out_of_range(errstr);
		    case EAGAIN:
			throw try_again(errstr);
		    case ECHILD:
			throw no_child(errstr);
		    case ETXTBSY:
			throw text_busy(errstr);
		    default:
			throw unknown_error(errstr);
		}
	}
};

#endif // SYS_ERROR_HPP__
