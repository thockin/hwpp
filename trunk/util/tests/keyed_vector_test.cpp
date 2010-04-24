#include "pp/util/keyed_vector.h"
#include <iostream>
#include "pp/util/test.h"

namespace util {

template<class Tkey, class Tval>
void dump_keyed_vector(const KeyedVector<Tkey, Tval> &kv, std::ostream &out) {
	for (size_t i = 0; i < kv.size(); i++) {
		out << kv.key_at(i) << " = " << kv.at(i) << std::endl;
	}
}

TEST(test_ctors)
{
	typedef KeyedVector<string, int> StringIntKeyvec;

	// default ctor
	StringIntKeyvec keyvec;
	if (keyvec.size() != 0) {
		TEST_FAIL("KeyedVector::KeyedVector()");
	}

	// copy ctor
	StringIntKeyvec *kvp;
	{
		StringIntKeyvec keyvec;
		keyvec.insert("one", 1);
		keyvec.insert("two", 2);
		kvp = new StringIntKeyvec(keyvec);
		if (kvp->size() != 2) {
			TEST_FAIL("KeyedVector::KeyedVector(KeyedVector)");
		}
	}
	if (kvp->size() != 2) {
		TEST_FAIL("KeyedVector::KeyedVector(KeyedVector)");
	}
	if (kvp->key_at(0) != "one") {
		TEST_FAIL("KeyedVector::KeyedVector(KeyedVector)");
	}

	// assignment
	{
		StringIntKeyvec keyvec;
		keyvec.insert("one", 1);
		keyvec.insert("two", 2);
		if (keyvec.size() != 2) {
			TEST_FAIL("KeyedVector::operator=(KeyedVector)");
		}

		StringIntKeyvec keyvec2;
		keyvec2.insert("three", 3);
		if (keyvec2.size() != 1) {
			TEST_FAIL("KeyedVector::operator=(KeyedVector)");
		}

		keyvec2 = keyvec;
		if (keyvec2.size() != 2) {
			TEST_FAIL("KeyedVector::operator=(KeyedVector)");
		}
	}
}

TEST(test_exceptions)
{
	typedef KeyedVector<string, int> StringIntKeyvec;

	StringIntKeyvec keyvec;
	if (keyvec.size() != 0) {
		TEST_FAIL("KeyedVector::size()");
	}

	try {
		keyvec.at(0);
		TEST_FAIL("KeyedVector::at(int)");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec[0];
		TEST_FAIL("KeyedVector::operator[](int)");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.key_at(0);
		TEST_FAIL("KeyedVector::key_at(int)");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.at("foo");
		TEST_FAIL("KeyedVector::at(Tkey)");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec["foo"];
		TEST_FAIL("KeyedVector::operator[](Tkey)");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.front();
		TEST_FAIL("KeyedVector::front()");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.back();
		TEST_FAIL("KeyedVector::back()");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.pop_front();
		TEST_FAIL("KeyedVector::pop_front()");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.pop_back();
		TEST_FAIL("KeyedVector::pop_back()");
	} catch (std::out_of_range &e) {
	}
}

TEST(test_int)
{
	typedef KeyedVector<string, int> StringIntKeyvec;
	StringIntKeyvec keyvec;
	const StringIntKeyvec &const_keyvec = keyvec;
	StringIntKeyvec::iterator it;

	// verify that it has the right size
	if (keyvec.size() != 0) {
		TEST_FAIL("KeyedVector::KeyedVector()");
	}
	if (!keyvec.empty()) {
		TEST_FAIL("KeyedVector::empty()");
	}

	// add some items
	keyvec.push_back("one", 1);
	keyvec.push_back("two", 2);
	keyvec.push_back("three", 3);

	// verify that it has the right size
	if (keyvec.size() != 3) {
		TEST_FAIL("KeyedVector::push_back()");
	}
	if (keyvec.empty()) {
		TEST_FAIL("KeyedVector::empty()");
	}

	// test simple iterator fetchers
	if (*keyvec.begin() != 1) {
		TEST_FAIL("KeyedVector::begin()");
	}
	if (*const_keyvec.begin() != 1) {
		TEST_FAIL("KeyedVector::begin()");
	}
	if (keyvec.end() != keyvec.begin()+3) {
		TEST_FAIL("KeyedVector::end()");
	}
	if (const_keyvec.end() != const_keyvec.begin()+3) {
		TEST_FAIL("KeyedVector::end()");
	}
	StringIntKeyvec::const_iterator cit = const_keyvec.begin();
	cit = keyvec.begin();
	//it = const_keyvec.begin(); // must fail
	//*cit = 1234; // must fail

	/* test reverse iterator fetchers */
	if (*keyvec.rbegin() != 3) {
		TEST_FAIL("KeyedVector::rbegin()");
	}
	if (*const_keyvec.rbegin() != 3) {
		TEST_FAIL("KeyedVector::rbegin()");
	}
	if (keyvec.rend() != keyvec.rbegin()+3) {
		TEST_FAIL("KeyedVector::rend()");
	}
	if (const_keyvec.rend() != const_keyvec.rbegin()+3) {
		TEST_FAIL("KeyedVector::rend()");
	}

	/* test forward iterator */
	int count = 0;
	it = keyvec.begin();
	while (it != keyvec.end()) {
		count++;
		it++;
	}
	if (count != 3) {
		TEST_FAIL("KeyedVector::iterator");
	}

	/* test simple int indexing and at() */
	if (keyvec[0] != 1) {
		TEST_FAIL("KeyedVector::operator[int]");
	}
	if (const_keyvec[0] != 1) {
		TEST_FAIL("const KeyedVector::operator[int]");
	}
	if (keyvec.at(0) != 1) {
		TEST_FAIL("KeyedVector::at(int)");
	}
	if (const_keyvec.at(0) != 1) {
		TEST_FAIL("const KeyedVector::at(int)");
	}

	/* test simple Tkey indexing and at() */
	if (keyvec["one"] != 1) {
		TEST_FAIL("KeyedVector::operator[Tkey]");
	}
	if (const_keyvec["one"] != 1) {
		TEST_FAIL("const KeyedVector::operator[Tkey]");
	}
	if (keyvec.at("one") != 1) {
		TEST_FAIL("KeyedVector::at(Tkey)");
	}
	if (const_keyvec.at("one") != 1) {
		TEST_FAIL("const KeyedVector::at(Tkey)");
	}

	/* test overwrites */
	if (keyvec["two"] != 2) {
		TEST_FAIL("KeyedVector::at(Tkey)");
	}
	keyvec.push_back("two", 22);
	if (keyvec["two"] != 22) {
		TEST_FAIL("KeyedVector::push_back()");
	}
	if (const_keyvec["two"] != 22) {
		TEST_FAIL("KeyedVector::push_back()");
	}

	/* test the has_key() method */
	if (!keyvec.has_key("one")) {
		TEST_FAIL("KeyedVector::has_key()");
	}
	if (!const_keyvec.has_key("one")) {
		TEST_FAIL("const KeyedVector::has_key()");
	}
	if (keyvec.has_key("four")) {
		TEST_FAIL("KeyedVector::has_key()");
	}

	/* test the find() method */
	it = keyvec.find("one");
	if (it == keyvec.end() || *it != 1) {
		TEST_FAIL("KeyedVector::find()");
	}
	it = keyvec.find("four");
	if (it != keyvec.end()) {
		TEST_FAIL("KeyedVector::find()");
	}

	/* test the key_at() method */
	if (keyvec.key_at(0) != "one") {
		TEST_FAIL("KeyedVector::key_at()");
	}
	// keyvec.key_at(0) = "four"; // must fail

	/* test writes through a subscript */
	keyvec[1] = 1234;
	if (keyvec[1] != 1234) {
		TEST_FAIL("KeyedVector::operator[int]");
	}
	keyvec["two"] = 5678;
	if (keyvec["two"] != 5678) {
		TEST_FAIL("KeyedVector::operator[Tkey]");
	}

	/* test erase() */
	keyvec.insert("four", 4);

	if (keyvec.at(3) != 4 || keyvec.at("four") != 4) {
		TEST_FAIL("KeyedVector::insert()");
	}
	if (keyvec.size() != 4) {
		TEST_FAIL("KeyedVector::insert()");
	}
	keyvec.erase(keyvec.begin());
	if (keyvec.size() != 3) {
		TEST_FAIL("KeyedVector::erase()");
	}

	/* erase should have adjusted everything */
	if (keyvec.at(2) != 4 || keyvec.at("four") != 4) {
		TEST_FAIL("KeyedVector::erase()");
	}

	/* test clear() */
	keyvec.clear();
	if (keyvec.size() != 0) {
		TEST_FAIL("KeyedVector::push_back()");
	}
}

class XyPair
{
    public:
	XyPair(int x, int y) {
		m_x = x;
		m_y = y;
	}
	int m_x;
	int m_y;
	bool operator==(const XyPair &that) const {
		return (this->m_x == that.m_x && this->m_y == that.m_y);
	}
	bool operator!=(const XyPair &that) const {
		return (this->m_x != that.m_x || this->m_y != that.m_y);
	}
};
std::ostream &operator<<(std::ostream &lhs, const XyPair &rhs)
{
	lhs << "(" << rhs.m_x << "," << rhs.m_y << ")";
	return lhs;
}

TEST(test_xypair)
{
	typedef KeyedVector<string, XyPair> StringXyPairKeyvec;
	StringXyPairKeyvec keyvec;
	const StringXyPairKeyvec &const_keyvec = keyvec;
	StringXyPairKeyvec::iterator it;

	XyPair zero(0,0);
	XyPair one(1, -1);
	XyPair two(2, 4);
	XyPair four(4, 20);

	// verify that it has the right size
	if (keyvec.size() != 0) {
		TEST_FAIL("KeyedVector::KeyedVector()");
	}
	if (!keyvec.empty()) {
		TEST_FAIL("KeyedVector::empty()");
	}

	// add some items
	keyvec.push_back("zero,zero", zero);
	keyvec.push_back("one,minusone", one);
	keyvec.push_back("two,four", two);

	// verify that it has the right size
	if (keyvec.size() != 3) {
		TEST_FAIL("KeyedVector::push_back()");
	}
	if (keyvec.empty()) {
		TEST_FAIL("KeyedVector::empty()");
	}

	// test simple iterator fetchers
	if (*keyvec.begin() != zero) {
		TEST_FAIL("KeyedVector::begin()");
	}
	if (*const_keyvec.begin() != zero) {
		TEST_FAIL("KeyedVector::begin()");
	}
	if (keyvec.end() != keyvec.begin()+3) {
		TEST_FAIL("KeyedVector::end()");
	}
	if (const_keyvec.end() != const_keyvec.begin()+3) {
		TEST_FAIL("KeyedVector::end()");
	}
	StringXyPairKeyvec::const_iterator cit = const_keyvec.begin();
	cit = keyvec.begin();

	/* test reverse iterator fetchers */
	if (*keyvec.rbegin() != two) {
		TEST_FAIL("KeyedVector::rbegin()");
	}
	if (*const_keyvec.rbegin() != two) {
		TEST_FAIL("KeyedVector::rbegin()");
	}
	if (keyvec.rend() != keyvec.rbegin()+3) {
		TEST_FAIL("KeyedVector::rend()");
	}
	if (const_keyvec.rend() != const_keyvec.rbegin()+3) {
		TEST_FAIL("KeyedVector::rend()");
	}

	/* test forward iterator */
	int count = 0;
	it = keyvec.begin();
	while (it != keyvec.end()) {
		count++;
		it++;
	}
	if (count != 3) {
		TEST_FAIL("KeyedVector::iterator");
	}

	/* test simple int indexing and at() */
	if (keyvec[0] != zero) {
		TEST_FAIL("KeyedVector::operator[int]");
	}
	if (const_keyvec[0] != zero) {
		TEST_FAIL("const KeyedVector::operator[int]");
	}
	if (keyvec.at(0) != zero) {
		TEST_FAIL("KeyedVector::at(int)");
	}
	if (const_keyvec.at(0) != zero) {
		TEST_FAIL("const KeyedVector::at(int)");
	}

	/* test simple Tkey indexing and at() */
	if (keyvec["zero,zero"] != zero) {
		TEST_FAIL("KeyedVector::operator[Tkey]");
	}
	if (const_keyvec["zero,zero"] != zero) {
		TEST_FAIL("const KeyedVector::operator[Tkey]");
	}
	if (keyvec.at("zero,zero") != zero) {
		TEST_FAIL("KeyedVector::at(Tkey)");
	}
	if (const_keyvec.at("zero,zero") != zero) {
		TEST_FAIL("const KeyedVector::at(Tkey)");
	}

	/* test overwrites */
	if (keyvec["two,four"] != two) {
		TEST_FAIL("KeyedVector::at(Tkey)");
	}
	keyvec.push_back("two,four", zero);
	if (keyvec["two,four"] != zero) {
		TEST_FAIL("KeyedVector::push_back()");
	}
	if (const_keyvec["two,four"] != zero) {
		TEST_FAIL("KeyedVector::push_back()");
	}

	/* test the has_key() method */
	if (!keyvec.has_key("one,minusone")) {
		TEST_FAIL("KeyedVector::has_key()");
	}
	if (!const_keyvec.has_key("one,minusone")) {
		TEST_FAIL("const KeyedVector::has_key()");
	}
	if (keyvec.has_key("four,score")) {
		TEST_FAIL("KeyedVector::has_key()");
	}

	/* test the find() method */
	it = keyvec.find("one,minusone");
	if (it == keyvec.end() || *it != one) {
		TEST_FAIL("KeyedVector::find()");
	}
	it = keyvec.find("four,score");
	if (it != keyvec.end()) {
		TEST_FAIL("KeyedVector::find()");
	}

	/* test the key_at() method */
	if (keyvec.key_at(0) != "zero,zero") {
		TEST_FAIL("KeyedVector::key_at()");
	}
	// keyvec.key_at(0) = "four,score"; // must fail

	/* test writes through a subscript */
	keyvec[1] = two;
	if (keyvec[1] != two) {
		TEST_FAIL("KeyedVector::operator[int]");
	}
	keyvec["two,four"] = zero;
	if (keyvec["two,four"] != zero) {
		TEST_FAIL("KeyedVector::operator[Tkey]");
	}

	/* test erase() */
	keyvec.insert("four,score", four);
	if (keyvec.size() != 4) {
		TEST_FAIL("KeyedVector::insert()");
	}
	keyvec.erase(keyvec.begin());
	if (keyvec.size() != 3) {
		TEST_FAIL("KeyedVector::erase()");
	}
}

} // namespace util
