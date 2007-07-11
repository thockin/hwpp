/*
 * Thin wrappers around standard file/directory IO.
 * Tim Hockin <thockin@hockin.org>
 */
#ifndef FILESYSTEM_HPP__
#define FILESYSTEM_HPP__

#include <string>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

#define _FILE_OFFSET_BITS 64
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

namespace fs {

class not_found_error: public std::runtime_error
{
    public:
	not_found_error(const std::string &str): runtime_error(str)
	{
	}
};

class io_error: public std::runtime_error
{
    public:
	io_error(const std::string &str): runtime_error(str)
	{
	}
};

/* forward declare the smart-pointer typedef */
class file;
typedef boost::shared_ptr<file> file_ptr;

class file
{
    private:
	/* constructors - private to prevent abuse */
	file(): m_path(""), m_mode(-1), m_fd(-1)
	{
	}
	file(const file &that)
	    : m_path(that.m_path), m_mode(that.m_mode), m_fd(that.m_fd)
	{
	}
	file &
	operator=(const file &that)
	{
		m_path = that.m_path;
		m_mode = that.m_mode;
		m_fd = that.m_fd;
		return *this;
	}

    public:
	/* destructor */
	~file()
	{
		if (m_fd >= 0) {
			close();
		}
	}

	static file_ptr
	open(const std::string &path, int mode)
	{
		file_ptr f(new file());

		f->m_path = path;
		f->m_mode = mode;
		f->m_fd = ::open(path.c_str(), mode);
		if (f->m_fd < 0) {
			if (errno == ENOENT) {
				throw not_found_error(path);
			}
			throw io_error(
			    std::string("fs::file::open(") + path + "): "
			    + strerror(errno));
		}

		return f;
	}

	void
	reopen(int new_mode)
	{
		int new_fd;

		new_fd = ::open(m_path.c_str(), new_mode);
		if (new_fd < 0) {
			if (errno == ENOENT) {
				throw not_found_error(m_path);
			}
			throw io_error(
			    std::string("fs::file::open(") + m_path + "): "
			    + strerror(errno));
		}

		int tmp = m_fd;
		m_mode = new_mode;
		m_fd = new_fd;
		if (::close(tmp) < 0) {
			throw io_error(
			    std::string("fs::file::close(") + m_path + "): "
			    + strerror(errno));
		}
	}

	void
	close()
	{
		if (::close(m_fd) < 0) {
			throw io_error(
			    std::string("fs::file::close(") + m_path + "): "
			    + strerror(errno));
		}
		m_fd = -1;
	}

	std::size_t
	read(void *buf, std::size_t size) const
	{
		int r;

		r = ::read(m_fd, buf, size);
		if (r < 0) {
			throw io_error(
			    std::string("fs::file::read(") + m_path + "): "
			    + strerror(errno));
		}

		return r;
	}

	//FIXME: add full_read() / full_write() methods?

	std::string
	read_line() const
	{
		std::string s;
		char buf[2] = " ";

		do {
			read(buf, 1);
			s.append(buf);
		} while (buf[0] != '\n');

		return s;
	}

	std::size_t
	write(void *buf, std::size_t size) const
	{
		int r;

		r = ::write(m_fd, buf, size);
		if (r < 0) {
			throw io_error(
			    std::string("fs::file::write(") + m_path + "): "
			    + strerror(errno));
		}

		return r;
	}

	::off_t
	seek(std::size_t off, int whence) const
	{
		::off_t r;

		r = ::lseek(m_fd, off, whence);
		if (r < 0) {
			throw io_error(
			    std::string("fs::file::seek(") + m_path + "): "
			    + strerror(errno));
		}

		return r;
	}

	off_t
	tell() const
	{
		return seek(0, SEEK_CUR);
	}

	std::string
	path() const
	{
		return m_path;
	}

	int
	mode() const
	{
		return m_mode & O_ACCMODE;
	}

	bool
	is_open() const
	{
		return (m_fd >= 0);
	}

    private:
	std::string m_path;
	int m_mode;
	int m_fd;
};

/* forward declare the smart-pointer typedef */
class direntry;
typedef boost::shared_ptr<direntry> direntry_ptr;

class direntry
{
    private:

    public:
	/* constructor - implicit conversion from ::dirent */
	direntry(struct ::dirent *de): m_dirent(de)
	{
	}

	/* destructor */
	~direntry()
	{
	}

	static bool
	exists(const std::string &path)
	{
		struct ::stat st;
		int r;

		r = ::stat(path.c_str(), &st);
		return (r == 0);
	}

	std::string
	name()
	{
		return m_dirent->d_name;
	}

	bool
	is_file()
	{
		return m_dirent->d_type & DT_REG;
	}

	static bool
	is_file(const std::string &path)
	{
		struct ::stat st;
		return (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode));
	}

	bool
	is_dir()
	{
		return m_dirent->d_type & DT_DIR;
	}

	static bool
	is_dir(const std::string &path)
	{
		struct ::stat st;
		return (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode));
	}

	bool
	is_link()
	{
		return m_dirent->d_type & DT_LNK;
	}

	static bool
	is_link(const std::string &path)
	{
		struct ::stat st;
		return (stat(path.c_str(), &st) == 0 && S_ISLNK(st.st_mode));
	}

	bool
	is_fifo()
	{
		return m_dirent->d_type & DT_FIFO;
	}

	static bool
	is_fifo(const std::string &path)
	{
		struct ::stat st;
		return (stat(path.c_str(), &st) == 0 && S_ISFIFO(st.st_mode));
	}

	bool
	is_socket()
	{
		return m_dirent->d_type & DT_SOCK;
	}

	static bool
	is_socket(const std::string &path)
	{
		struct ::stat st;
		return (stat(path.c_str(), &st) == 0 && S_ISSOCK(st.st_mode));
	}

	bool
	is_chrdev()
	{
		return m_dirent->d_type & DT_CHR;
	}

	static bool
	is_chrdev(const std::string &path)
	{
		struct ::stat st;
		return (stat(path.c_str(), &st) == 0 && S_ISCHR(st.st_mode));
	}

	bool
	is_blkdev()
	{
		return m_dirent->d_type & DT_BLK;
	}

	static bool
	is_blkdev(const std::string &path)
	{
		struct ::stat st;
		return (stat(path.c_str(), &st) == 0 && S_ISBLK(st.st_mode));
	}

	bool
	is_dev()
	{
		return (m_dirent->d_type & DT_CHR)
		    || (m_dirent->d_type & DT_BLK);
	}

	static bool
	is_dev(const std::string &path)
	{
		struct ::stat st;
		return (stat(path.c_str(), &st) == 0
		    && (S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode)));
	}

    private:
	struct ::dirent *m_dirent;
};

/* forward declare the smart-pointer typedef */
class directory;
typedef boost::shared_ptr<directory> directory_ptr;

class directory
{
    private:
	/* constructors - private to prevent abuse */
	directory(): m_path(""), m_dir(NULL)
	{
	}
	directory(const directory &that)
	    : m_path(that.m_path), m_dir(that.m_dir)
	{
	}
	directory &
	operator=(const directory &that)
	{
		m_path = that.m_path;
		m_dir = that.m_dir;
		return *this;
	}

    public:
	/* destructor */
	~directory()
	{
		if (m_dir) {
			close();
		}
	}

	static directory_ptr
	open(const std::string &path)
	{
		directory_ptr d(new directory());

		d->m_dir = ::opendir(path.c_str());
		if (!d->m_dir) {
			if (errno == ENOENT) {
				throw not_found_error(path);
			}
			throw io_error(
			    std::string("fs::directory::open(") + path + "): "
			    + strerror(errno));
		}

		return d;
	}

	void
	close()
	{
		::closedir(m_dir);
		m_dir = NULL;
	}

	direntry_ptr
	read() const
	{
		struct ::dirent *de = ::readdir(m_dir);
		if (de) {
			return direntry_ptr(new direntry(de));
		}
		return direntry_ptr();
	}

	void
	rewind() const
	{
		::rewinddir(m_dir);
	}

	const std::string &
	path() const
	{
		return m_path;
	}

	bool is_open() const
	{
		return (m_dir != NULL);
	}

    private:
	std::string m_path;
	::DIR *m_dir;
};

} /* namespace fs */

#endif /* FILESYSTEM_HPP__ */
