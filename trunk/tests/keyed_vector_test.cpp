#include "keyed_vector.h"
#include "pp_test.h"
#include <iostream>
using namespace std;

template<class Tkey, class Tval>
void dump_keyed_vector(const keyed_vector<Tkey, Tval> &kv, ostream &out) {
	for (size_t i = 0; i < kv.size(); i++) {
		out << kv.key_at(i) << " = " << kv.at(i) << endl;
	}
}

void
test_ctors()
{
	typedef keyed_vector<string, int> si_keyvec;

	// default ctor
	si_keyvec keyvec;
	if (keyvec.size() != 0) {
		TEST_ERROR("keyed_vector::keyed_vector()");
	}

	// copy ctor
	si_keyvec *kvp;
	{
		si_keyvec keyvec;
		keyvec.insert("one", 1);
		keyvec.insert("two", 2);
		kvp = new si_keyvec(keyvec);
		if (kvp->size() != 2) {
			TEST_ERROR("keyed_vector::keyed_vector(keyed_vector)");
		}
	}
	if (kvp->size() != 2) {
		TEST_ERROR("keyed_vector::keyed_vector(keyed_vector)");
	}
	if (kvp->key_at(0) != "one") {
		TEST_ERROR("keyed_vector::keyed_vector(keyed_vector)");
	}

	// assignment
	{
		si_keyvec keyvec;
		keyvec.insert("one", 1);
		keyvec.insert("two", 2);
		if (keyvec.size() != 2) {
			TEST_ERROR("keyed_vector::opersator=(keyed_vector)");
		}

		si_keyvec keyvec2;
		keyvec2.insert("three", 3);
		if (keyvec2.size() != 1) {
			TEST_ERROR("keyed_vector::opersator=(keyed_vector)");
		}

		keyvec2 = keyvec;
		if (keyvec2.size() != 2) {
			TEST_ERROR("keyed_vector::opersator=(keyed_vector)");
		}
	}
}

void
test_exceptions()
{
	typedef keyed_vector<string, int> si_keyvec;

	si_keyvec keyvec;
	if (keyvec.size() != 0) {
		TEST_ERROR("keyed_vector::size()");
	}

	try {
		keyvec.at(0);
		TEST_ERROR("keyed_vector::at(int)");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec[0];
		TEST_ERROR("keyed_vector::operator[](int)");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.key_at(0);
		TEST_ERROR("keyed_vector::key_at(int)");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.at("foo");
		TEST_ERROR("keyed_vector::at(Tkey)");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec["foo"];
		TEST_ERROR("keyed_vector::operator[](Tkey)");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.front();
		TEST_ERROR("keyed_vector::front()");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.back();
		TEST_ERROR("keyed_vector::back()");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.pop_front();
		TEST_ERROR("keyed_vector::pop_front()");
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.pop_back();
		TEST_ERROR("keyed_vector::pop_back()");
	} catch (std::out_of_range &e) {
	}
}

void
test_int()
{
	typedef keyed_vector<string, int> si_keyvec;
	si_keyvec keyvec;
	const si_keyvec &const_keyvec = keyvec;
	si_keyvec::iterator it;

	// verify that it has the right size
	if (keyvec.size() != 0) {
		TEST_ERROR("keyed_vector::keyed_vector()");
	}
	if (!keyvec.empty()) {
		TEST_ERROR("keyed_vector::empty()");
	}

	// add some items
	keyvec.push_back("one", 1);
	keyvec.push_back("two", 2);
	keyvec.push_back("three", 3);

	// verify that it has the right size
	if (keyvec.size() != 3) {
		TEST_ERROR("keyed_vector::push_back()");
	}
	if (keyvec.empty()) {
		TEST_ERROR("keyed_vector::empty()");
	}

	// test simple iterator fetchers
	if (*keyvec.begin() != 1) {
		TEST_ERROR("keyed_vector::begin()");
	}
	if (*const_keyvec.begin() != 1) {
		TEST_ERROR("keyed_vector::begin()");
	}
	if (keyvec.end() != keyvec.begin()+3) {
		TEST_ERROR("keyed_vector::end()");
	}
	if (const_keyvec.end() != const_keyvec.begin()+3) {
		TEST_ERROR("keyed_vector::end()");
	}
	si_keyvec::const_iterator cit = const_keyvec.begin();
	cit = keyvec.begin();
	//it = const_keyvec.begin(); // must fail
	//*cit = 1234; // must fail

	/* test reverse iterator fetchers */
	if (*keyvec.rbegin() != 3) {
		TEST_ERROR("keyed_vector::rbegin()");
	}
	if (*const_keyvec.rbegin() != 3) {
		TEST_ERROR("keyed_vector::rbegin()");
	}
	if (keyvec.rend() != keyvec.rbegin()+3) {
		TEST_ERROR("keyed_vector::rend()");
	}
	if (const_keyvec.rend() != const_keyvec.rbegin()+3) {
		TEST_ERROR("keyed_vector::rend()");
	}

	/* test forward iterator */
	int count = 0;
	it = keyvec.begin();
	while (it != keyvec.end()) {
		count++;
		it++;
	}
	if (count != 3) {
		TEST_ERROR("keyed_vector::iterator");
	}

	/* test simple int indexing and at() */
	if (keyvec[0] != 1) {
		TEST_ERROR("keyed_vector::operator[int]");
	}
	if (const_keyvec[0] != 1) {
		TEST_ERROR("const keyed_vector::operator[int]");
	}
	if (keyvec.at(0) != 1) {
		TEST_ERROR("keyed_vector::at(int)");
	}
	if (const_keyvec.at(0) != 1) {
		TEST_ERROR("const keyed_vector::at(int)");
	}

	/* test simple Tkey indexing and at() */
	if (keyvec["one"] != 1) {
		TEST_ERROR("keyed_vector::operator[Tkey]");
	}
	if (const_keyvec["one"] != 1) {
		TEST_ERROR("const keyed_vector::operator[Tkey]");
	}
	if (keyvec.at("one") != 1) {
		TEST_ERROR("keyed_vector::at(Tkey)");
	}
	if (const_keyvec.at("one") != 1) {
		TEST_ERROR("const keyed_vector::at(Tkey)");
	}

	/* test overwrites */
	if (keyvec["two"] != 2) {
		TEST_ERROR("keyed_vector::at(Tkey)");
	}
	keyvec.push_back("two", 22);
	if (keyvec["two"] != 22) {
		TEST_ERROR("keyed_vector::push_back()");
	}
	if (const_keyvec["two"] != 22) {
		TEST_ERROR("keyed_vector::push_back()");
	}

	/* test the has_key() method */
	if (!keyvec.has_key("one")) {
		TEST_ERROR("keyed_vector::has_key()");
	}
	if (!const_keyvec.has_key("one")) {
		TEST_ERROR("const keyed_vector::has_key()");
	}
	if (keyvec.has_key("four")) {
		TEST_ERROR("keyed_vector::has_key()");
	}

	/* test the find() method */
	it = keyvec.find("one");
	if (it == keyvec.end() || *it != 1) {
		TEST_ERROR("keyed_vector::find()");
	}
	it = keyvec.find("four");
	if (it != keyvec.end()) {
		TEST_ERROR("keyed_vector::find()");
	}

	/* test the key_at() method */
	if (keyvec.key_at(0) != "one") {
		TEST_ERROR("keyed_vector::key_at()");
	}
	// keyvec.key_at(0) = "four"; // must fail

	/* test writes through a subscript */
	keyvec[1] = 1234;
	if (keyvec[1] != 1234) {
		TEST_ERROR("keyed_vector::operator[int]");
	}
	keyvec["two"] = 5678;
	if (keyvec["two"] != 5678) {
		TEST_ERROR("keyed_vector::operator[Tkey]");
	}

	/* test erase() */
	keyvec.insert("four", 4);

	if (keyvec.at(3) != 4 || keyvec.at("four") != 4) {
		TEST_ERROR("keyed_vector::insert()");
	}
	if (keyvec.size() != 4) {
		TEST_ERROR("keyed_vector::insert()");
	}
	keyvec.erase(keyvec.begin());
	if (keyvec.size() != 3) {
		TEST_ERROR("keyed_vector::erase()");
	}

	/* erase should have adjusted everything */
	if (keyvec.at(2) != 4 || keyvec.at("four") != 4) {
		TEST_ERROR("keyed_vector::erase()");
	}

	/* test clear() */
	keyvec.clear();
	if (keyvec.size() != 0) {
		TEST_ERROR("keyed_vector::push_back()");
	}
}

class xypair {
    public:
	xypair(int x, int y) {
		m_x = x;
		m_y = y;
	}
	int m_x;
	int m_y;
	bool operator==(const xypair &that) const {
		return (this->m_x == that.m_x && this->m_y == that.m_y);
	}
	bool operator!=(const xypair &that) const {
		return (this->m_x != that.m_x || this->m_y != that.m_y);
	}
};
ostream &operator<<(ostream &lhs, const xypair &rhs)
{
	lhs << "(" << rhs.m_x << "," << rhs.m_y << ")";
	return lhs;
}

void
test_xypair()
{
	typedef keyed_vector<string, xypair> xy_keyvec;
	xy_keyvec keyvec;
	const xy_keyvec &const_keyvec = keyvec;
	xy_keyvec::iterator it;

	xypair zero(0,0);
	xypair one(1, -1);
	xypair two(2, 4);
	xypair four(4, 20);

	// verify that it has the right size
	if (keyvec.size() != 0) {
		TEST_ERROR("keyed_vector::keyed_vector()");
	}
	if (!keyvec.empty()) {
		TEST_ERROR("keyed_vector::empty()");
	}

	// add some items
	keyvec.push_back("zero,zero", zero);
	keyvec.push_back("one,minusone", one);
	keyvec.push_back("two,four", two);

	// verify that it has the right size
	if (keyvec.size() != 3) {
		TEST_ERROR("keyed_vector::push_back()");
	}
	if (keyvec.empty()) {
		TEST_ERROR("keyed_vector::empty()");
	}

	// test simple iterator fetchers
	if (*keyvec.begin() != zero) {
		TEST_ERROR("keyed_vector::begin()");
	}
	if (*const_keyvec.begin() != zero) {
		TEST_ERROR("keyed_vector::begin()");
	}
	if (keyvec.end() != keyvec.begin()+3) {
		TEST_ERROR("keyed_vector::end()");
	}
	if (const_keyvec.end() != const_keyvec.begin()+3) {
		TEST_ERROR("keyed_vector::end()");
	}
	xy_keyvec::const_iterator cit = const_keyvec.begin();
	cit = keyvec.begin();

	/* test reverse iterator fetchers */
	if (*keyvec.rbegin() != two) {
		TEST_ERROR("keyed_vector::rbegin()");
	}
	if (*const_keyvec.rbegin() != two) {
		TEST_ERROR("keyed_vector::rbegin()");
	}
	if (keyvec.rend() != keyvec.rbegin()+3) {
		TEST_ERROR("keyed_vector::rend()");
	}
	if (const_keyvec.rend() != const_keyvec.rbegin()+3) {
		TEST_ERROR("keyed_vector::rend()");
	}

	/* test forward iterator */
	int count = 0;
	it = keyvec.begin();
	while (it != keyvec.end()) {
		count++;
		it++;
	}
	if (count != 3) {
		TEST_ERROR("keyed_vector::iterator");
	}

	/* test simple int indexing and at() */
	if (keyvec[0] != zero) {
		TEST_ERROR("keyed_vector::operator[int]");
	}
	if (const_keyvec[0] != zero) {
		TEST_ERROR("const keyed_vector::operator[int]");
	}
	if (keyvec.at(0) != zero) {
		TEST_ERROR("keyed_vector::at(int)");
	}
	if (const_keyvec.at(0) != zero) {
		TEST_ERROR("const keyed_vector::at(int)");
	}

	/* test simple Tkey indexing and at() */
	if (keyvec["zero,zero"] != zero) {
		TEST_ERROR("keyed_vector::operator[Tkey]");
	}
	if (const_keyvec["zero,zero"] != zero) {
		TEST_ERROR("const keyed_vector::operator[Tkey]");
	}
	if (keyvec.at("zero,zero") != zero) {
		TEST_ERROR("keyed_vector::at(Tkey)");
	}
	if (const_keyvec.at("zero,zero") != zero) {
		TEST_ERROR("const keyed_vector::at(Tkey)");
	}

	/* test overwrites */
	if (keyvec["two,four"] != two) {
		TEST_ERROR("keyed_vector::at(Tkey)");
	}
	keyvec.push_back("two,four", zero);
	if (keyvec["two,four"] != zero) {
		TEST_ERROR("keyed_vector::push_back()");
	}
	if (const_keyvec["two,four"] != zero) {
		TEST_ERROR("keyed_vector::push_back()");
	}

	/* test the has_key() method */
	if (!keyvec.has_key("one,minusone")) {
		TEST_ERROR("keyed_vector::has_key()");
	}
	if (!const_keyvec.has_key("one,minusone")) {
		TEST_ERROR("const keyed_vector::has_key()");
	}
	if (keyvec.has_key("four,score")) {
		TEST_ERROR("keyed_vector::has_key()");
	}

	/* test the find() method */
	it = keyvec.find("one,minusone");
	if (it == keyvec.end() || *it != one) {
		TEST_ERROR("keyed_vector::find()");
	}
	it = keyvec.find("four,score");
	if (it != keyvec.end()) {
		TEST_ERROR("keyed_vector::find()");
	}

	/* test the key_at() method */
	if (keyvec.key_at(0) != "zero,zero") {
		TEST_ERROR("keyed_vector::key_at()");
	}
	// keyvec.key_at(0) = "four,score"; // must fail

	/* test writes through a subscript */
	keyvec[1] = two;
	if (keyvec[1] != two) {
		TEST_ERROR("keyed_vector::operator[int]");
	}
	keyvec["two,four"] = zero;
	if (keyvec["two,four"] != zero) {
		TEST_ERROR("keyed_vector::operator[Tkey]");
	}

	/* test erase() */
	keyvec.insert("four,score", four);
	if (keyvec.size() != 4) {
		TEST_ERROR("keyed_vector::insert()");
	}
	keyvec.erase(keyvec.begin());
	if (keyvec.size() != 3) {
		TEST_ERROR("keyed_vector::erase()");
	}
}

TEST_LIST(
	TEST(test_ctors),
	TEST(test_exceptions),
	TEST(test_int),
	TEST(test_xypair),
);
