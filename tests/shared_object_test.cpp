#include "pp.h"
#include "pp_test.h"
#include "shared_object.h"
using namespace std;

void
test_ctors()
{
	// test ctors
	{
		shared_object so;
		TEST_ASSERT(so.handle() == NULL,
			"shared_object::shared_object()");
		TEST_ASSERT(so.path() == "",
			"shared_object::shared_object()");
	}
	{
		shared_object so("libdl.so");
		TEST_ASSERT(so.handle() != NULL,
			"shared_object::shared_object(libdl.so)");
		TEST_ASSERT(so.path() == "libdl.so",
			"shared_object::shared_object(libdl.so)");
	}
	{
		shared_object so("libdl.so", RTLD_NOW | RTLD_GLOBAL);
		TEST_ASSERT(so.handle() != NULL,
			"shared_object::shared_object(libdl.so)");
		TEST_ASSERT(so.path() == "libdl.so",
			"shared_object::shared_object(libdl.so)");
	}
	{
		try {
			shared_object so("./nonexistant.so");
			TEST_ERROR("shared_object::shared_object(string)");
		} catch (shared_object::load_error &e) {
		}
	}
	{
		shared_object so("libdl.so");
		shared_object so2(so);
		TEST_ASSERT(so.handle() != NULL,
			"shared_object::shared_object(string)");
		TEST_ASSERT(so.path() == "libdl.so",
			"shared_object::shared_object(string)");
		TEST_ASSERT(so2.handle() == so.handle(),
			"shared_object::shared_object(shared_object)");
		TEST_ASSERT(so2.path() == "libdl.so",
			"shared_object::shared_object(lshared_object)");
	}
	{
		shared_object so("libdl.so");
		shared_object so2;
		so2 = so;
		TEST_ASSERT(so.handle() != NULL,
			"shared_object::shared_object(string)");
		TEST_ASSERT(so.path() == "libdl.so",
			"shared_object::shared_object(string)");
		TEST_ASSERT(so2.handle() == so.handle(),
			"shared_object::operator=(shared_object)");
		TEST_ASSERT(so2.path() == "libdl.so",
			"shared_object::operator=(lshared_object)");
	}
}

void
test_open_close()
{
	{
		shared_object so;

		so.open("libdl.so");
		TEST_ASSERT(so.handle() != NULL,
			"shared_object::open()");
		TEST_ASSERT(so.path() == "libdl.so",
			"shared_object::open()");

		so.close();
		TEST_ASSERT(so.handle() == NULL,
			"shared_object::close()");
		TEST_ASSERT(so.path() == "libdl.so",
			"shared_object::close()");
	}
}

void
test_lookup_symbol()
{
	// test lookup_symbol()
	{
		shared_object so("libdl.so");
		void *p = so.lookup_symbol("dlopen");
		TEST_ASSERT(p != NULL,
			"shared_object::lookup_symbol()");
		void *p2 = const_cast<void *>(so.handle());
		TEST_ASSERT(p == dlsym(p2, "dlopen"),
			"shared_object::lookup_symbol()");
	}
	{
		try {
			shared_object so("libdl.so");
			so.lookup_symbol("nonexistant");
			TEST_ERROR("shared_object::lookup_symbol()");
		} catch (shared_object::symbol_not_found_error &e) {
		}
	}
}

void
test_refcount()
{
	// test reference-counting
	{
		shared_object so("libdl.so");
		shared_object so2 = so;
		void *p;

		TEST_ASSERT(so.handle() != NULL,
			"shared_object::shared_object()");
		TEST_ASSERT(so2.handle() != NULL,
			"shared_object::shared_object()");
		p = so.lookup_symbol("dlopen");
		TEST_ASSERT(p != NULL,
			"shared_object::lookup_symbol()");
		p = so2.lookup_symbol("dlopen");
		TEST_ASSERT(p != NULL,
			"shared_object::lookup_symbol()");

		so.close();
		TEST_ASSERT(so.handle() == NULL,
			"shared_object::shared_object()");
		TEST_ASSERT(so2.handle() != NULL,
			"shared_object::shared_object()");
		try {
			p = so.lookup_symbol("dlopen");
			TEST_ERROR("shared_object::lookup_symbol()");
		} catch (shared_object::invalid_handle_error &e) {
		}
		p = so2.lookup_symbol("dlopen");
		TEST_ASSERT(p != NULL,
			"shared_object::lookup_symbol()");

		so2.close();
		TEST_ASSERT(so.handle() == NULL,
			"shared_object::shared_object()");
		TEST_ASSERT(so2.handle() == NULL,
			"shared_object::shared_object()");
		try {
			p = so.lookup_symbol("dlopen");
			TEST_ERROR("shared_object::lookup_symbol()");
		} catch (shared_object::invalid_handle_error &e) {
		}
		try {
			p = so2.lookup_symbol("dlopen");
			TEST_ERROR("shared_object::lookup_symbol()");
		} catch (shared_object::invalid_handle_error &e) {
		}
	}
}

TEST_LIST(
	TEST(test_ctors),
	TEST(test_open_close),
	TEST(test_lookup_symbol),
	TEST(test_refcount),
);
