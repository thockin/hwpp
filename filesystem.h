//
// Thin wrappers around standard file/directory IO.
// Tim Hockin <thockin@hockin.org>
//
#ifndef FILESYSTEM_HPP__
#define FILESYSTEM_HPP__

#undef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64

#include <string>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "pp.h"
#include "syserror.h"

namespace fs {

using std::size_t;
using ::off_t;

// a file
class file;
typedef boost::shared_ptr<file> file_ptr;
typedef boost::shared_ptr<const file> const_file_ptr;
typedef boost::weak_ptr<file> weak_file_ptr;

// a memory-mapped area of a file
class file_mapping;
typedef boost::shared_ptr<file_mapping> file_mapping_ptr;

// a directory
class directory;
typedef boost::shared_ptr<directory> directory_ptr;

// a directory entry
class direntry;
typedef boost::shared_ptr<direntry> direntry_ptr;


//
// file_mapping
//
// This class tracks an individual mmap() of a file.  When this class is
// destructed, the mapping is munmap()ed.
//
// Users can not create instances of this class.  To create a mapping call
// fs::file::mmap(), which returns a smart pointer to one of these.
//
class file_mapping
{
	friend class file;

    private:
	// constructors - private to prevent abuse, defined later
	file_mapping(const const_file_ptr &file, off_t offset, size_t length,
			int prot = PROT_READ, int flags = MAP_SHARED);

    public:
	// destructor
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

		// If other references exist, this can be bad.  Better to
		// use the destructor whenever possible.
		int r = munmap(m_real_address, m_real_length);
		if (r < 0) {
			syserr::throw_errno_error(errno,
			    "fs::file_mapping::unmap()");
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

	off_t
	offset() const
	{
		return m_offset;
	}

	size_t
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
	// these are the file offset and map length that were requested
	off_t m_offset;
	size_t m_length;
	uint8_t *m_address;
	// these are the aligned file offset and map length
	off_t m_real_offset;
	size_t m_real_length;
	uint8_t *m_real_address;
	// flags for the mapping
	int m_prot;
	int m_flags;
};

//
// file
//
// This class represents a single opened file.  When this class is
// destructed, the file descriptor is close()d.
//
// Users can not create instances of this class.  To open a file call
// fs::file::open(), which returns a smart pointer to one of these.
//
class file
{
    private:
	// constructors - private to prevent abuse
	file(): m_path(""), m_flags(-1), m_fd(-1)
	{
	}
	file(const file &that)
	    : m_path(that.m_path), m_flags(that.m_flags), m_fd(that.m_fd)
	{
	}
	file &
	operator=(const file &that)
	{
		m_path = that.m_path;
		m_flags = that.m_flags;
		m_fd = that.m_fd;
		return *this;
	}

    public:
	// destructor
	~file()
	{
		if (m_fd >= 0) {
			close();
		}
	}

	static file_ptr
	open(const std::string &path, int flags)
	{
		file_ptr f(new file());

		f->m_this_ptr = f;
		f->m_path = path;
		f->m_flags = flags;
		f->m_fd = ::open(path.c_str(), flags);
		if (f->m_fd < 0) {
			syserr::throw_errno_error(errno,
			    "fs::file::open(" + path + ")");
		}

		return f;
	}

	static file_ptr
	fdopen(int fd, const std::string &path = "", int flags = -1)
	{
		file_ptr f(new file());

		f->m_this_ptr = f;
		f->m_path = path;
		f->m_flags = flags;
		f->m_fd = fd;

		return f;
	}

	static file_ptr
	tempfile(std::string path_template = "")
	{
		if (path_template == "") {
			path_template = find_tmp_dir() + "/"
				+ to_string(getpid()) + ".XXXXXX";
		}

		int r;
		char buf[path_template.size()+1]; // for mkstemp()

		path_template.copy(buf, path_template.size());
		buf[path_template.size()] = '\0';
		r = ::mkstemp(buf);
		if (r < 0) {
			syserr::throw_errno_error(errno,
			    "fs::file::tempfile(" + path_template + ")");
		}

		return fdopen(r, buf, O_RDWR);
	}

	// warning: this is racy, but sometimes that is OK
	static std::string
	tempname(std::string path_template = "")
	{
		fs::file_ptr f = fs::file::tempfile(path_template);
		std::string filename = f->path();
		f->close();
		unlink(filename);
		return filename;
	}

	void
	reopen(int new_flags)
	{
		int new_fd;

		new_fd = ::open(m_path.c_str(), new_flags);
		if (new_fd < 0) {
			syserr::throw_errno_error(errno,
			    "fs::file::open(" + m_path + ")");
		}

		int tmp = m_fd;
		m_flags = new_flags;
		m_fd = new_fd;
		if (::close(tmp) < 0) {
			syserr::throw_errno_error(errno,
			    "fs::file::close(" + m_path + ")");
		}
	}

	void
	close()
	{
		//
		// If other references exist, this can be bad.  Better to
		// use the destructor whenever possible.
		//
		if (::close(m_fd) < 0) {
			syserr::throw_errno_error(errno,
			    "fs::file::close(" + m_path + ")");
		}
		m_fd = -1;
	}

	static void
	unlink(const std::string &path)
	{
		int r;

		r = ::unlink(path.c_str());
		if (r < 0) {
			syserr::throw_errno_error(errno,
			    "fs::file::unlink(" + path + ")");
		}
	}

	void
	unlink()
	{
		unlink(m_path);
	}

	size_t
	read(void *buf, size_t size) const
	{
		int r;

		r = ::read(m_fd, buf, size);
		if (r < 0) {
			syserr::throw_errno_error(errno,
			    "fs::file::read(" + m_path + ")");
		}

		return r;
	}

	//FIXME: stat
	//FIXME: create (static)
	//FIXME: rename (static with 2 args, and 1 arg)
	//FIXME: touch
	//FIXME: add full_read() / full_write() methods?

	std::string
	read_line() const
	{
		std::string s;
		char buf[2] = " ";

		do {
			read(buf, 1);
			//FIXME: check for errors
			s.append(buf);
		} while (buf[0] != '\n');

		return s;
	}

	size_t
	write(void *buf, size_t size) const
	{
		int r;

		r = ::write(m_fd, buf, size);
		if (r < 0) {
			syserr::throw_errno_error(errno,
			    "fs::file::write(" + m_path + ")");
		}

		return r;
	}

	file_mapping_ptr
	mmap(off_t offset, size_t length, int prot = -1,
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

	off_t
	seek(off_t offset, int whence) const
	{
		off_t r;

		r = ::lseek(m_fd, offset, whence);
		if (r == (off_t)-1) {
			syserr::throw_errno_error(errno,
			    "fs::file::seek(" + m_path + ")");
		}

		return r;
	}

	static off_t
	size(const std::string &path)
	{
		int r;
		struct stat st;

		r = ::stat(path.c_str(), &st);
		if (r < 0) {
			syserr::throw_errno_error(errno,
			    "fs::file::size(" + path + ")");
		}

		return st.st_size;
	}

	off_t
	size() const
	{
		int r;
		struct stat st;

		r = ::fstat(m_fd, &st);
		if (r < 0) {
			syserr::throw_errno_error(errno,
			    "fs::file::size(" + m_path + ")");
		}

		return st.st_size;
	}

	off_t
	tell() const
	{
		return seek(0, SEEK_CUR);
	}

	bool
	is_eof() const
	{
		return (tell() >= size());
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
		return m_flags & O_ACCMODE;
	}

	int
	flags() const
	{
		return m_flags;
	}

	int
	fd() const
	{
		return m_fd;
	}

    private:
	weak_file_ptr m_this_ptr;
	std::string m_path;
	int m_flags;
	int m_fd;

	static std::string
	find_tmp_dir()
	{
		char *p;

		p = getenv("TMPDIR");
		if (p) {
			return p;
		}
		p = getenv("TEMPDIR");
		if (p) {
			return p;
		}
		#ifdef P_tmpdir
		return P_tmpdir;
		#endif
		return "/tmp";
	}
};

inline
file_mapping::file_mapping(const const_file_ptr &file,
    off_t offset, size_t length, int prot, int flags)
    : m_file(file),
      m_offset(offset), m_length(length), m_address(NULL),
      m_real_offset(0), m_real_length(0), m_real_address(NULL),
      m_prot(prot), m_flags(flags)
{
	size_t pgsize;
	size_t pgmask;
	off_t ptr_off;

	// maps need to be page aligned
	pgsize = getpagesize();
	pgmask = pgsize - 1;
	ptr_off = m_offset & pgmask;
	m_real_offset = m_offset & ~((uint64_t)pgmask);
	m_real_length = pgsize + ((m_length + pgmask) & ~pgmask);

	m_real_address = (uint8_t *)mmap(NULL, m_real_length, prot,
			flags, m_file->fd(), m_real_offset);
	if (!m_real_address || m_real_address == MAP_FAILED) {
		syserr::throw_errno_error(errno,
		    "fs::file_mapping::file_mapping()");
	}

	m_address = m_real_address + ptr_off;
}

//
// device
//
// This class represents a single device node.
//
// Users can not create instances of this class.  To open a device call
// fs::device::open(), which returns a smart pointer to one of these.
//
class device: public file
{
    public:
	static void
	mkdev(const std::string &path, mode_t perms, int type,
			int major, int minor)
	{
		int r;

		r = ::mknod(path.c_str(), perms | type,
				::makedev(major, minor));
		if (r < 0) {
			syserr::throw_errno_error(errno,
			    "fs::device::mkdev(" + path + ")");
		}
	}
};

//
// direntry
//
// This class represents a single directory entry.
//
// Users can not create instances of this class.  To access a direntry, use
// fs::directory::read(), which returns a smart pointer to one of these.
//
class direntry
{
    friend class directory;

    private:
	// constructor - implicit conversion from ::dirent
	direntry(struct ::dirent *de): m_dirent(de)
	{
	}

    public:
	// destructor
	~direntry()
	{
	}

	static bool
	exists(const std::string &path)
	{
		struct ::stat st;
		int r = ::stat(path.c_str(), &st);
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
		return (::stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode));
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
		return (::stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode));
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
		return (::stat(path.c_str(), &st) == 0 && S_ISLNK(st.st_mode));
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
		return (::stat(path.c_str(), &st) == 0 && S_ISFIFO(st.st_mode));
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
		return (::stat(path.c_str(), &st) == 0 && S_ISSOCK(st.st_mode));
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
		return (::stat(path.c_str(), &st) == 0 && S_ISCHR(st.st_mode));
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
		return (::stat(path.c_str(), &st) == 0 && S_ISBLK(st.st_mode));
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
		return (::stat(path.c_str(), &st) == 0
		    && (S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode)));
	}

    private:
	struct ::dirent *m_dirent;
};

//
// directory
//
// This class represents a single directory.  When this class is destructed,
// the directory is closedir()d.
//
// Users can not create instances of this class.  To access a direntry, use
// fs::directory::open(), which returns a smart pointer to one of these.
//
class directory
{
    private:
	// constructors - private to prevent abuse
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
	// destructor
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
			syserr::throw_errno_error(errno,
			    "fs::directory::open(" + path + ")");
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
	//FIXME: mkdtemp => tempname() and tempdir()

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

inline void
chdir(const string &path)
{
	int ret = ::chdir(path.c_str());
	if (ret < 0) {
		syserr::throw_errno_error(errno, path);
	}
}
//FIXME: chdir(directory)

inline const string
getcwd()
{
	#ifdef _GNU_SOURCE
	char *p = ::get_current_dir_name();
	if (p == NULL) {
		syserr::throw_errno_error(errno, "fs::getcwd()");
	}
	string cwd(p);
	free(p);
	return cwd;
	#else
	char buf[4096];
	char *p = ::getcwd(buf, sizeof(buf));
	if (p == NULL) {
		syserr::throw_errno_error(errno, "fs::getcwd()");
	}
	return p;
	#endif
}

} // namespace fs

#endif // FILESYSTEM_HPP__
