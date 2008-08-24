#include "filesystem.h"
#include <iostream>
#include <set>
#include "pp_test.h"

#define FILE_EXISTS_NAME	"file.exists"
#define FILE_EXISTS_PATH	TEST_TMP_DIR "/" FILE_EXISTS_NAME
#define FILE_EXISTS_DATA	"data.exists"
#define FILE_EXISTS_SIZE	strlen(FILE_EXISTS_DATA)
#define FILE_NOT_EXISTS_PATH	FILE_EXISTS_PATH ".not"

#define DIR_EXISTS_NAME		"dir.exists"
#define DIR_EXISTS_PATH		TEST_TMP_DIR "/" DIR_EXISTS_NAME
#define DIR_NOT_EXISTS_PATH	DIR_EXISTS_PATH ".not"

#define TEMPFILE_TEMPLATE	TEST_TMP_DIR "/tempfile.XXXXXX"

TEST_SETUP_EACH()
{
	system("rm -f " FILE_EXISTS_PATH);
	system("echo -n " FILE_EXISTS_DATA " > " FILE_EXISTS_PATH);

	system("rm -rf " DIR_EXISTS_PATH);
	system("mkdir " DIR_EXISTS_PATH);
	system("echo -n " FILE_EXISTS_DATA " > "
	       DIR_EXISTS_PATH "/" FILE_EXISTS_NAME);
}

TEST_CLEANUP_EACH()
{
	system("rm -f " FILE_EXISTS_PATH);
	system("rm -rf " DIR_EXISTS_PATH);
}

TEST(test_file)
{
	if (fs::direntry::exists(FILE_NOT_EXISTS_PATH)) {
		TEST_ERROR("fs::direntry::exists()");
	}

	if (!fs::direntry::exists(FILE_EXISTS_PATH)) {
		TEST_ERROR("fs::direntry::exists()");
	}

	if (fs::file::size(FILE_EXISTS_PATH) != FILE_EXISTS_SIZE) {
		TEST_ERROR("fs::file::size(std::string)");
	}

	fs::file_ptr f = fs::file::open(FILE_EXISTS_PATH, O_RDONLY);
	if (!f->is_open()) {
		TEST_ERROR("fs::direntry::is_open()");
	}

	if (f->size() != FILE_EXISTS_SIZE) {
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
	if (f->tell() != FILE_EXISTS_SIZE-3) {
		TEST_ERROR("fs::file::seek()");
	}

	f->seek(FILE_EXISTS_SIZE+1, SEEK_SET);
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
	if (std::string(buf) != FILE_EXISTS_DATA) {
		TEST_ERROR("fs::file::read()");
	}

	f->unlink();
	if (fs::direntry::exists(FILE_EXISTS_PATH)) {
		TEST_ERROR("fs::file::unlink()");
	}

	f->close();
	if (f->is_open()) {
		TEST_ERROR("fs::file::is_open()");
	}

	f = fs::file::tempfile(TEMPFILE_TEMPLATE);
	if (!f->is_open()) {
		TEST_ERROR("fs::file::tempfile()");
	}
	f->close();
	if (f->is_open()) {
		TEST_ERROR("fs::file::is_open()");
	}

	std::string tempname = fs::file::tempname(TEMPFILE_TEMPLATE);
	if (fs::direntry::exists(tempname)) {
		TEST_ERROR("fs::file::tempname()");
	}
}

TEST(test_file_mapping)
{
	fs::file_ptr f = fs::file::open(FILE_EXISTS_PATH, O_RDONLY);
	fs::file_mapping_ptr map = f->mmap(1, 4, PROT_READ, MAP_SHARED);
	char *p = (char *)map->address();
	if (strncmp(p, &FILE_EXISTS_DATA[1], 4)) {
		TEST_ERROR("fs::file::mmap()");
	}

	map = f->mmap(2, 4);
	p = (char *)map->address();
	if (strncmp(p, &FILE_EXISTS_DATA[2], 4)) {
		TEST_ERROR("fs::file::mmap()");
	}
}

TEST(test_dir)
{
	if (fs::direntry::exists(DIR_NOT_EXISTS_PATH)) {
		TEST_ERROR("fs::direntry::exists()");
	}

	if (!fs::direntry::exists(DIR_EXISTS_PATH)) {
		TEST_ERROR("fs::direntry::exists()");
	}

	std::set<std::string> expected;
	std::set<std::string>::iterator it;

	expected.insert(".");
	expected.insert("..");
	expected.insert(FILE_EXISTS_NAME);

	fs::directory_ptr d = fs::directory::open(DIR_EXISTS_PATH);

	fs::direntry_ptr de;
	de = d->read();
	it = expected.find(de->name());
	if (it == expected.end()) {
		TEST_ERROR("fs::directory::read()");
	}
	// don't erase this, save it for rewind()

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
}

TEST(test_dev)
{
	std::string tempname = fs::file::tempname(TEMPFILE_TEMPLATE);
	if (fs::direntry::exists(tempname)) {
		TEST_ERROR("fs::file::tempname()");
	}
	try {
		// major 1, minor 3 = /dev/null
		fs::device::mkdev(tempname, 0600, S_IFCHR, 1, 3);
		if (!fs::direntry::exists(tempname)) {
			TEST_ERROR("fs::file::tempname()");
		}
	} catch (syserr::operation_not_permitted &e) {
		TEST_WARNING("must be root to call fs::device::mkdev()");
	}
}
