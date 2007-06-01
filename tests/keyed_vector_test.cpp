#include "keyed_vector.h"
#include <iostream>
using namespace std;

#define ERROR(msg) \
	cerr << "ERROR: [" __FILE__ << ":" << __LINE__ << "] " << msg << endl

template<class Tkey, class Tval>
void dump_keyed_vector(const keyed_vector<Tkey, Tval> &kv, ostream &out) {
	for (size_t i = 0; i < kv.size(); i++) {
		out << kv.key_at(i) << " = " << kv.at(i) << endl;
	}
}

int
test_keyed_vector_int() {
	int ret = 0;

	typedef keyed_vector<string, int> si_keyvec;
	si_keyvec keyvec;
	const si_keyvec &const_keyvec = keyvec;
	si_keyvec::iterator it;
	si_keyvec empty_keyvec;

	keyvec.push_back("three", 3);
	keyvec.push_back("two", 2);
	keyvec.push_back("one", 1);
	keyvec.push_back("three", 33);

	/* test simple iterator fetchers */
	if (*keyvec.begin() != 3) {
		ERROR("keyed_vector::begin()");
		ret++;
	}
	if (*const_keyvec.begin() != 3) {
		ERROR("keyed_vector::begin()");
		ret++;
	}
	if (keyvec.end() != keyvec.begin()+4) {
		ERROR("keyed_vector::end()");
		ret++;
	}
	if (const_keyvec.end() != const_keyvec.begin()+4) {
		ERROR("keyed_vector::end()");
		ret++;
	}
	si_keyvec::const_iterator cit = const_keyvec.begin();
	cit = keyvec.begin();
	// manual test: these should fail
	//it = const_keyvec.begin();
	//*cit = 1234;

	/* test reverse iterator fetchers */
	if (*keyvec.rbegin() != 33) {
		ERROR("keyed_vector::rbegin()");
		ret++;
	}
	if (*const_keyvec.rbegin() != 33) {
		ERROR("keyed_vector::rbegin()");
		ret++;
	}
	if (keyvec.rend() != keyvec.rbegin()+4) {
		ERROR("keyed_vector::rend()");
		ret++;
	}
	if (const_keyvec.rend() != const_keyvec.rbegin()+4) {
		ERROR("keyed_vector::rend()");
		ret++;
	}

	/* test forward iterator */
	int count = 0;
	it = keyvec.begin();
	while (it != keyvec.end()) {
		count++;
		it++;
	}
	if (count != 4) {
		ERROR("keyed_vector::iterator");
		ret++;
	}

	/* test size() and friends */
	if (keyvec.size() != 4) {
		ERROR("keyed_vector::size()");
		ret++;
	}
	if (empty_keyvec.size() != 0) {
		ERROR("keyed_vector::size()");
		ret++;
	}
	if (keyvec.capacity() != 4) {
		ERROR("keyed_vector::capacity()");
		ret++;
	}
	if (empty_keyvec.capacity() != 0) {
		ERROR("keyed_vector::capacity()");
		ret++;
	}

	/* test empty() */
	if (keyvec.empty()) {
		ERROR("keyed_vector::empty()");
		ret++;
	}
	if (!empty_keyvec.empty()) {
		ERROR("keyed_vector::empty()");
		ret++;
	}

	/* test pair_at() */
	const si_keyvec::pair_type &ckvpair = const_keyvec.pair_at(0);
	if (ckvpair.first != "three" || ckvpair.second != 3) {
		ERROR("keyed_vector::pair_at()");
		ret++;
	}
	//ckvpair.first = string("three");  // must fail
	//ckvpair.second = 3;  // must fail

	/* test simple int indexing and at() */
	if (keyvec[0] != 3) {
		ERROR("keyed_vector::operator[int]");
		ret++;
	}
	if (const_keyvec[0] != 3) {
		ERROR("const keyed_vector::operator[int]");
		ret++;
	}
	if (keyvec.at(0) != 3) {
		ERROR("keyed_vector::at(int)");
		ret++;
	}
	if (const_keyvec.at(0) != 3) {
		ERROR("const keyed_vector::at(int)");
		ret++;
	}

	/* test simple Tkey indexing and at() */
	if (keyvec["one"] != 1) {
		ERROR("keyed_vector::operator[string]");
		ret++;
	}
	if (const_keyvec["one"] != 1) {
		ERROR("const keyed_vector::operator[string]");
		ret++;
	}
	if (keyvec.at("one") != 1) {
		ERROR("keyed_vector::at(string)");
		ret++;
	}
	if (const_keyvec.at("one") != 1) {
		ERROR("const keyed_vector::at(string)");
		ret++;
	}

	/* test offset Tkey at() */
	it = keyvec.begin();
	if (keyvec.at("three", it) != 3) {
		ERROR("const keyed_vector::at(string, iterator)");
		ret++;
	}
	it++;
	if (keyvec.at("three", it) != 33) {
		ERROR("const keyed_vector::at(string, iterator)");
		ret++;
	}
	it++;
	try {
		keyvec.at("three", keyvec.end());
		ERROR("const keyed_vector::at(string, iterator)");
		ret++;
	} catch (std::out_of_range) {
		/* this is the expected path upon success */
	}

	/* test the has_key() method */
	if (!keyvec.has_key("one")) {
		ERROR("keyed_vector::has_key()");
		ret++;
	}
	if (!const_keyvec.has_key("one")) {
		ERROR("const keyed_vector::has_key()");
		ret++;
	}
	if (keyvec.has_key("four")) {
		ERROR("keyed_vector::has_key()");
		ret++;
	}

	/* test the find() method */
	it = keyvec.find("one");
	if (it == keyvec.end() || *it != 1) {
		ERROR("keyed_vector::find()");
		ret++;
	}
	it = keyvec.find("four");
	if (it != keyvec.end()) {
		ERROR("keyed_vector::find()");
		ret++;
	}
	it = keyvec.begin();
	if (!keyvec.has_key("three", it)) {
		ERROR("keyed_vector::has_key()");
		ret++;
	}
	it = keyvec.find("three", it);
	if (*it != 3) {
		ERROR("keyed_vector::find()");
		ret++;
	}
	it++;
	if (!keyvec.has_key("three", it)) {
		ERROR("keyed_vector::has_key()");
		ret++;
	}
	it = keyvec.find("three", it);
	if (*it != 33) {
		ERROR("keyed_vector::find()");
		ret++;
	}
	it++;
	if (keyvec.has_key("three", it)) {
		ERROR("keyed_vector::has_key()");
		ret++;
	}
	it = keyvec.find("three", it);
	if (it != keyvec.end()) {
		ERROR("keyed_vector::find()");
		ret++;
	}

	/* test the key_at() method */
	if (keyvec.key_at(0) != "three") {
		ERROR("keyed_vector::key_at()");
		ret++;
	}

	/* test writes through a subscript */
	keyvec[1] = 1234;
	if (keyvec[1] != 1234) {
		ERROR("keyed_vector::operator[int]");
		ret++;
	}
	keyvec["two"] = 5678;
	if (keyvec["two"] != 5678) {
		ERROR("keyed_vector::operator[Tkey]");
		ret++;
	}

	/* test erase() */
	keyvec.insert("four", 4);
	if (keyvec.size() != 5) {
		ERROR("keyed_vector::erase()");
		ret++;
	}
	keyvec.erase(keyvec.begin());
	if (keyvec.size() != 4) {
		ERROR("keyed_vector::erase()");
		ret++;
	}

	return ret;
}

int
test_keyed_vector_unique()
{
	int ret = 0;

	keyed_vector<string, int> keyvec;
	keyvec.insert("three", 3);
	keyvec.insert("two", 2);
	keyvec.insert("one", 1);

	/* test insert() of a duplicate key */
	keyvec.insert("two", 22);
	if (keyvec["two"] != 22) {
		ERROR("keyed_vector::insert()");
		ret++;
	}
	if (keyvec[1] != 22) {
		ERROR("keyed_vector::insert()");
		ret++;
	}

	/* test the remove() method */
	keyvec.insert("four", 4);
	if (!keyvec.has_key("four")) {
		ERROR("keyed_vector::insert()");
		ret++;
	}
	keyvec.remove("four");
	if (keyvec.has_key("four")) {
		ERROR("keyed_vector::remove()");
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
test_keyed_vector_xypair()
{
	int ret = 0;

	keyed_vector<string, xypair> keyvec;
	const keyed_vector<string, xypair> &const_keyvec = keyvec;
	keyed_vector<string, xypair>::iterator it;

	xypair zero(0,0);
	keyvec.insert("zero,zero", zero);
	xypair one(1, -1);
	keyvec.insert("one,minusone", one);
	xypair two(2, 4);
	keyvec.insert("two,four", two);

	/* test int indexing */
	if (keyvec[0] != zero) {
		ERROR("keyed_vector::operator[int]");
		ret++;
	}
	if (const_keyvec[0] != zero) {
		ERROR("const keyed_vector::operator[int]");
		ret++;
	}

	/* test string indexing */
	if (keyvec["one,minusone"] != one) {
		ERROR("keyed_vector::operator[string]");
		ret++;
	}
	if (const_keyvec["one,minusone"] != one) {
		ERROR("const keyed_vector::operator[string]");
		ret++;
	}

	/* test the has_key() method */
	if (!keyvec.has_key("zero,zero")) {
		ERROR("keyed_vector::has_key()");
		ret++;
	}
	if (!const_keyvec.has_key("zero,zero")) {
		ERROR("const keyed_vector::has_key()");
		ret++;
	}
	if (keyvec.has_key("doesn't have")) {
		ERROR("keyed_vector::has_key()");
		ret++;
	}

	/* test the find() method */
	it = keyvec.find("zero,zero");
	if (it == keyvec.end() || *it != zero) {
		ERROR("keyed_vector::find()");
		ret++;
	}
	it = keyvec.find("doesn't have");
	if (it != keyvec.end()) {
		ERROR("keyed_vector::find()");
		ret++;
	}
	/* test the key_at() method */
	if (keyvec.key_at(0) != "zero,zero") {
		ERROR("keyed_vector::key_at()");
		ret++;
	}
	/* test insert() of a duplicate key */
	keyvec.insert("two,four", xypair(4,2));
	if (keyvec["two,four"] == two) {
		ERROR("keyed_vector::insert()");
		ret++;
	}
	if (keyvec[1] == two) {
		ERROR("keyed_vector::insert()");
		ret++;
	}
	/* test the remove() method */
	xypair four(4,4);
	keyvec.insert("four,four", four);
	if (!keyvec.has_key("four,four")) {
		ERROR("keyed_vector::insert()");
		ret++;
	}
	keyvec.remove("four,four");
	if (keyvec.has_key("four,four")) {
		ERROR("keyed_vector::remove()");
		ret++;
	}
	/* test erase */
	keyvec.insert("four,four", four);
	if (keyvec.size() != 4) {
		ERROR("keyed_vector::erase()");
		ret++;
	}
	keyvec.erase(keyvec.begin());
	if (keyvec.size() != 3) {
		ERROR("keyed_vector::erase()");
		ret++;
	}

	/* test the iterator */
	int count = 0;
	it = keyvec.begin();
	while (it != keyvec.end()) {
		count++;
		it++;
	}
	if (count != 3) {
		ERROR("keyed_vector::iterator");
		ret++;
	}
	if (keyvec.size() != 3) {
		ERROR("keyed_vector::size()");
		ret++;
	}

	/* test writes through a subscript */
	xypair five(5,10);
	keyvec["one,minusone"] = five;
	if (keyvec["one,minusone"] != five) {
		ERROR("keyed_vector::operator[]");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r = test_keyed_vector_int();
	if (r) return EXIT_FAILURE;
	r = test_keyed_vector_unique();
	if (r) return EXIT_FAILURE;
	r = test_keyed_vector_xypair();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
