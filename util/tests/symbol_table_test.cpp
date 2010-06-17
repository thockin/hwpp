#include "util/symbol_table.h"
#include "util/test.h"

namespace util {

TEST(test_symbol_table)
{
	{
		SymbolTable<std::string, std::string> st;
		TEST_ASSERT(st.depth() == 1, "SymbolTable::SymbolTable()");
		TEST_ASSERT(st.context_size() == 0,
		            "SymbolTable::context_size()");
		TEST_ASSERT(st.pop_context() == false,
		            "SymbolTable::pop_context()");
		TEST_ASSERT(st.depth() == 1, "SymbolTable::pop_context()");

		st.push_context();
		TEST_ASSERT(st.depth() == 2, "SymbolTable::push_context()");
		TEST_ASSERT(st.context_size() == 0,
		            "SymbolTable::context_size()");
		TEST_ASSERT(st.pop_context() == true,
		            "SymbolTable::pop_context()");
		TEST_ASSERT(st.depth() == 1, "SymbolTable::pop_context()");

		const std::string *result;
		result = st.lookup("key1");
		TEST_ASSERT(result == NULL, "SymbolTable::lookup()");
		st.add("key1", "value1");
		TEST_ASSERT(st.context_size() == 1, "SymbolTable::add()");
		result = st.lookup("key1");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "value1", "SymbolTable::lookup()");

		st.add("key1", "value2");
		TEST_ASSERT(st.context_size() == 1, "SymbolTable::add()");
		result = st.lookup("key1");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "value2", "SymbolTable::lookup()");

		TEST_ASSERT(st.add_unique("key1", "value3") == false,
		            "SymbolTable::add_unique()");
		TEST_ASSERT(st.context_size() == 1,
		            "SymbolTable::add_unique()");
		TEST_ASSERT(st.add_unique("key2", "value1") == true,
		            "SymbolTable::add_unique()");
		TEST_ASSERT(st.context_size() == 2,
		            "SymbolTable::add_unique()");
		TEST_ASSERT(st.add_unique("key2", "value2") == false,
		            "SymbolTable::add_unique()");
		TEST_ASSERT(st.context_size() == 2,
		            "SymbolTable::add_unique()");
		result = st.lookup("key2");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "value1", "SymbolTable::lookup()");
	}

	{
		SymbolTable<std::string, std::string> st;
		const std::string *result;

		st.add("key1", "value1");
		st.add("key2", "value2");
		TEST_ASSERT(st.context_size() == 2, "SymbolTable::add()");

		st.push_context();
		TEST_ASSERT(st.depth() == 2, "SymbolTable::push_context()");
		result = st.lookup("key1");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "value1", "SymbolTable::lookup()");
		result = st.lookup("key2");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "value2", "SymbolTable::lookup()");

		st.add("key1", "sub-value1");
		result = st.lookup("key1");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "sub-value1", "SymbolTable::lookup()");
		result = st.lookup("key2");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "value2", "SymbolTable::lookup()");

		st.push_context();
		TEST_ASSERT(st.depth() == 3, "SymbolTable::push_context()");
		result = st.lookup("key1");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "sub-value1", "SymbolTable::lookup()");
		result = st.lookup("key2");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "value2", "SymbolTable::lookup()");

		st.add("key1", "sub-sub-value1");
		result = st.lookup("key1");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "sub-sub-value1",
		            "SymbolTable::lookup()");
		result = st.lookup("key2");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "value2", "SymbolTable::lookup()");

		st.pop_context();
		TEST_ASSERT(st.depth() == 2, "SymbolTable::push_context()");
		result = st.lookup("key1");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "sub-value1", "SymbolTable::lookup()");
		result = st.lookup("key2");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "value2", "SymbolTable::lookup()");

		st.pop_context();
		TEST_ASSERT(st.depth() == 1, "SymbolTable::push_context()");
		result = st.lookup("key1");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "value1", "SymbolTable::lookup()");
		result = st.lookup("key2");
		TEST_ASSERT(result != NULL, "SymbolTable::lookup()");
		TEST_ASSERT(*result == "value2", "SymbolTable::lookup()");
	}
}

}  // namespace util
