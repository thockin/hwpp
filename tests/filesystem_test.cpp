#include "filesystem.h"
#include <iostream>
#include <set>
#include "pp_test.h"

namespace filesystem {

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
	if (Direntry::exists(FILE_NOT_EXISTS_PATH)) {
		TEST_FAIL("filesystem::Direntry::exists()");
	}

	if (!Direntry::exists(FILE_EXISTS_PATH)) {
		TEST_FAIL("filesystem::Direntry::exists()");
	}

	if (File::size(FILE_EXISTS_PATH) != FILE_EXISTS_SIZE) {
		TEST_FAIL("filesystem::File::size(std::string)");
	}

	FilePtr f = File::open(FILE_EXISTS_PATH, O_RDONLY);
	if (!f->is_open()) {
		TEST_FAIL("filesystem::Direntry::is_open()");
	}

	if (f->size() != FILE_EXISTS_SIZE) {
		TEST_FAIL("filesystem::File::size()");
	}

	if (f->tell() != 0) {
		TEST_FAIL("filesystem::File::tell()");
	}

	f->seek(2, SEEK_SET);
	if (f->tell() != 2) {
		TEST_FAIL("filesystem::File::seek()");
	}
	f->seek(2, SEEK_CUR);
	if (f->tell() != 4) {
		TEST_FAIL("filesystem::File::seek()");
	}
	f->seek(-3, SEEK_END);
	if (f->tell() != FILE_EXISTS_SIZE-3) {
		TEST_FAIL("filesystem::File::seek()");
	}

	f->seek(FILE_EXISTS_SIZE+1, SEEK_SET);
	if (!f->is_eof()) {
		TEST_FAIL("filesystem::File::is_eof()");
	}

	f->seek(0, SEEK_SET);
	if (f->tell() != 0) {
		TEST_FAIL("filesystem::File::seek()");
	}

	char buf[16];
	int r;
	r = f->read(buf, 16);
	buf[r] = '\0';
	if (std::string(buf) != FILE_EXISTS_DATA) {
		TEST_FAIL("filesystem::File::read()");
	}

	f->unlink();
	if (Direntry::exists(FILE_EXISTS_PATH)) {
		TEST_FAIL("filesystem::File::unlink()");
	}

	f->close();
	if (f->is_open()) {
		TEST_FAIL("filesystem::File::is_open()");
	}

	f = File::tempfile(TEMPFILE_TEMPLATE);
	if (!f->is_open()) {
		TEST_FAIL("filesystem::File::tempfile()");
	}
	f->close();
	if (f->is_open()) {
		TEST_FAIL("filesystem::File::is_open()");
	}

	std::string tempname = File::tempname(TEMPFILE_TEMPLATE);
	if (Direntry::exists(tempname)) {
		TEST_FAIL("filesystem::File::tempname()");
	}
}

TEST(test_file_mapping)
{
	FilePtr f = File::open(FILE_EXISTS_PATH, O_RDONLY);
	FileMappingPtr map = f->mmap(1, 4, PROT_READ, MAP_SHARED);
	char *p = (char *)map->address();
	if (strncmp(p, &FILE_EXISTS_DATA[1], 4)) {
		TEST_FAIL("filesystem::File::mmap()");
	}

	map = f->mmap(2, 4);
	p = (char *)map->address();
	if (strncmp(p, &FILE_EXISTS_DATA[2], 4)) {
		TEST_FAIL("filesystem::File::mmap()");
	}
}

TEST(test_dir)
{
	if (Direntry::exists(DIR_NOT_EXISTS_PATH)) {
		TEST_FAIL("filesystem::Direntry::exists()");
	}

	if (!Direntry::exists(DIR_EXISTS_PATH)) {
		TEST_FAIL("filesystem::Direntry::exists()");
	}

	std::set<std::string> expected;
	std::set<std::string>::iterator it;

	expected.insert(".");
	expected.insert("..");
	expected.insert(FILE_EXISTS_NAME);

	DirectoryPtr d = Directory::open(DIR_EXISTS_PATH);

	DirentryPtr de;
	de = d->read();
	it = expected.find(de->name());
	if (it == expected.end()) {
		TEST_FAIL("filesystem::Directory::read()");
	}
	// don't erase this, save it for rewind()

	de = d->read();
	it = expected.find(de->name());
	if (it == expected.end()) {
		TEST_FAIL("filesystem::Directory::read()");
	}
	expected.erase(it);

	de = d->read();
	it = expected.find(de->name());
	if (it == expected.end()) {
		TEST_FAIL("filesystem::Directory::read()");
	}
	expected.erase(it);

	d->rewind();
	de = d->read();
	it = expected.find(de->name());
	if (it == expected.end()) {
		TEST_FAIL("filesystem::Directory::read()");
	}
	expected.erase(it);

	if (!expected.empty()) {
		TEST_FAIL("filesystem::Directory::read()");
	}
}

TEST(test_dev)
{
	std::string tempname = File::tempname(TEMPFILE_TEMPLATE);
	if (Direntry::exists(tempname)) {
		TEST_FAIL("filesystem::File::tempname()");
	}
	try {
		// major 1, minor 3 = /dev/null
		Device::mkdev(tempname, 0600, S_IFCHR, 1, 3);
		if (!Direntry::exists(tempname)) {
			TEST_FAIL("filesystem::File::tempname()");
		}
	} catch (syserr::operation_not_permitted &e) {
		TEST_WARN("must be root to call filesystem::Device::mkdev()");
	}
}

} // namespace filesystem
