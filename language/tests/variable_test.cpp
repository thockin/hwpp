// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:

//FIXME: fix include paths
#include "language/variable.h"
#include "pp.h"
#include "util/test.h"

TEST(test_datum) {
	using pp::language::Type;
	using pp::language::Variable;

	{
		Variable::Datum d(Type::INT);
		TEST_ASSERT(d.type().primitive() == Type::INT);
		TEST_ASSERT(d.type().n_arguments() == 0);
		Variable::Datum d_int(Type::INT);
		TEST_ASSERT(d.is_assignable_from(&d_int));
		Variable::Datum d_string(Type::STRING);
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
		Variable::Datum d1(Type::BOOL);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.bool_value() == false);
		TEST_ASSERT(d2.bool_value() == false);
		TEST_ASSERT(d1.type() == d2.type());
	}

	{
		Variable::Datum d1(Type::INT);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.int_value() == 0);
		TEST_ASSERT(d2.int_value() == 0);
		TEST_ASSERT(d1.type() == d2.type());
	}

	{
		Variable::Datum d1(Type::FUNC);
		Variable::Datum d2(d1);
		// There's no easy way to compare a func to a known value.
		TEST_ASSERT(d1.type() == d2.type());
	}

	{
		Type t(Type::LIST);
		t.add_argument(Type::INT);
		Variable::Datum d1(t);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.list_value().contents().size() == 0);
		TEST_ASSERT(d2.list_value().contents().size() == 0);
		TEST_ASSERT(d1.type() == d2.type());
	}

	{
		Variable::Datum d1(Type::STRING);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.string_value() == "");
		TEST_ASSERT(d2.string_value() == "");
		TEST_ASSERT(d1.type() == d2.type());
	}

	{
		Type t(Type::TUPLE);
		t.add_argument(Type::INT);
		Variable::Datum d1(t);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.tuple_value().contents().size() == 0);
		TEST_ASSERT(d2.tuple_value().contents().size() == 0);
		TEST_ASSERT(d1.type() == d2.type());
	}

	{
		Variable::Datum d1(Type::VAR);
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
		TEST_ASSERT(d1.type() == d2.type());
	}

	{
		Variable::Datum d1(Type::BOOL, true);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.bool_value() == true);
		TEST_ASSERT(d2.bool_value() == true);
	}

	{
		Variable::Datum d1(Type::INT, pp::Value(42));
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

		Variable::Datum d1(Type::FUNC, func);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.func_value() == func);
		TEST_ASSERT(d2.func_value() == func);
	}

	{
		Variable::List *list = new Variable::List();
		list->append(new Variable(Type::INT, pp::Value(42)));
		Type type(Type::LIST);
		type.add_argument(Type::INT);
		Variable::Datum d1(type, list);
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.list_value().contents().size() == 1);
		TEST_ASSERT(d1.list_value().contents()[0]->int_value() == 42);
		TEST_ASSERT(d2.list_value().contents().size() == 1);
		TEST_ASSERT(d2.list_value().contents()[0]->int_value() == 42);
	}

	{
		Variable::Datum d1(Type::STRING, string("forty two"));
		Variable::Datum d2(d1);
		TEST_ASSERT(d1.string_value() == "forty two");
		TEST_ASSERT(d2.string_value() == "forty two");
	}

	{
		Variable::Tuple *tuple = new Variable::Tuple();
		tuple->append(new Variable(Type::INT, pp::Value(42)));
		Type type(Type::TUPLE);
		type.add_argument(Type::INT);
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
		Variable v(Type::BOOL);
		TEST_ASSERT(v.type().primitive() == Type::BOOL);
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

		Variable v2(Type::BOOL);
		TEST_ASSERT(v.is_assignable_from(&v2));
		TEST_ASSERT(v2.type().primitive() == Type::BOOL);
		TEST_ASSERT(v2.bool_value() == false);
		TEST_ASSERT(!v2.is_const());

		Variable v3(v);
		TEST_ASSERT(v3.type().primitive() == Type::BOOL);
		TEST_ASSERT(v3.bool_value() == false);
		TEST_ASSERT(!v3.is_const());
		TEST_ASSERT(v.value() == v3.value());

		Variable v4(v, Type::CONST);
		TEST_ASSERT(v4.type().primitive() == Type::BOOL);
		TEST_ASSERT(v4.bool_value() == false);
		TEST_ASSERT(v4.is_const());
	}

	{
		Variable v1(Type::BOOL, true);
		Variable v2(v1);
		TEST_ASSERT(v1.bool_value() == true);
		TEST_ASSERT(v2.bool_value() == true);

		Variable v3(Type::BOOL, false);
		TEST_ASSERT(v3.bool_value() == false);
		v3.assign_from(v1);
		TEST_ASSERT(v3.bool_value() == true);
	}

	{
		Variable v1(Type::INT, pp::Value(42));
		Variable v2(v1);
		TEST_ASSERT(v1.int_value() == 42);
		TEST_ASSERT(v2.int_value() == 42);

		Variable v3(Type::INT, pp::Value(93));
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

		Variable v1(Type::FUNC, func);
		Variable v2(v1);
		TEST_ASSERT(v1.func_value() == func);
		TEST_ASSERT(v2.func_value() == func);
	}

	{
		Variable::List *list = new Variable::List();
		list->append(new Variable(Type::INT, pp::Value(42)));
		Type type(Type::LIST);
		type.add_argument(Type::INT);
		Variable v1(type, list);
		Variable v2(v1);
		TEST_ASSERT(v1.list_value().contents().size() == 1);
		TEST_ASSERT(v1.list_value().contents()[0]->int_value() == 42);
		TEST_ASSERT(v2.list_value().contents().size() == 1);
		TEST_ASSERT(v2.list_value().contents()[0]->int_value() == 42);
	}

	{
		Variable v1(Type::STRING, string("forty two"));
		Variable v2(v1);
		TEST_ASSERT(v1.string_value() == "forty two");
		TEST_ASSERT(v2.string_value() == "forty two");

		Variable v3(Type::STRING, string("ninety three"));
		TEST_ASSERT(v3.string_value() == "ninety three");
		v3.assign_from(v1);
		TEST_ASSERT(v3.string_value() == "forty two");
	}

	{
		Variable::Tuple *tuple = new Variable::Tuple();
		tuple->append(new Variable(Type::INT, pp::Value(42)));
		Type type(Type::TUPLE);
		type.add_argument(Type::INT);
		Variable v1(type, tuple);
		Variable v2(v1);
		TEST_ASSERT(v1.tuple_value().contents().size() == 1);
		TEST_ASSERT(v1.tuple_value().contents()[0]->int_value() == 42);
		TEST_ASSERT(v2.tuple_value().contents().size() == 1);
		TEST_ASSERT(v2.tuple_value().contents()[0]->int_value() == 42);
	}
}
