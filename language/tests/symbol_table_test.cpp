//FIXME: fix include paths
#include "symbol_table.h"
#include "variable.h"
#include "pp.h"
#include "util/test.h"

TEST(test_ctor) {
	pp::language::SymbolTable s;
	TEST_ASSERT(s.context_depth() == 1);
	TEST_ASSERT(s.context_size() == 0);
	TEST_ASSERT(s.lookup("does_not_exist") == NULL);
}

TEST(test_contexts) {
	pp::language::SymbolTable s;
	TEST_ASSERT(s.context_depth() == 1);
	TEST_ASSERT(s.context_size() == 0);

	s.push_context();
	TEST_ASSERT(s.context_depth() == 2);
	TEST_ASSERT(s.context_size() == 0);

	s.push_context();
	TEST_ASSERT(s.context_depth() == 3);
	TEST_ASSERT(s.context_size() == 0);

	s.pop_context();
	TEST_ASSERT(s.context_depth() == 2);

	s.pop_context();
	TEST_ASSERT(s.context_depth() == 1);

	// The context_depth should never go below 1.
	s.pop_context();
	TEST_ASSERT(s.context_depth() == 1);

	s.pop_context();
	TEST_ASSERT(s.context_depth() == 1);
}

TEST(test_add_and_lookup) {
	pp::language::SymbolTable s;
	TEST_ASSERT(s.context_size() == 0);

	pp::language::Variable *v;

	s.add("x", pp::language::Variable(true));
	TEST_ASSERT(s.context_size() == 1);
	v = s.lookup("x");
	TEST_ASSERT(v->type() == pp::language::Variable::TYPE_BOOL);
	TEST_ASSERT(v->bool_value() == true);

	s.add("y", pp::language::Variable(pp::Value(0)));
	TEST_ASSERT(s.context_size() == 2);
	v = s.lookup("y");
	TEST_ASSERT(v->type() == pp::language::Variable::TYPE_INT);
	TEST_ASSERT(v->int_value() == 0);

	s.add("z", pp::language::Variable(string("")));
	TEST_ASSERT(s.context_size() == 3);
	v = s.lookup("z");
	TEST_ASSERT(v->type() == pp::language::Variable::TYPE_STRING);
	TEST_ASSERT(v->string_value() == "");

	// Add another context.
	s.push_context();
	TEST_ASSERT(s.lookup("x") != NULL);
	TEST_ASSERT(s.lookup("y") != NULL);
	TEST_ASSERT(s.lookup("z") != NULL);

	// "a" exists in the new context only.
	s.add("a", pp::language::Variable(pp::Value(-1)));
	TEST_ASSERT(s.context_size() == 1);
	v = s.lookup("a");
	TEST_ASSERT(v->type() == pp::language::Variable::TYPE_INT);
	TEST_ASSERT(v->int_value() == -1);

	// This "x" shadows the previous context's "x".
	s.add("x", pp::language::Variable(pp::Value(93)));
	TEST_ASSERT(s.context_size() == 2);
	v = s.lookup("x");
	TEST_ASSERT(v->type() == pp::language::Variable::TYPE_INT);
	TEST_ASSERT(v->int_value() == 93);
	v->set_int_value(76);

	// "z" is still acessible.
	v = s.lookup("z");
	TEST_ASSERT(v->type() == pp::language::Variable::TYPE_STRING);
	TEST_ASSERT(v->string_value() == "");
	v->set_string_value("abc");

	// Go back to the first context.
	s.pop_context();

	// "a" no longer exists.
	TEST_ASSERT(s.lookup("a") == NULL);

	// "x" is still bool.
	v = s.lookup("x");
	TEST_ASSERT(v->type() == pp::language::Variable::TYPE_BOOL);
	TEST_ASSERT(v->bool_value() == true);

	// "z" has changed.
	v = s.lookup("z");
	TEST_ASSERT(v->type() == pp::language::Variable::TYPE_STRING);
	TEST_ASSERT(v->string_value() == "abc");
}
