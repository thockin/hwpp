#include "filesystem.h"
#include <iostream>
#include <set>
#include "pp_test.h"

using namespace fs;
using namespace std;

int
test_file()
{
	int ret = 0;

	system("rm -f file.exists");

	if (direntry::exists("file.exists")) {
		PP_TEST_ERROR("fs::direntry::exists()");
		ret++;
	}

	system("echo -n exists > file.exists");

	if (!direntry::exists("file.exists")) {
		PP_TEST_ERROR("fs::direntry::exists()");
		ret++;
	}
	char buf[16];
	int r;
	file_ptr f = file::open("file.exists", O_RDONLY);

	r = f->read(buf, 16);
	buf[r] = '\0';
	if (string(buf) != "exists") {
		PP_TEST_ERROR("fs::file::read()");
		ret++;
	}

	system("rm -f file.exists");
	return ret;
}

int
test_file_mapping()
{
	int ret = 0;

	system("rm -f file.exists");
	system("echo -n exists > file.exists");

	file_ptr f = file::open("file.exists", O_RDONLY);

	char *p;

	file_mapping_ptr map = f->mmap(1, 4, PROT_READ, MAP_SHARED);
	p = (char *)map->address();
	if (strncmp(p, "xist", 4)) {
		PP_TEST_ERROR("fs::file::mmap()");
		ret++;
	}

	map = f->mmap(2, 4);
	p = (char *)map->address();
	if (strncmp(p, "ists", 4)) {
		PP_TEST_ERROR("fs::file::mmap()");
		ret++;
	}

	system("rm -f file.exists");
	return ret;
}

int
test_dir()
{
	int ret = 0;
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
		PP_TEST_ERROR("fs::directory::read()");
		ret++;
	}
	/* don't erase this, save it for rewind() */

	de = d->read();
	it = expected.find(de->name());
	if (it == expected.end()) {
		PP_TEST_ERROR("fs::directory::read()");
		ret++;
	}
	expected.erase(it);

	de = d->read();
	it = expected.find(de->name());
	if (it == expected.end()) {
		PP_TEST_ERROR("fs::directory::read()");
		ret++;
	}
	expected.erase(it);

	d->rewind();
	de = d->read();
	it = expected.find(de->name());
	if (it == expected.end()) {
		PP_TEST_ERROR("fs::directory::read()");
		ret++;
	}
	expected.erase(it);

	if (!expected.empty()) {
		PP_TEST_ERROR("fs::directory::read()");
		ret++;
	}

	system("rm -rf dir");
	return 0;
}

int
main(void)
{
	int ret = 0;

	ret += test_file();
	ret += test_file_mapping();
	ret += test_dir();

	return ret;
}
