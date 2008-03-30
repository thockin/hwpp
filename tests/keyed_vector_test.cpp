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

int
test_ctors()
{
	int ret = 0;
	typedef keyed_vector<string, int> si_keyvec;

	// default ctor
	si_keyvec keyvec;
	if (keyvec.size() != 0) {
		TEST_ERROR("keyed_vector::keyed_vector()");
		ret++;
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
			ret++;
		}
	}
	if (kvp->size() != 2) {
		TEST_ERROR("keyed_vector::keyed_vector(keyed_vector)");
		ret++;
	}
	if (kvp->key_at(0) != "one") {
		TEST_ERROR("keyed_vector::keyed_vector(keyed_vector)");
		ret++;
	}

	// assignment
	{
		si_keyvec keyvec;
		keyvec.insert("one", 1);
		keyvec.insert("two", 2);
		if (keyvec.size() != 2) {
			TEST_ERROR("keyed_vector::opersator=(keyed_vector)");
			ret++;
		}

		si_keyvec keyvec2;
		keyvec2.insert("three", 3);
		if (keyvec2.size() != 1) {
			TEST_ERROR("keyed_vector::opersator=(keyed_vector)");
			ret++;
		}

		keyvec2 = keyvec;
		if (keyvec2.size() != 2) {
			TEST_ERROR("keyed_vector::opersator=(keyed_vector)");
			ret++;
		}
	}

	return ret;
}

int
test_exceptions()
{
	int ret = 0;
	typedef keyed_vector<string, int> si_keyvec;

	si_keyvec keyvec;
	if (keyvec.size() != 0) {
		TEST_ERROR("keyed_vector::size()");
		ret++;
	}

	try {
		keyvec.at(0);
		TEST_ERROR("keyed_vector::at(int)");
		ret++;
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec[0];
		TEST_ERROR("keyed_vector::operator[](int)");
		ret++;
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.key_at(0);
		TEST_ERROR("keyed_vector::key_at(int)");
		ret++;
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.at("foo");
		TEST_ERROR("keyed_vector::at(Tkey)");
		ret++;
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec["foo"];
		TEST_ERROR("keyed_vector::operator[](Tkey)");
		ret++;
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.front();
		TEST_ERROR("keyed_vector::front()");
		ret++;
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.back();
		TEST_ERROR("keyed_vector::back()");
		ret++;
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.pop_front();
		TEST_ERROR("keyed_vector::pop_front()");
		ret++;
	} catch (std::out_of_range &e) {
	}
	try {
		keyvec.pop_back();
		TEST_ERROR("keyed_vector::pop_back()");
		ret++;
	} catch (std::out_of_range &e) {
	}

	return ret;
}

int
test_int()
{
	int ret = 0;

	typedef keyed_vector<string, int> si_keyvec;
	si_keyvec keyvec;
	const si_keyvec &const_keyvec = keyvec;
	si_keyvec::iterator it;

	// verify that it has the right size
	if (keyvec.size() != 0) {
		TEST_ERROR("keyed_vector::keyed_vector()");
		ret++;
	}
	if (!keyvec.empty()) {
		TEST_ERROR("keyed_vector::empty()");
		ret++;
	}

	// add some items
	keyvec.push_back("one", 1);
	keyvec.push_back("two", 2);
	keyvec.push_back("three", 3);

	// verify that it has the right size
	if (keyvec.size() != 3) {
		TEST_ERROR("keyed_vector::push_back()");
		ret++;
	}
	if (keyvec.empty()) {
		TEST_ERROR("keyed_vector::empty()");
		ret++;
	}

	// test simple iterator fetchers
	if (*keyvec.begin() != 1) {
		TEST_ERROR("keyed_vector::begin()");
		ret++;
	}
	if (*const_keyvec.begin() != 1) {
		TEST_ERROR("keyed_vector::begin()");
		ret++;
	}
	if (keyvec.end() != keyvec.begin()+3) {
		TEST_ERROR("keyed_vector::end()");
		ret++;
	}
	if (const_keyvec.end() != const_keyvec.begin()+3) {
		TEST_ERROR("keyed_vector::end()");
		ret++;
	}
	si_keyvec::const_iterator cit = const_keyvec.begin();
	cit = keyvec.begin();
	//it = const_keyvec.begin(); // must fail
	//*cit = 1234; // must fail

	/* test reverse iterator fetchers */
	if (*keyvec.rbegin() != 3) {
		TEST_ERROR("keyed_vector::rbegin()");
		ret++;
	}
	if (*const_keyvec.rbegin() != 3) {
		TEST_ERROR("keyed_vector::rbegin()");
		ret++;
	}
	if (keyvec.rend() != keyvec.rbegin()+3) {
		TEST_ERROR("keyed_vector::rend()");
		ret++;
	}
	if (const_keyvec.rend() != const_keyvec.rbegin()+3) {
		TEST_ERROR("keyed_vector::rend()");
		ret++;
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
		ret++;
	}

	/* test simple int indexing and at() */
	if (keyvec[0] != 1) {
		TEST_ERROR("keyed_vector::operator[int]");
		ret++;
	}
	if (const_keyvec[0] != 1) {
		TEST_ERROR("const keyed_vector::operator[int]");
		ret++;
	}
	if (keyvec.at(0) != 1) {
		TEST_ERROR("keyed_vector::at(int)");
		ret++;
	}
	if (const_keyvec.at(0) != 1) {
		TEST_ERROR("const keyed_vector::at(int)");
		ret++;
	}

	/* test simple Tkey indexing and at() */
	if (keyvec["one"] != 1) {
		TEST_ERROR("keyed_vector::operator[Tkey]");
		ret++;
	}
	if (const_keyvec["one"] != 1) {
		TEST_ERROR("const keyed_vector::operator[Tkey]");
		ret++;
	}
	if (keyvec.at("one") != 1) {
		TEST_ERROR("keyed_vector::at(Tkey)");
		ret++;
	}
	if (const_keyvec.at("one") != 1) {
		TEST_ERROR("const keyed_vector::at(Tkey)");
		ret++;
	}

	/* test overwrites */
	if (keyvec["two"] != 2) {
		TEST_ERROR("keyed_vector::at(Tkey)");
		ret++;
	}
	keyvec.push_back("two", 22);
	if (keyvec["two"] != 22) {
		TEST_ERROR("keyed_vector::push_back()");
		ret++;
	}
	if (const_keyvec["two"] != 22) {
		TEST_ERROR("keyed_vector::push_back()");
		ret++;
	}

	/* test the has_key() method */
	if (!keyvec.has_key("one")) {
		TEST_ERROR("keyed_vector::has_key()");
		ret++;
	}
	if (!const_keyvec.has_key("one")) {
		TEST_ERROR("const keyed_vector::has_key()");
		ret++;
	}
	if (keyvec.has_key("four")) {
		TEST_ERROR("keyed_vector::has_key()");
		ret++;
	}

	/* test the find() method */
	it = keyvec.find("one");
	if (it == keyvec.end() || *it != 1) {
		TEST_ERROR("keyed_vector::find()");
		ret++;
	}
	it = keyvec.find("four");
	if (it != keyvec.end()) {
		TEST_ERROR("keyed_vector::find()");
		ret++;
	}

	/* test the key_at() method */
	if (keyvec.key_at(0) != "one") {
		TEST_ERROR("keyed_vector::key_at()");
		ret++;
	}
	// keyvec.key_at(0) = "four"; // must fail

	/* test writes through a subscript */
	keyvec[1] = 1234;
	if (keyvec[1] != 1234) {
		TEST_ERROR("keyed_vector::operator[int]");
		ret++;
	}
	keyvec["two"] = 5678;
	if (keyvec["two"] != 5678) {
		TEST_ERROR("keyed_vector::operator[Tkey]");
		ret++;
	}

	/* test erase() */
	keyvec.insert("four", 4);

	if (keyvec.at(3) != 4 || keyvec.at("four") != 4) {
		TEST_ERROR("keyed_vector::insert()");
		ret++;
	}
	if (keyvec.size() != 4) {
		TEST_ERROR("keyed_vector::insert()");
		ret++;
	}
	keyvec.erase(keyvec.begin());
	if (keyvec.size() != 3) {
		TEST_ERROR("keyed_vector::erase()");
		ret++;
	}

	/* erase should have adjusted everything */
	if (keyvec.at(2) != 4 || keyvec.at("four") != 4) {
		TEST_ERROR("keyed_vector::erase()");
		ret++;
	}

	/* test clear() */
	keyvec.clear();
	if (keyvec.size() != 0) {
		TEST_ERROR("keyed_vector::push_back()");
		ret++;
	}

	return ret;
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

int
test_xypair()
{
	int ret = 0;

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
		ret++;
	}
	if (!keyvec.empty()) {
		TEST_ERROR("keyed_vector::empty()");
		ret++;
	}

	// add some items
	keyvec.push_back("zero,zero", zero);
	keyvec.push_back("one,minusone", one);
	keyvec.push_back("two,four", two);

	// verify that it has the right size
	if (keyvec.size() != 3) {
		TEST_ERROR("keyed_vector::push_back()");
		ret++;
	}
	if (keyvec.empty()) {
		TEST_ERROR("keyed_vector::empty()");
		ret++;
	}

	// test simple iterator fetchers
	if (*keyvec.begin() != zero) {
		TEST_ERROR("keyed_vector::begin()");
		ret++;
	}
	if (*const_keyvec.begin() != zero) {
		TEST_ERROR("keyed_vector::begin()");
		ret++;
	}
	if (keyvec.end() != keyvec.begin()+3) {
		TEST_ERROR("keyed_vector::end()");
		ret++;
	}
	if (const_keyvec.end() != const_keyvec.begin()+3) {
		TEST_ERROR("keyed_vector::end()");
		ret++;
	}
	xy_keyvec::const_iterator cit = const_keyvec.begin();
	cit = keyvec.begin();

	/* test reverse iterator fetchers */
	if (*keyvec.rbegin() != two) {
		TEST_ERROR("keyed_vector::rbegin()");
		ret++;
	}
	if (*const_keyvec.rbegin() != two) {
		TEST_ERROR("keyed_vector::rbegin()");
		ret++;
	}
	if (keyvec.rend() != keyvec.rbegin()+3) {
		TEST_ERROR("keyed_vector::rend()");
		ret++;
	}
	if (const_keyvec.rend() != const_keyvec.rbegin()+3) {
		TEST_ERROR("keyed_vector::rend()");
		ret++;
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
		ret++;
	}

	/* test simple int indexing and at() */
	if (keyvec[0] != zero) {
		TEST_ERROR("keyed_vector::operator[int]");
		ret++;
	}
	if (const_keyvec[0] != zero) {
		TEST_ERROR("const keyed_vector::operator[int]");
		ret++;
	}
	if (keyvec.at(0) != zero) {
		TEST_ERROR("keyed_vector::at(int)");
		ret++;
	}
	if (const_keyvec.at(0) != zero) {
		TEST_ERROR("const keyed_vector::at(int)");
		ret++;
	}

	/* test simple Tkey indexing and at() */
	if (keyvec["zero,zero"] != zero) {
		TEST_ERROR("keyed_vector::operator[Tkey]");
		ret++;
	}
	if (const_keyvec["zero,zero"] != zero) {
		TEST_ERROR("const keyed_vector::operator[Tkey]");
		ret++;
	}
	if (keyvec.at("zero,zero") != zero) {
		TEST_ERROR("keyed_vector::at(Tkey)");
		ret++;
	}
	if (const_keyvec.at("zero,zero") != zero) {
		TEST_ERROR("const keyed_vector::at(Tkey)");
		ret++;
	}

	/* test overwrites */
	if (keyvec["two,four"] != two) {
		TEST_ERROR("keyed_vector::at(Tkey)");
		ret++;
	}
	keyvec.push_back("two,four", zero);
	if (keyvec["two,four"] != zero) {
		TEST_ERROR("keyed_vector::push_back()");
		ret++;
	}
	if (const_keyvec["two,four"] != zero) {
		TEST_ERROR("keyed_vector::push_back()");
		ret++;
	}

	/* test the has_key() method */
	if (!keyvec.has_key("one,minusone")) {
		TEST_ERROR("keyed_vector::has_key()");
		ret++;
	}
	if (!const_keyvec.has_key("one,minusone")) {
		TEST_ERROR("const keyed_vector::has_key()");
		ret++;
	}
	if (keyvec.has_key("four,score")) {
		TEST_ERROR("keyed_vector::has_key()");
		ret++;
	}

	/* test the find() method */
	it = keyvec.find("one,minusone");
	if (it == keyvec.end() || *it != one) {
		TEST_ERROR("keyed_vector::find()");
		ret++;
	}
	it = keyvec.find("four,score");
	if (it != keyvec.end()) {
		TEST_ERROR("keyed_vector::find()");
		ret++;
	}

	/* test the key_at() method */
	if (keyvec.key_at(0) != "zero,zero") {
		TEST_ERROR("keyed_vector::key_at()");
		ret++;
	}
	// keyvec.key_at(0) = "four,score"; // must fail

	/* test writes through a subscript */
	keyvec[1] = two;
	if (keyvec[1] != two) {
		TEST_ERROR("keyed_vector::operator[int]");
		ret++;
	}
	keyvec["two,four"] = zero;
	if (keyvec["two,four"] != zero) {
		TEST_ERROR("keyed_vector::operator[Tkey]");
		ret++;
	}

	/* test erase() */
	keyvec.insert("four,score", four);
	if (keyvec.size() != 4) {
		TEST_ERROR("keyed_vector::insert()");
		ret++;
	}
	keyvec.erase(keyvec.begin());
	if (keyvec.size() != 3) {
		TEST_ERROR("keyed_vector::erase()");
		ret++;
	}

	return ret;
}

TEST_MAIN(
	TEST_FUNC(test_ctors),
	TEST_FUNC(test_exceptions),
	TEST_FUNC(test_int),
	TEST_FUNC(test_xypair),
);
