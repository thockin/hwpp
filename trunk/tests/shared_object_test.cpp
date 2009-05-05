#include "shared_object.h"
#include "pp_test.h"

namespace util {

TEST(test_ctors)
{
	// test ctors
	{
		SharedObject so;
		TEST_ASSERT(so.handle() == NULL,
			"SharedObject::SharedObject()");
		TEST_ASSERT(so.path() == "",
			"SharedObject::SharedObject()");
	}
	{
		SharedObject so("libdl.so");
		TEST_ASSERT(so.handle() != NULL,
			"SharedObject::SharedObject(libdl.so)");
		TEST_ASSERT(so.path() == "libdl.so",
			"SharedObject::SharedObject(libdl.so)");
	}
	{
		SharedObject so("libdl.so", RTLD_NOW | RTLD_GLOBAL);
		TEST_ASSERT(so.handle() != NULL,
			"SharedObject::SharedObject(libdl.so)");
		TEST_ASSERT(so.path() == "libdl.so",
			"SharedObject::SharedObject(libdl.so)");
	}
	{
		try {
			SharedObject so("./nonexistant.so");
			TEST_FAIL("SharedObject::SharedObject(string)");
		} catch (SharedObject::load_error &e) {
		}
	}
	{
		SharedObject so("libdl.so");
		SharedObject so2(so);
		TEST_ASSERT(so.handle() != NULL,
			"SharedObject::SharedObject(string)");
		TEST_ASSERT(so.path() == "libdl.so",
			"SharedObject::SharedObject(string)");
		TEST_ASSERT(so2.handle() == so.handle(),
			"SharedObject::SharedObject(SharedObject)");
		TEST_ASSERT(so2.path() == "libdl.so",
			"SharedObject::SharedObject(lSharedObject)");
	}
	{
		SharedObject so("libdl.so");
		SharedObject so2;
		so2 = so;
		TEST_ASSERT(so.handle() != NULL,
			"SharedObject::SharedObject(string)");
		TEST_ASSERT(so.path() == "libdl.so",
			"SharedObject::SharedObject(string)");
		TEST_ASSERT(so2.handle() == so.handle(),
			"SharedObject::operator=(SharedObject)");
		TEST_ASSERT(so2.path() == "libdl.so",
			"SharedObject::operator=(lSharedObject)");
	}
}

TEST(test_open_close)
{
	{
		SharedObject so;

		so.open("libdl.so");
		TEST_ASSERT(so.handle() != NULL,
			"SharedObject::open()");
		TEST_ASSERT(so.path() == "libdl.so",
			"SharedObject::open()");

		so.close();
		TEST_ASSERT(so.handle() == NULL,
			"SharedObject::close()");
		TEST_ASSERT(so.path() == "libdl.so",
			"SharedObject::close()");
	}
}

TEST(test_lookup_symbol)
{
	// test lookup_symbol()
	{
		SharedObject so("libdl.so");
		void *p = so.lookup_symbol("dlopen");
		TEST_ASSERT(p != NULL,
			"SharedObject::lookup_symbol()");
		void *p2 = const_cast<void *>(so.handle());
		TEST_ASSERT(p == dlsym(p2, "dlopen"),
			"SharedObject::lookup_symbol()");
	}
	{
		try {
			SharedObject so("libdl.so");
			so.lookup_symbol("nonexistant");
			TEST_FAIL("SharedObject::lookup_symbol()");
		} catch (SharedObject::symbol_not_found_error &e) {
		}
	}
}

TEST(test_refcount)
{
	// test reference-counting
	{
		SharedObject so("libdl.so");
		SharedObject so2 = so;
		void *p;

		TEST_ASSERT(so.handle() != NULL,
			"SharedObject::SharedObject()");
		TEST_ASSERT(so2.handle() != NULL,
			"SharedObject::SharedObject()");
		p = so.lookup_symbol("dlopen");
		TEST_ASSERT(p != NULL,
			"SharedObject::lookup_symbol()");
		p = so2.lookup_symbol("dlopen");
		TEST_ASSERT(p != NULL,
			"SharedObject::lookup_symbol()");

		so.close();
		TEST_ASSERT(so.handle() == NULL,
			"SharedObject::SharedObject()");
		TEST_ASSERT(so2.handle() != NULL,
			"SharedObject::SharedObject()");
		try {
			p = so.lookup_symbol("dlopen");
			TEST_FAIL("SharedObject::lookup_symbol()");
		} catch (SharedObject::invalid_handle_error &e) {
		}
		p = so2.lookup_symbol("dlopen");
		TEST_ASSERT(p != NULL,
			"SharedObject::lookup_symbol()");

		so2.close();
		TEST_ASSERT(so.handle() == NULL,
			"SharedObject::SharedObject()");
		TEST_ASSERT(so2.handle() == NULL,
			"SharedObject::SharedObject()");
		try {
			p = so.lookup_symbol("dlopen");
			TEST_FAIL("SharedObject::lookup_symbol()");
		} catch (SharedObject::invalid_handle_error &e) {
		}
		try {
			p = so2.lookup_symbol("dlopen");
			TEST_FAIL("SharedObject::lookup_symbol()");
		} catch (SharedObject::invalid_handle_error &e) {
		}
	}
}

} // namespace util
