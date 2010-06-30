// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:

//FIXME: fix include paths
#include "language/type.h"
#include "pp.h"
#include "util/test.h"

TEST(test_type) {
	using pp::language::Type;

	// Test the class basics for primitive types.
	{
		Type t(Type::INT);

		TEST_ASSERT(t.primitive() == Type::INT);
		TEST_ASSERT(t.is_const() == false);
		TEST_ASSERT(t.n_arguments() == 0);
		TEST_ASSERT(t.to_string() == "int");

		Type other_int(Type::INT);
		TEST_ASSERT(t.is_equal_to(other_int));
		TEST_ASSERT(t == Type(Type::INT));
		TEST_ASSERT(t.is_assignable_from(other_int));

		Type other_bool(Type::BOOL);
		TEST_ASSERT(!t.is_equal_to(other_bool));
		TEST_ASSERT(!t.is_assignable_from(other_bool));
		TEST_ASSERT(t != Type(Type::BOOL));

		try {
			Type bad(Type::INT);
			t.add_argument(bad);
			TEST_ERROR();
		} catch (Type::ArgumentError &e) {
			// Success.
		} catch (...) {
			TEST_ERROR();
		}

		t.set_const();
		TEST_ASSERT(t.is_const() == true);
		TEST_ASSERT(!t.is_assignable_from(other_int));

		t.reinit(Type::STRING);
		TEST_ASSERT(t.is_const() == false);
		TEST_ASSERT(t.primitive() == Type::STRING);
		TEST_ASSERT(t.to_string() == "string");
		t.set_const();
		TEST_ASSERT(t.to_string() == "const string");
	}

	// Test the special behaviors of var types.
	{
		Type t(Type::VAR);

		TEST_ASSERT(t.primitive() == Type::VAR);
		TEST_ASSERT(t.to_string() == "var");

		Type other_var(Type::VAR);
		TEST_ASSERT(t.is_equal_to(other_var));
		TEST_ASSERT(t == Type(Type::VAR));
		TEST_ASSERT(t.is_assignable_from(other_var));

		Type other_bool(Type::BOOL);
		TEST_ASSERT(!t.is_equal_to(other_bool));
		TEST_ASSERT(t.is_assignable_from(other_bool));
		TEST_ASSERT(t != Type(Type::BOOL));
	}

	// Test the basics of list types.
	{
		Type t(Type::LIST);
		Type other_list(Type::LIST);

		TEST_ASSERT(t.primitive() == Type::LIST);
		TEST_ASSERT(t.is_equal_to(other_list));
		TEST_ASSERT(t.is_assignable_from(other_list));
		TEST_ASSERT(t.n_arguments() == 0);

		t.add_argument(Type::INT);
		TEST_ASSERT(t.n_arguments() == 1);
		TEST_ASSERT(!t.is_equal_to(other_list));

		try {
			t.add_argument(Type::INT);
			TEST_ERROR();
		} catch (Type::ArgumentError &e) {
		} catch (...) {
			TEST_ERROR();
		}
	}

	//
	// Test the specifics of list assignments.
	//

	// list<int> = list<int>
	{
		Type t1(Type::LIST);
		t1.add_argument(Type::INT);

		Type t2(Type::LIST);
		t2.add_argument(Type::INT);

		TEST_ASSERT(t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(t2.is_assignable_from(t1));
	}
	// list<int> = list<bool>
	{
		Type t1(Type::LIST);
		t1.add_argument(Type::INT);

		Type t2(Type::LIST);
		t2.add_argument(Type::BOOL);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(!t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// list<var> = list<bool>
	{
		Type t1(Type::LIST);
		t1.add_argument(Type::VAR);

		Type t2(Type::LIST);
		t2.add_argument(Type::BOOL);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(t2.is_assignable_from(t1));
	}
	// list<int> = tuple<int>
	{
		Type t1(Type::LIST);
		t1.add_argument(Type::INT);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::INT);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// list<int> = tuple<int, int>
	{
		Type t1(Type::LIST);
		t1.add_argument(Type::INT);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::INT);
		t2.add_argument(Type::INT);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// list<int> = tuple<var, int>
	{
		Type t1(Type::LIST);
		t1.add_argument(Type::INT);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::VAR);
		t2.add_argument(Type::INT);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// list<var> = tuple<int, bool>
	{
		Type t1(Type::LIST);
		t1.add_argument(Type::VAR);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::INT);
		t2.add_argument(Type::BOOL);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// list<int> = tuple<bool>
	{
		Type t1(Type::LIST);
		t1.add_argument(Type::INT);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::BOOL);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(!t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// list<int> = tuple<int, bool>
	{
		Type t1(Type::LIST);
		t1.add_argument(Type::INT);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::INT);
		t2.add_argument(Type::BOOL);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(!t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// list<list<int>> = list<list<int>>
	{
		Type t1(Type::LIST);
		Type arg1(Type::LIST);
		arg1.add_argument(Type::INT);
		t1.add_argument(arg1);

		Type t2(Type::LIST);
		Type arg2(Type::LIST);
		arg2.add_argument(Type::INT);
		t2.add_argument(arg2);

		TEST_ASSERT(t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(t2.is_assignable_from(t1));
	}
	// list<list<int>> = list<list<bool>>
	{
		Type t1(Type::LIST);
		Type arg1(Type::LIST);
		arg1.add_argument(Type::INT);
		t1.add_argument(arg1);

		Type t2(Type::LIST);
		Type arg2(Type::LIST);
		arg2.add_argument(Type::BOOL);
		t2.add_argument(arg2);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(!t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// list<list<var>> = list<list<bool>>
	{
		Type t1(Type::LIST);
		Type arg1(Type::LIST);
		arg1.add_argument(Type::VAR);
		t1.add_argument(arg1);

		Type t2(Type::LIST);
		Type arg2(Type::LIST);
		arg2.add_argument(Type::BOOL);
		t2.add_argument(arg2);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(t2.is_assignable_from(t1));
	}

	//
	// Test the specifics of tuple assignments.
	//

	// tuple<int> = tuple<int>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(Type::INT);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::INT);

		TEST_ASSERT(t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(t2.is_assignable_from(t1));
	}
	// tuple<int,int> = tuple<int,int>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(Type::INT);
		t1.add_argument(Type::INT);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::INT);
		t2.add_argument(Type::INT);

		TEST_ASSERT(t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(t2.is_assignable_from(t1));
	}
	// tuple<int> = tuple<bool>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(Type::INT);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::BOOL);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(!t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// tuple<int,int> = tuple<int,bool>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(Type::INT);
		t1.add_argument(Type::INT);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::INT);
		t2.add_argument(Type::BOOL);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(!t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// tuple<int,bool> = tuple<int>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(Type::INT);
		t1.add_argument(Type::BOOL);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::INT);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// tuple<var> = tuple<bool>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(Type::VAR);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::BOOL);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(t2.is_assignable_from(t1));
	}
	// tuple<int,var> = tuple<int,bool>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(Type::INT);
		t1.add_argument(Type::VAR);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::INT);
		t2.add_argument(Type::BOOL);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(t2.is_assignable_from(t1));
	}
	// tuple<tuple<int>> = tuple<tuple<int>>
	{
		Type t1(Type::TUPLE);
		Type arg1(Type::TUPLE);
		arg1.add_argument(Type::INT);
		t1.add_argument(arg1);

		Type t2(Type::TUPLE);
		Type arg2(Type::TUPLE);
		arg2.add_argument(Type::INT);
		t2.add_argument(arg2);

		TEST_ASSERT(t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(t2.is_assignable_from(t1));
	}
	// tuple<int,tuple<int,int>> = tuple<int,tuple<int,int>>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(Type::INT);
		Type arg1(Type::TUPLE);
		arg1.add_argument(Type::INT);
		arg1.add_argument(Type::INT);
		t1.add_argument(arg1);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::INT);
		Type arg2(Type::TUPLE);
		arg2.add_argument(Type::INT);
		arg2.add_argument(Type::INT);
		t2.add_argument(arg2);

		TEST_ASSERT(t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(t2.is_assignable_from(t1));
	}
	// tuple<tuple<int>> = tuple<tuple<bool>>
	{
		Type t1(Type::TUPLE);
		Type arg1(Type::TUPLE);
		arg1.add_argument(Type::INT);
		t1.add_argument(arg1);

		Type t2(Type::TUPLE);
		Type arg2(Type::TUPLE);
		arg2.add_argument(Type::BOOL);
		t2.add_argument(arg2);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(!t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// tuple<int,tuple<int,int>> = tuple<int,tuple<int,bool>>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(Type::INT);
		Type arg1(Type::TUPLE);
		arg1.add_argument(Type::INT);
		arg1.add_argument(Type::INT);
		t1.add_argument(arg1);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::INT);
		Type arg2(Type::TUPLE);
		arg2.add_argument(Type::INT);
		arg2.add_argument(Type::BOOL);
		t2.add_argument(arg2);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(!t1.is_assignable_from(t2));
		TEST_ASSERT(!t2.is_assignable_from(t1));
	}
	// tuple<tuple<var>> = tuple<tuple<bool>>
	{
		Type t1(Type::TUPLE);
		Type arg1(Type::TUPLE);
		arg1.add_argument(Type::VAR);
		t1.add_argument(arg1);

		Type t2(Type::TUPLE);
		Type arg2(Type::TUPLE);
		arg2.add_argument(Type::BOOL);
		t2.add_argument(arg2);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(t2.is_assignable_from(t1));
	}
	// tuple<int,tuple<int,var>> = tuple<int,tuple<int,bool>>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(Type::INT);
		Type arg1(Type::TUPLE);
		arg1.add_argument(Type::INT);
		arg1.add_argument(Type::VAR);
		t1.add_argument(arg1);

		Type t2(Type::TUPLE);
		t2.add_argument(Type::INT);
		Type arg2(Type::TUPLE);
		arg2.add_argument(Type::INT);
		arg2.add_argument(Type::BOOL);
		t2.add_argument(arg2);

		TEST_ASSERT(!t1.is_equal_to(t2));
		TEST_ASSERT(t1.is_assignable_from(t2));
		TEST_ASSERT(t2.is_assignable_from(t1));
	}
}
