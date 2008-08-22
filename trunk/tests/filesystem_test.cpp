#include "filesystem.h"
#include <iostream>
#include <set>
#include "pp_test.h"

using namespace fs;
using namespace std;

void
test_file()
{
	system("rm -f file.exists");

	if (direntry::exists("file.exists")) {
		TEST_ERROR("fs::direntry::exists()");
	}

	system("echo -n exists > file.exists");

	if (!direntry::exists("file.exists")) {
		TEST_ERROR("fs::direntry::exists()");
	}

	if (file::size("file.exists") != 6) {
		TEST_ERROR("fs::file::size(string)");
	}

	file_ptr f = file::open("file.exists", O_RDONLY);
	if (!f->is_open()) {
		TEST_ERROR("fs::direntry::is_open()");
	}

	if (f->size() != 6) {
		TEST_ERROR("fs::file::size()");
	}

	if (f->tell() != 0) {
		TEST_ERROR("fs::file::tell()");
	}

	f->seek(2, SEEK_SET);
	if (f->tell() != 2) {
		TEST_ERROR("fs::file::seek()");
	}
	f->seek(2, SEEK_CUR);
	if (f->tell() != 4) {
		TEST_ERROR("fs::file::seek()");
	}
	f->seek(-3, SEEK_END);
	if (f->tell() != 3) {
		TEST_ERROR("fs::file::seek()");
	}

	f->seek(7, SEEK_SET);
	if (!f->is_eof()) {
		TEST_ERROR("fs::file::is_eof()");
	}

	f->seek(0, SEEK_SET);
	if (f->tell() != 0) {
		TEST_ERROR("fs::file::seek()");
	}

	char buf[16];
	int r;
	r = f->read(buf, 16);
	buf[r] = '\0';
	if (string(buf) != "exists") {
		TEST_ERROR("fs::file::read()");
	}

	f->unlink();
	if (direntry::exists("file.exists")) {
		TEST_ERROR("fs::direntry::unlink()");
	}

	f->close();
	if (f->is_open()) {
		TEST_ERROR("fs::direntry::is_open()");
	}

	f = file::tempfile();
	if (!f->is_open()) {
		TEST_ERROR("fs::direntry::tempfile()");
	}

	string tempname = file::tempname();
	if (direntry::exists(tempname)) {
		TEST_ERROR("fs::file::tempname()");
	}

	system("rm -f file.exists");
}

void
test_file_mapping()
{
	system("rm -f file.exists");
	system("echo -n exists > file.exists");

	file_ptr f = file::open("file.exists", O_RDONLY);

	char *p;

	file_mapping_ptr map = f->mmap(1, 4, PROT_READ, MAP_SHARED);
	p = (char *)map->address();
	if (strncmp(p, "xist", 4)) {
		TEST_ERROR("fs::file::mmap()");
	}

	map = f->mmap(2, 4);
	p = (char *)map->address();
	if (strncmp(p, "ists", 4)) {
		TEST_ERROR("fs::file::mmap()");
	}

	system("rm -f file.exists");
}

void
test_dir()
{
	set<string> expected;
	set<string>::iterator it;

	system("rm -rf dir; mkdir dir; touch dir/a");
	expected.insert(".");
	expected.insert("..");
	expected.insert("a");

	directory_ptr d = fs::directory::open("dir");

	direntry_ptr de;
	de = d->read();
	it = expected.find(de->name());
	if (it == expected.end()) {
		TEST_ERROR("fs::directory::read()");
	}
	/* don't erase this, save it for rewind() */

	de = d->read();
	it = expected.find(de->name());
	if (it == expected.end()) {
		TEST_ERROR("fs::directory::read()");
	}
	expected.erase(it);

	de = d->read();
	it = expected.find(de->name());
	if (it == expected.end()) {
		TEST_ERROR("fs::directory::read()");
	}
	expected.erase(it);

	d->rewind();
	de = d->read();
	it = expected.find(de->name());
	if (it == expected.end()) {
		TEST_ERROR("fs::directory::read()");
	}
	expected.erase(it);

	if (!expected.empty()) {
		TEST_ERROR("fs::directory::read()");
	}

	system("rm -rf dir");
}

void
test_dev()
{
	string tempname = file::tempname();
	if (direntry::exists(tempname)) {
		TEST_ERROR("fs::file::tempname()");
	}
	try {
		// major 1, minor 3 = /dev/null
		device::mkdev(tempname, 0600, S_IFCHR, 1, 3);
		if (!direntry::exists(tempname)) {
			TEST_ERROR("fs::file::tempname()");
		}
	} catch (syserr::operation_not_permitted &e) {
		TEST_WARNING("must be root to call fs::device::mkdev()");
	}
}

TEST_LIST(
	TEST(test_file),
	TEST(test_file_mapping),
	TEST(test_dir),
	TEST(test_dev),
);
