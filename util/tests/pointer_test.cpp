#include "pp.h"
#include "util/pointer.h"
#include "util/test.h"

namespace util {

TEST(test_never_null_scoped_ptr)
{
	{
		//NeverNullScopedPtr<int> nnsp(NULL);  // should fail to compile
	}
	{
		int *p = NULL;
		try {
			NeverNullScopedPtr<int> nnsp(p);
		} catch (NullPointerError &e) {
			// Expected.
		} catch (...) {
			TEST_FAIL("NeverNullScopedPtr::NeverNullScopedPtr()");
		}
	}
	{
		NeverNullScopedPtr<int> nnsp(new int(93));
		TEST_ASSERT(*nnsp == 93);
		*nnsp = 76;
		TEST_ASSERT(*nnsp == 76);
		nnsp.reset(new int(42));
		TEST_ASSERT(*nnsp == 42);
		int *p = NULL;
		try {
			nnsp.reset(p);
		} catch (NullPointerError &e) {
			// Expected.
		} catch (...) {
			TEST_FAIL("NeverNullScopedPtr::reset()");
		}
	}
	{
		const int *p1 = new int(93);
		NeverNullScopedPtr<const int> nnsp(p1);
		TEST_ASSERT(*nnsp == 93);
		int *p2 = new int(76);
		nnsp.reset(p2);
		TEST_ASSERT(*nnsp == 76);
		const int *p3 = new int(42);
		nnsp.reset(p3);
		TEST_ASSERT(*nnsp == 42);
	}
	{
		NeverNullScopedPtr<int> nnsp1(new int(93));
		NeverNullScopedPtr<int> nnsp2(new int(76));
		TEST_ASSERT(*nnsp1 == 93);
		TEST_ASSERT(*nnsp2 == 76);
		nnsp1.swap(nnsp2);
		TEST_ASSERT(*nnsp1 == 76);
		TEST_ASSERT(*nnsp2 == 93);
		boost::scoped_ptr<int> sp(new int(42));
		//nnsp1.swap(sp);  // should fail to compile
	}
}

} // namespace util
