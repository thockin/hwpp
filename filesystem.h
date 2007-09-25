/*
 * Thin wrappers around standard file/directory IO.
 * Tim Hockin <thockin@hockin.org>
 */
#ifndef FILESYSTEM_HPP__
#define FILESYSTEM_HPP__

#include <string>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#define _FILE_OFFSET_BITS 64
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/mman.h>

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

/* a file */
class file;
typedef boost::shared_ptr<file> file_ptr;
typedef boost::shared_ptr<const file> const_file_ptr;
typedef boost::weak_ptr<file> weak_file_ptr;

/* a memory-mapped area of a file */
class file_mapping;
typedef boost::shared_ptr<file_mapping> file_mapping_ptr;

/* a directory */
class directory;
typedef boost::shared_ptr<directory> directory_ptr;

/* a directory entry */
class direntry;
typedef boost::shared_ptr<direntry> direntry_ptr;


/*
 * file_mapping
 *
 * This class tracks an individual mmap() of a file.  When this class is
 * destructed, the mapping is munmap()ed.
 *
 * Users can not create instances of this class.  To create a mapping call
 * fs::file::mmap(), which returns a smart pointer to one of these.
 */
class file_mapping
{
	friend class file;

    private:
	/* constructors - private to prevent abuse, defined later */
	file_mapping(const_file_ptr file, ::off_t offset, std::size_t length,
			int prot = PROT_READ, int flags = MAP_SHARED);

    public:
	/* destructor */
	~file_mapping()
	{
		unmap();
	}

	void
	unmap()
	{
		if (!is_mapped()) {
			return;
		}

		/*
		 * If other references exist, this can be bad.  Better to
		 * use the destructor whenever possible.
		 */
		int r = munmap(m_real_address, m_real_length);
		if (r < 0) {
			throw io_error(
			    std::string("fs::file_mapping::unmap(): ")
			    + strerror(errno));
		}
		m_address = m_real_address = NULL;
		m_length = m_real_length = 0;
	}

	bool
	is_mapped() const
	{
		return (m_address != NULL);
	}

	void *
	address() const
	{
		return (void *)m_address;
	}

	::off_t
	offset() const
	{
		return m_offset;
	}

	std::size_t
	length() const
	{
		return m_length;
	}

	int
	prot() const
	{
		return m_prot;
	}

	int
	flags() const
	{
		return m_flags;
	}

    private:
	const_file_ptr m_file;
	/* these are the file offset and map length that were requested */
	::off_t m_offset;
	std::size_t m_length;
	uint8_t *m_address;
	/* these are the aligned file offset and map length */
	::off_t m_real_offset;
	std::size_t m_real_length;
	uint8_t *m_real_address;
	/* flags for the mapping */
	int m_prot;
	int m_flags;
};

/*
 * file
 *
 * This class represents a single opened file.  When this class is
 * destructed, the file descriptor is close()d.
 *
 * Users can not create instances of this class.  To open a file call
 * fs::file::open(), which returns a smart pointer to one of these.
 */
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

		f->m_this_ptr = f;
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
		/*
		 * If other references exist, this can be bad.  Better to
		 * use the destructor whenever possible.
		 */
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

	//FIXME: create
	//FIXME: remove
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

	file_mapping_ptr
	mmap(::off_t offset, std::size_t length, int prot = -1,
			int flags = MAP_SHARED) const
	{
		if (prot == -1) {
			if (mode() == O_RDONLY) {
				prot = PROT_READ;
			} else if (mode() == O_WRONLY) {
				prot = PROT_WRITE;
			} else if (mode() == O_RDWR) {
				prot = PROT_READ | PROT_WRITE;
			}
		}

		return file_mapping_ptr(new file_mapping(
				const_file_ptr(m_this_ptr),
				offset, length, prot, flags));
	}

	::off_t
	seek(::off_t offset, int whence) const
	{
		::off_t r;

		r = ::lseek(m_fd, offset, whence);
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

	bool
	is_open() const
	{
		return (m_fd >= 0);
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

	int
	fd() const
	{
		return m_fd;
	}

    private:
	weak_file_ptr m_this_ptr;
	std::string m_path;
	int m_mode;
	int m_fd;
};

inline
file_mapping::file_mapping(const_file_ptr file,
    ::off_t offset, std::size_t length, int prot, int flags)
    : m_file(file),
      m_offset(offset), m_length(length), m_address(NULL),
      m_real_offset(0), m_real_length(0), m_real_address(NULL),
      m_prot(prot), m_flags(flags)
{
	std::size_t pgsize;
	std::size_t pgmask;
	::off_t ptr_off;

	/* maps need to be page aligned */
	pgsize = getpagesize();
	pgmask = pgsize - 1;
	ptr_off = m_offset & pgmask;
	m_real_offset = m_offset & ~((uint64_t)pgmask);
	m_real_length = pgsize + ((m_length + pgmask) & ~pgmask);

	m_real_address = (uint8_t *)mmap(NULL, m_real_length, prot,
			flags, m_file->fd(), m_real_offset);
	if (!m_real_address || m_real_address == MAP_FAILED) {
		throw io_error(
		    std::string("fs::file_mapping::file_mapping(): ")
		    + strerror(errno));
	}

	m_address = m_real_address + ptr_off;
}

/*
 * direntry
 *
 * This class represents a single directory entry.
 *
 * Users can not create instances of this class.  To access a direntry, use
 * fs::directory::read(), which returns a smart pointer to one of these.
 */
class direntry
{
    friend class directory;

    private:
	/* constructor - implicit conversion from ::dirent */
	direntry(struct ::dirent *de): m_dirent(de)
	{
	}

    public:
	/* destructor */
	~direntry()
	{
	}

	static bool
	exists(const std::string &path) const
	{
		struct ::stat st;
		int r;

		r = ::stat(path.c_str(), &st);
		return (r == 0);
	}

	std::string
	name() const
	{
		return m_dirent->d_name;
	}

	bool
	is_file() const
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
	is_dir() const
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
	is_link() const
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
	is_fifo() const
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
	is_socket() const
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
	is_chrdev() const
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
	is_blkdev() const
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
	is_dev() const
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

/*
 * directory
 *
 * This class represents a single directory.  When this class is destructed,
 * the directory is closedir()d.
 *
 * Users can not create instances of this class.  To access a direntry, use
 * fs::directory::open(), which returns a smart pointer to one of these.
 */
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

	//FIXME: create
	//FIXME: remove

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

	bool
	is_open() const
	{
		return (m_dir != NULL);
	}

	const std::string &
	path() const
	{
		return m_path;
	}

    private:
	std::string m_path;
	::DIR *m_dir;
};

} /* namespace fs */

#endif /* FILESYSTEM_HPP__ */
