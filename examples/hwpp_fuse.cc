// TODO:
//   FS: maybe make read(field) = field->eval(), read(.field) = field->read()
//   FS: use getxattr for field->read(), reg->width, etc
//   move this to the pp tree
//   PP: multi-thread safety
//   FS: multi-thread safety
//   FS: open() - check fi->flags against the reg/field rwmode
//

// Suggested commandline options
// 	-f		- run in foreground
// 	-s		- run single threaded
// 	-oallow_other	- allow non-root users to access the FS
// 	-odirect_io	- do not do caching in the kernel
// 	-ofsname=ppfs	- set the FS type in mtab

#define FUSE_USE_VERSION 22

#define _FILE_OFFSET_BITS 64
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

#include "hwpp.h"
#include "drivers.h"
#include "device_init.h"
#include "register.h"
#include "datatype_types.h"
#include "field_types.h"
#include "array.h"
#include "alias.h"

using namespace std;

static hwpp::ScopePtr platform;
static time_t startup_time;

static void
fill_base_stat(struct stat *st)
{
	memset(st, 0, sizeof(struct stat));
	st->st_uid = 0;
	st->st_gid = 0;
	// we don't really know the length without reading
	st->st_size = 1024;
	st->st_mtime = st->st_atime = st->st_ctime = startup_time;
}
static void
fill_file_stat(struct stat *st)
{
	fill_base_stat(st);
	st->st_mode = S_IFREG | 0644;
	st->st_nlink = 1;
}
static void
fill_dir_stat(struct stat *st)
{
	fill_base_stat(st);
	st->st_mode = S_IFDIR | 0555;
	// this is a documented hack that allows 'find' to work properly
	st->st_nlink = 1;
}
static void
fill_link_stat(struct stat *st)
{
	fill_base_stat(st);
	st->st_mode = S_IFLNK | 0644;
	st->st_nlink = 1;
}
static int
fill_stat(const hwpp::ConstDirentPtr &de, struct stat *st)
{
	if (de->is_register() || de->is_field()) {
		fill_file_stat(st);
	} else if (de->is_scope()) {
		fill_dir_stat(st);
	} else if (de->is_alias()) {
		fill_link_stat(st);
	} else if (de->is_array()) {
		return -ENOENT;
	} else {
		return -EIO;
	}
	return 0;
}

static int
ppfs_getattr(const char *path, struct stat *st)
{
printf("%d getattr: path = %s\n", getpid(), path);
	try {
		const hwpp::ConstDirentPtr &de = platform->lookup_dirent(path);
		if (!de) {
			return -ENOENT;
		}
		return fill_stat(de, st);
	} catch (std::out_of_range &e) {
		return -ENOENT;
	} catch (hwpp::Path::InvalidError &e) {
		return -ENOENT;
	}
}

static int
fill_dirent(void *buf, fuse_fill_dir_t filler, const hwpp::ConstDirentPtr &de,
            const std::string &name)
{
	struct stat st;
	int ret;

	if (de->is_array()) {
		// array dirents get flattened
		const hwpp::ConstArrayPtr &array = hwpp::array_from_dirent(de);
		for (size_t i = 0; i < array->size(); i++) {
			string s = to_string(boost::format("%s[%d]") %name %i);
			ret = fill_dirent(buf, filler, array->at(i), s);
			if (ret < 0) {
				return ret;
			}
		}
	} else {
		// other dirents are simple
		memset(&st, 0, sizeof(struct stat));
		ret = fill_stat(de, &st);
		if (ret < 0) {
			return ret;
		}
		filler(buf, name.c_str(), &st, 0);
	}
	return 0;
}

static int
ppfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi)
{
	(void)offset;
	(void)fi;

printf("%d readdir: path = %s\n", getpid(), path);
	const hwpp::ConstDirentPtr &de = platform->lookup_dirent(path);
	if (!de) {
		return -ENOENT;
	}
	if (de->is_scope()) {
		struct stat st;
		int ret;

		fill_dir_stat(&st);
		filler(buf, ".", &st, 0);
		filler(buf, "..", &st, 0);

		const hwpp::ConstScopePtr &scope = hwpp::scope_from_dirent(de);
		for (size_t i = 0; i < scope->n_dirents(); i++) {
			ret = fill_dirent(buf, filler, scope->dirent(i),
			                  scope->dirent_name(i));
			if (ret < 0) {
				return ret;
			}
		}
	} else {
		return -ENOTDIR;
	}

	return 0;
}

static int
ppfs_readlink(const char *path, char *buf, size_t bufsize)
{
printf("%d readlink: path = %s, size = %zd\n", getpid(), path, bufsize);
	const hwpp::ConstDirentPtr &de = platform->lookup_dirent(path);
	if (!de) {
		return -ENOENT;
	}
	if (de->is_alias()) {
		const hwpp::ConstAliasPtr &alias = hwpp::alias_from_dirent(de);
		std::string pointee = to_string(alias->link_path());
		if (bufsize > pointee.size()) {
			bufsize = pointee.size();
		}
		strncpy(buf, pointee.data(), bufsize);
	} else {
		return -EINVAL;
	}

	return 0;
}

static int
ppfs_truncate(const char *path, off_t size)
{
	(void)path;
	(void)size;
	return 0;
}

static int
ppfs_open(const char *path, struct fuse_file_info *fi)
{
printf("%d open: path = %s\n", getpid(), path);
	// pretend this is direct IO so short read() works properly
	fi->direct_io = 0;
	return 0;
}

static int
ppfs_read(const char *path, char *data, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	(void)fi;

	int ret;

printf("%d read: path = %s\n", getpid(), path);
	try {
		const hwpp::ConstDirentPtr &de = platform->lookup_dirent(path);
		if (!de) {
			return -ENOENT;
		}
		hwpp::Value val;
		string str;
		if (de->is_register()) {
			val = hwpp::register_from_dirent(de)->read();
			str = val.get_str(16);
		} else if (de->is_field()) {
			str = hwpp::field_from_dirent(de)->evaluate();
		} else {
			return -EISDIR;
		}
		str += '\n';

		size_t len = str.length();
		if ((size_t)offset < len) {
			if (offset + size > len) {
				size = len - offset;
			}
			memcpy(data, str.c_str() + offset, size);
		} else {
			size = 0;
		}

		ret = size;
	} catch (std::out_of_range &e) {
		ret = -ENOENT;
	}

	return ret;
}

static char *
chomp(char *str, size_t size)
{
	char *p;

	// kill trailing whitespace, newlines, etc
	p = str + size-1;
	while (p >= str && (isblank(*p) || iscntrl(*p))) {
		*p = '\0';
		p--;
	}

	// skip leading whitespace
	p = str;
	while (*p && isblank(*p)) {
		p++;
	}

	return p;
}

static int
ppfs_write(const char *path, const char *data, size_t size,
		off_t offset, struct fuse_file_info *fi)
{
	(void)offset;
	(void)fi;

	int ret;

printf("%d write: path = %s\n", getpid(), path);
	try {
		std::vector<char> my_data(size+1);
		memcpy(&my_data[0], data, size);
		my_data[size] = '\0';

		char *clean_data = chomp(&my_data[0], size);

		const hwpp::ConstDirentPtr &de = platform->lookup_dirent(path);
		if (!de) {
			return -ENOENT;
		}
		if (de->is_register()) {
			const hwpp::ConstRegisterPtr &reg =
			   hwpp::register_from_dirent(de);
			hwpp::Value val;
			if (isdigit(clean_data[0])) {
				val = hwpp::Value(clean_data);
			} else {
				return -EINVAL;
			}
			reg->write(val);
		} else if (de->is_field()) {
			const hwpp::ConstFieldPtr &field =
			   hwpp::field_from_dirent(de);
			hwpp::Value val;
			if (isdigit(clean_data[0])) {
				val = hwpp::Value(clean_data);
			} else {
				try {
					val = field->lookup(clean_data);
				} catch (hwpp::Datatype::InvalidError &e) {
					return -EINVAL;
				}
			}
			field->write(val);
		} else {
			return -EISDIR;
		}

		ret = size;
	} catch (std::out_of_range &e) {
		ret = -ENOENT;
	}

	return ret;
}

static int ppfs_release(const char *path, struct fuse_file_info *fi)
{
	// stub
	(void)path;
	(void)fi;
	return 0;
}

// if this is local to main(), I get a segfault!
static struct fuse_operations ppfs_ops;

int main(int argc, char *argv[])
{
	system("modprobe fuse >/dev/null 2>&1");

	startup_time = time(NULL);
	umask(0);

	ppfs_ops.getattr	= ppfs_getattr;
	ppfs_ops.readdir	= ppfs_readdir;
	ppfs_ops.readlink	= ppfs_readlink;
	ppfs_ops.truncate	= ppfs_truncate;
	ppfs_ops.open		= ppfs_open;
	ppfs_ops.read		= ppfs_read;
	ppfs_ops.write		= ppfs_write;
	ppfs_ops.release	= ppfs_release;

	platform = hwpp::initialize_device_tree();
	hwpp::do_discovery();

	return fuse_main(argc, argv, &ppfs_ops);
}
