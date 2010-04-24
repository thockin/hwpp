//FIXME: fix include paths
#include "language/variable.h"
#include "pp/pp.h"
#include "pp/util/test.h"

TEST(test_type) {
	using pp::language::Type;

	// Test the class basics for primitive types.
	{
		Type t(Type::INT);

		TEST_ASSERT(t.primitive() == Type::INT);
		TEST_ASSERT(t.is_const() == false);
		TEST_ASSERT(t.arguments().size() == 0);
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

		t.set_const();
		TEST_ASSERT(t.is_const() == true);
		TEST_ASSERT(!t.is_assignable_from(&other_int));

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

TEST(test_datum) {
	using pp::language::Type;
	using pp::language::Variable;

	{
		Variable::Datum d(new Type(Type::INT));
		TEST_ASSERT(d.type()->primitive() == Type::INT);
		TEST_ASSERT(d.type()->arguments().size() == 0);
		Variable::Datum d_int(new Type(Type::INT));
		TEST_ASSERT(d.is_assignable_from(&d_int));
		Variable::Datum d_string(new Type(Type::STRING));
		TEST_ASSERT(!d.is_assignable_from(&d_string));
		TEST_ASSERT(!d.is_undef());
		TEST_ASSERT(d.int_value() == 0);
		try {
			d.bool_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			d.func_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			d.list_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			d.string_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			d.tuple_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
	}

	{
		Variable::Datum d1(new Type(Type::BOOL));
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.bool_value() == false);
		TEST_ASSERT(d2.bool_value() == false);
		TEST_ASSERT(d1.type() != d2.type());
	}

	{
		Variable::Datum d1(new Type(Type::INT));
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.int_value() == 0);
		TEST_ASSERT(d2.int_value() == 0);
		TEST_ASSERT(d1.type() != d2.type());
	}

	{
		Variable::Datum d1(new Type(Type::FUNC));
		Variable::Datum d2(d1);
		// There's no easy way to compare a func to a known value.
		TEST_ASSERT(d1.type() != d2.type());
	}

	{
		Type *t = new Type(Type::LIST);
		t->add_argument(new Type(Type::INT));
		Variable::Datum d1(t);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.list_value().contents().size() == 0);
		TEST_ASSERT(d2.list_value().contents().size() == 0);
		TEST_ASSERT(d1.type() != d2.type());
	}

	{
		Variable::Datum d1(new Type(Type::STRING));
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.string_value() == "");
		TEST_ASSERT(d2.string_value() == "");
		TEST_ASSERT(d1.type() != d2.type());
	}

	{
		Type *t = new Type(Type::TUPLE);
		t->add_argument(new Type(Type::INT));
		Variable::Datum d1(t);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.tuple_value().contents().size() == 0);
		TEST_ASSERT(d2.tuple_value().contents().size() == 0);
		TEST_ASSERT(d1.type() != d2.type());
	}

	{
		Variable::Datum d1(new Type(Type::VAR));
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.is_undef());
		TEST_ASSERT(d2.is_undef());
		try {
			d1.bool_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			d1.func_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			d1.int_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			d1.list_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			d1.string_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			d1.tuple_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		TEST_ASSERT(d1.type() != d2.type());
	}

	{
		Variable::Datum d1(new Type(Type::BOOL), true);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.bool_value() == true);
		TEST_ASSERT(d2.bool_value() == true);
	}

	{
		Variable::Datum d1(new Type(Type::INT), pp::Value(42));
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.int_value() == 42);
		TEST_ASSERT(d2.int_value() == 42);
	}

	{
		struct TestFunc : public Variable::Func {
			virtual Variable *call() {
				return NULL;
			}
		};
		TestFunc *func = new TestFunc();

		Variable::Datum d1(new Type(Type::FUNC), func);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.func_value() == func);
		TEST_ASSERT(d2.func_value() == func);
	}

	{
		Variable::List *list = new Variable::List();
		list->append(new Variable(new Type(Type::INT), pp::Value(42)));
		Type *type = new Type(Type::LIST);
		type->add_argument(new Type(Type::INT));
		Variable::Datum d1(type, list);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.list_value().contents().size() == 1);
		TEST_ASSERT(d1.list_value().contents()[0]->int_value() == 42);
		TEST_ASSERT(d2.list_value().contents().size() == 1);
		TEST_ASSERT(d2.list_value().contents()[0]->int_value() == 42);
	}

	{
		Variable::Datum d1(new Type(Type::STRING), string("forty two"));
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.string_value() == "forty two");
		TEST_ASSERT(d2.string_value() == "forty two");
	}

	{
		Variable::Tuple *tuple = new Variable::Tuple();
		tuple->append(new Variable(new Type(Type::INT), pp::Value(42)));
		Type *type = new Type(Type::TUPLE);
		type->add_argument(new Type(Type::INT));
		Variable::Datum d1(type, tuple);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.tuple_value().contents().size() == 1);
		TEST_ASSERT(d1.tuple_value().contents()[0]->int_value() == 42);
		TEST_ASSERT(d2.tuple_value().contents().size() == 1);
		TEST_ASSERT(d2.tuple_value().contents()[0]->int_value() == 42);
	}
}

TEST(test_basics) {
	using pp::language::Type;
	using pp::language::Variable;

	{
		Variable v(new Type(Type::BOOL));
		TEST_ASSERT(v.type()->primitive() == Type::BOOL);
		TEST_ASSERT(v.bool_value() == false);
		TEST_ASSERT(!v.is_const());
		try {
			v.int_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			v.func_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			v.list_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			v.string_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}
		try {
			v.tuple_value();
			TEST_ERROR();
		} catch (Variable::TypeError &e) {
		} catch (...) {
			TEST_ERROR();
		}

		Variable v2(new Type(Type::BOOL));
		TEST_ASSERT(v.is_assignable_from(&v2));
		TEST_ASSERT(v2.type()->primitive() == Type::BOOL);
		TEST_ASSERT(v2.bool_value() == false);
		TEST_ASSERT(!v2.is_const());

		Variable v3(v);
		TEST_ASSERT(v3.type()->primitive() == Type::BOOL);
		TEST_ASSERT(v3.bool_value() == false);
		TEST_ASSERT(!v3.is_const());
		TEST_ASSERT(v.value() == v3.value());

		Variable v4(v, Variable::CONST);
		TEST_ASSERT(v4.type()->primitive() == Type::BOOL);
		TEST_ASSERT(v4.bool_value() == false);
		TEST_ASSERT(v4.is_const());
	}

	{
		Variable v1(new Type(Type::BOOL), true);
		Variable v2(v1);
		TEST_ASSERT(v1.bool_value() == true);
		TEST_ASSERT(v2.bool_value() == true);

		Variable v3(new Type(Type::BOOL), false);
		TEST_ASSERT(v3.bool_value() == false);
		v3.assign_from(v1);
		TEST_ASSERT(v3.bool_value() == true);
	}

	{
		Variable v1(new Type(Type::INT), pp::Value(42));
		Variable v2(v1);
		TEST_ASSERT(v1.int_value() == 42);
		TEST_ASSERT(v2.int_value() == 42);

		Variable v3(new Type(Type::INT), pp::Value(93));
		TEST_ASSERT(v3.int_value() == 93);
		v3.assign_from(v1);
		TEST_ASSERT(v3.int_value() == 42);
	}

	{
		struct TestFunc : public Variable::Func {
			virtual Variable *call() {
				return NULL;
			}
		};
		TestFunc *func = new TestFunc();

		Variable v1(new Type(Type::FUNC), func);
		Variable v2(v1);
		TEST_ASSERT(v1.func_value() == func);
		TEST_ASSERT(v2.func_value() == func);
	}

	{
		Variable::List *list = new Variable::List();
		list->append(new Variable(new Type(Type::INT), pp::Value(42)));
		Type *type = new Type(Type::LIST);
		type->add_argument(new Type(Type::INT));
		Variable v1(type, list);
		Variable v2(v1);
		TEST_ASSERT(v1.list_value().contents().size() == 1);
		TEST_ASSERT(v1.list_value().contents()[0]->int_value() == 42);
		TEST_ASSERT(v2.list_value().contents().size() == 1);
		TEST_ASSERT(v2.list_value().contents()[0]->int_value() == 42);
	}

	{
		Variable v1(new Type(Type::STRING), string("forty two"));
		Variable v2(v1);
		TEST_ASSERT(v1.string_value() == "forty two");
		TEST_ASSERT(v2.string_value() == "forty two");

		Variable v3(new Type(Type::STRING), string("ninety three"));
		TEST_ASSERT(v3.string_value() == "ninety three");
		v3.assign_from(v1);
		TEST_ASSERT(v3.string_value() == "forty two");
	}

	{
		Variable::Tuple *tuple = new Variable::Tuple();
		tuple->append(new Variable(new Type(Type::INT), pp::Value(42)));
		Type *type = new Type(Type::TUPLE);
		type->add_argument(new Type(Type::INT));
		Variable v1(type, tuple);
		Variable v2(v1);
		TEST_ASSERT(v1.tuple_value().contents().size() == 1);
		TEST_ASSERT(v1.tuple_value().contents()[0]->int_value() == 42);
		TEST_ASSERT(v2.tuple_value().contents().size() == 1);
		TEST_ASSERT(v2.tuple_value().contents()[0]->int_value() == 42);
	}
}
