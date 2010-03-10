//FIXME: fix include paths
#include "language/variable.h"
#include "pp/pp.h"
#include "pp/tests/pp_test.h"

TEST(test_type) {
	using pp::language::Type;

	// Test the class basics for primitive types.
	{
		Type t(Type::INT);

		TEST_ASSERT(t.primitive() == Type::INT);
		TEST_ASSERT(t.arguments().size() == 0);
		TEST_ASSERT(t.is_const() == false);
		t.set_const();
		TEST_ASSERT(t.is_const() == true);
		TEST_ASSERT(t.to_string() == "int");

		Type other_int(Type::INT);
		TEST_ASSERT(t.is_equal_to(&other_int));
		TEST_ASSERT(t == Type(Type::INT));
		TEST_ASSERT(t.is_assignable_from(&other_int));

		Type other_bool(Type::BOOL);
		TEST_ASSERT(!t.is_equal_to(&other_bool));
		TEST_ASSERT(!t.is_assignable_from(&other_bool));
		TEST_ASSERT(t != Type(Type::BOOL));

		try {
			Type bad(Type::INT);
			t.add_argument(&bad);
			TEST_ERROR();
		} catch (Type::ArgumentError &e) {
			// Success.
		} catch (...) {
			TEST_ERROR();
		}

		t.reinit(Type::STRING);
		TEST_ASSERT(t.primitive() == Type::STRING);
		TEST_ASSERT(t.to_string() == "string");
	}

	// Test the special behaviors of var types.
	{
		Type t(Type::VAR);

		TEST_ASSERT(t.primitive() == Type::VAR);
		TEST_ASSERT(t.to_string() == "var");

		Type other_var(Type::VAR);
		TEST_ASSERT(t.is_equal_to(&other_var));
		TEST_ASSERT(t == Type(Type::VAR));
		TEST_ASSERT(t.is_assignable_from(&other_var));

		Type other_bool(Type::BOOL);
		TEST_ASSERT(!t.is_equal_to(&other_bool));
		TEST_ASSERT(t.is_assignable_from(&other_bool));
		TEST_ASSERT(t != Type(Type::BOOL));
	}

	// Test the basics of list types.
	{
		Type t(Type::LIST);
		Type other_list(Type::LIST);

		TEST_ASSERT(t.primitive() == Type::LIST);
		TEST_ASSERT(t.is_equal_to(&other_list));
		TEST_ASSERT(t.is_assignable_from(&other_list));
		TEST_ASSERT(t.arguments().size() == 0);

		Type *arg = new Type(Type::INT);
		t.add_argument(arg);
		TEST_ASSERT(t.arguments().size() == 1);
		TEST_ASSERT(!t.is_equal_to(&other_list));

		try {
			Type *bad = new Type(Type::INT);
			t.add_argument(bad);
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
		t1.add_argument(new Type(Type::INT));

		Type t2(Type::LIST);
		t2.add_argument(new Type(Type::INT));

		TEST_ASSERT(t1.is_equal_to(&t2));
		TEST_ASSERT(t1.is_assignable_from(&t2));
		TEST_ASSERT(t2.is_assignable_from(&t1));
	}
	// list<int> = list<bool>
	{
		Type t1(Type::LIST);
		t1.add_argument(new Type(Type::INT));

		Type t2(Type::LIST);
		t2.add_argument(new Type(Type::BOOL));

		TEST_ASSERT(!t1.is_equal_to(&t2));
		TEST_ASSERT(!t1.is_assignable_from(&t2));
		TEST_ASSERT(!t2.is_assignable_from(&t1));
	}
	// list<var> = list<bool>
	{
		Type t1(Type::LIST);
		t1.add_argument(new Type(Type::VAR));

		Type t2(Type::LIST);
		t2.add_argument(new Type(Type::BOOL));

		TEST_ASSERT(!t1.is_equal_to(&t2));
		TEST_ASSERT(t1.is_assignable_from(&t2));
		TEST_ASSERT(!t2.is_assignable_from(&t1));
	}
	// list<list<int>> = list<list<int>>
	{
		Type t1(Type::LIST);
		Type *arg1 = new Type(Type::LIST);
		arg1->add_argument(new Type(Type::INT));
		t1.add_argument(arg1);

		Type t2(Type::LIST);
		Type *arg2 = new Type(Type::LIST);
		arg2->add_argument(new Type(Type::INT));
		t2.add_argument(arg2);

		TEST_ASSERT(t1.is_equal_to(&t2));
		TEST_ASSERT(t1.is_assignable_from(&t2));
		TEST_ASSERT(t2.is_assignable_from(&t1));
	}
	// list<list<int>> = list<list<bool>>
	{
		Type t1(Type::LIST);
		Type *arg1 = new Type(Type::LIST);
		arg1->add_argument(new Type(Type::INT));
		t1.add_argument(arg1);

		Type t2(Type::LIST);
		Type *arg2 = new Type(Type::LIST);
		arg2->add_argument(new Type(Type::BOOL));
		t2.add_argument(arg2);

		TEST_ASSERT(!t1.is_equal_to(&t2));
		TEST_ASSERT(!t1.is_assignable_from(&t2));
		TEST_ASSERT(!t2.is_assignable_from(&t1));
	}
	// list<list<var>> = list<list<bool>>
	{
		Type t1(Type::LIST);
		Type *arg1 = new Type(Type::LIST);
		arg1->add_argument(new Type(Type::VAR));
		t1.add_argument(arg1);

		Type t2(Type::LIST);
		Type *arg2 = new Type(Type::LIST);
		arg2->add_argument(new Type(Type::BOOL));
		t2.add_argument(arg2);

		TEST_ASSERT(!t1.is_equal_to(&t2));
		TEST_ASSERT(t1.is_assignable_from(&t2));
		TEST_ASSERT(!t2.is_assignable_from(&t1));
	}

	//
	// Test the specifics of tuple assignments.
	//

	// tuple<int> = tuple<int>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(new Type(Type::INT));

		Type t2(Type::TUPLE);
		t2.add_argument(new Type(Type::INT));

		TEST_ASSERT(t1.is_equal_to(&t2));
		TEST_ASSERT(t1.is_assignable_from(&t2));
		TEST_ASSERT(t2.is_assignable_from(&t1));
	}
	// tuple<int,int> = tuple<int,int>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(new Type(Type::INT));
		t1.add_argument(new Type(Type::INT));

		Type t2(Type::TUPLE);
		t2.add_argument(new Type(Type::INT));
		t2.add_argument(new Type(Type::INT));

		TEST_ASSERT(t1.is_equal_to(&t2));
		TEST_ASSERT(t1.is_assignable_from(&t2));
		TEST_ASSERT(t2.is_assignable_from(&t1));
	}
	// tuple<int> = tuple<bool>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(new Type(Type::INT));

		Type t2(Type::TUPLE);
		t2.add_argument(new Type(Type::BOOL));

		TEST_ASSERT(!t1.is_equal_to(&t2));
		TEST_ASSERT(!t1.is_assignable_from(&t2));
		TEST_ASSERT(!t2.is_assignable_from(&t1));
	}
	// tuple<int,int> = tuple<int,bool>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(new Type(Type::INT));
		t1.add_argument(new Type(Type::INT));

		Type t2(Type::TUPLE);
		t2.add_argument(new Type(Type::INT));
		t2.add_argument(new Type(Type::BOOL));

		TEST_ASSERT(!t1.is_equal_to(&t2));
		TEST_ASSERT(!t1.is_assignable_from(&t2));
		TEST_ASSERT(!t2.is_assignable_from(&t1));
	}
	// tuple<var> = tuple<bool>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(new Type(Type::VAR));

		Type t2(Type::TUPLE);
		t2.add_argument(new Type(Type::BOOL));

		TEST_ASSERT(!t1.is_equal_to(&t2));
		TEST_ASSERT(t1.is_assignable_from(&t2));
		TEST_ASSERT(!t2.is_assignable_from(&t1));
	}
	// tuple<int,var> = tuple<int,bool>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(new Type(Type::INT));
		t1.add_argument(new Type(Type::VAR));

		Type t2(Type::TUPLE);
		t2.add_argument(new Type(Type::INT));
		t2.add_argument(new Type(Type::BOOL));

		TEST_ASSERT(!t1.is_equal_to(&t2));
		TEST_ASSERT(t1.is_assignable_from(&t2));
		TEST_ASSERT(!t2.is_assignable_from(&t1));
	}
	// tuple<tuple<int>> = tuple<tuple<int>>
	{
		Type t1(Type::TUPLE);
		Type *arg1 = new Type(Type::TUPLE);
		arg1->add_argument(new Type(Type::INT));
		t1.add_argument(arg1);

		Type t2(Type::TUPLE);
		Type *arg2 = new Type(Type::TUPLE);
		arg2->add_argument(new Type(Type::INT));
		t2.add_argument(arg2);

		TEST_ASSERT(t1.is_equal_to(&t2));
		TEST_ASSERT(t1.is_assignable_from(&t2));
		TEST_ASSERT(t2.is_assignable_from(&t1));
	}
	// tuple<int,tuple<int,int>> = tuple<int,tuple<int,int>>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(new Type(Type::INT));
		Type *arg1 = new Type(Type::TUPLE);
		arg1->add_argument(new Type(Type::INT));
		arg1->add_argument(new Type(Type::INT));
		t1.add_argument(arg1);

		Type t2(Type::TUPLE);
		t2.add_argument(new Type(Type::INT));
		Type *arg2 = new Type(Type::TUPLE);
		arg2->add_argument(new Type(Type::INT));
		arg2->add_argument(new Type(Type::INT));
		t2.add_argument(arg2);

		TEST_ASSERT(t1.is_equal_to(&t2));
		TEST_ASSERT(t1.is_assignable_from(&t2));
		TEST_ASSERT(t2.is_assignable_from(&t1));
	}
	// tuple<tuple<int>> = tuple<tuple<bool>>
	{
		Type t1(Type::TUPLE);
		Type *arg1 = new Type(Type::TUPLE);
		arg1->add_argument(new Type(Type::INT));
		t1.add_argument(arg1);

		Type t2(Type::TUPLE);
		Type *arg2 = new Type(Type::TUPLE);
		arg2->add_argument(new Type(Type::BOOL));
		t2.add_argument(arg2);

		TEST_ASSERT(!t1.is_equal_to(&t2));
		TEST_ASSERT(!t1.is_assignable_from(&t2));
		TEST_ASSERT(!t2.is_assignable_from(&t1));
	}
	// tuple<int,tuple<int,int>> = tuple<int,tuple<int,bool>>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(new Type(Type::INT));
		Type *arg1 = new Type(Type::TUPLE);
		arg1->add_argument(new Type(Type::INT));
		arg1->add_argument(new Type(Type::INT));
		t1.add_argument(arg1);

		Type t2(Type::TUPLE);
		t2.add_argument(new Type(Type::INT));
		Type *arg2 = new Type(Type::TUPLE);
		arg2->add_argument(new Type(Type::INT));
		arg2->add_argument(new Type(Type::BOOL));
		t2.add_argument(arg2);

		TEST_ASSERT(!t1.is_equal_to(&t2));
		TEST_ASSERT(!t1.is_assignable_from(&t2));
		TEST_ASSERT(!t2.is_assignable_from(&t1));
	}
	// tuple<tuple<var>> = tuple<tuple<bool>>
	{
		Type t1(Type::TUPLE);
		Type *arg1 = new Type(Type::TUPLE);
		arg1->add_argument(new Type(Type::VAR));
		t1.add_argument(arg1);

		Type t2(Type::TUPLE);
		Type *arg2 = new Type(Type::TUPLE);
		arg2->add_argument(new Type(Type::BOOL));
		t2.add_argument(arg2);

		TEST_ASSERT(!t1.is_equal_to(&t2));
		TEST_ASSERT(t1.is_assignable_from(&t2));
		TEST_ASSERT(!t2.is_assignable_from(&t1));
	}
	// tuple<int,tuple<int,var>> = tuple<int,tuple<int,bool>>
	{
		Type t1(Type::TUPLE);
		t1.add_argument(new Type(Type::INT));
		Type *arg1 = new Type(Type::TUPLE);
		arg1->add_argument(new Type(Type::INT));
		arg1->add_argument(new Type(Type::VAR));
		t1.add_argument(arg1);

		Type t2(Type::TUPLE);
		t2.add_argument(new Type(Type::INT));
		Type *arg2 = new Type(Type::TUPLE);
		arg2->add_argument(new Type(Type::INT));
		arg2->add_argument(new Type(Type::BOOL));
		t2.add_argument(arg2);

		TEST_ASSERT(!t1.is_equal_to(&t2));
		TEST_ASSERT(t1.is_assignable_from(&t2));
		TEST_ASSERT(!t2.is_assignable_from(&t1));
	}
}

#if 0
TEST(test_basics) {
	using pp::language::Type;
	using pp::language::Variable;
	{
		//FIXME: left off here
		pp::language::Variable v(Type::BOOL);
		TEST_ASSERT(v.type() == pp::language::Variable::TYPE_BOOL);
		TEST_ASSERT(v.bool_value() == true);
		v.set_bool_value(false);
		TEST_ASSERT(v.bool_value() == false);
		v.set_bool_value(true);
		TEST_ASSERT(v.bool_value() == true);
	}
	{
		pp::language::Variable v(pp::Value(0));
		TEST_ASSERT(v.type() == pp::language::Variable::TYPE_INT);
		TEST_ASSERT(v.int_value() == 0);
		v.set_int_value(pp::Value(8675309));
		TEST_ASSERT(v.int_value() == 8675309);
		v.set_int_value(pp::Value(-1));
		TEST_ASSERT(v.int_value() == -1);
	}
	{
		pp::language::Variable v(string(""));
		TEST_ASSERT(v.type() == pp::language::Variable::TYPE_STRING);
		TEST_ASSERT(v.string_value() == "");
		v.set_string_value("abc");
		TEST_ASSERT(v.string_value() == "abc");
		v.set_string_value(string("123"));
		TEST_ASSERT(v.string_value() == "123");
	}
}

TEST(test_type_safety) {
	{
		pp::language::Variable v(true);
		try {
			v.int_value();
			TEST_FAIL();
		} catch (pp::language::Variable::TypeError &e) {
		}
		try {
			v.string_value();
			TEST_FAIL();
		} catch (pp::language::Variable::TypeError &e) {
		}
	}
	{
		pp::language::Variable v(pp::Value(0));
		try {
			v.bool_value();
			TEST_FAIL();
		} catch (pp::language::Variable::TypeError &e) {
		}
		try {
			v.string_value();
			TEST_FAIL();
		} catch (pp::language::Variable::TypeError &e) {
		}
	}
	{
		pp::language::Variable v(string(""));
		try {
			v.bool_value();
			TEST_FAIL();
		} catch (pp::language::Variable::TypeError &e) {
		}
		try {
			v.int_value();
			TEST_FAIL();
		} catch (pp::language::Variable::TypeError &e) {
		}
	}
}
#endif
