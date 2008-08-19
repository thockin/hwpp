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
class sys_error
{
    public:
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
	DEFINE_ERROR(unknown_error);

	// throw a specific error
	sys_error(int error)
	{
		std::string msg(::strerror(error));
		throw_real_error(error, msg);
	}
	sys_error(int error, std::string msg)
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
		    case EAGAIN:
			throw try_again(msg);
		    case ECHILD:
			throw no_child(msg);
		    case ETXTBSY:
			throw text_busy(msg);
		    default:
			throw unknown_error(msg);
		}
	}
};

#endif // SYS_ERROR_HPP__
