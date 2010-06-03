#include "language/syntax_tree.h"
#include "pp.h"
#include "util/test.h"

#if 0
TEST(test_syntax_node) {
	using pp::language::syntax::SyntaxNode;

	// SyntaxNode is not directly usable, you have to inherit from it.
	class TestNode : public SyntaxNode {
	 public:
		TestNode() : SyntaxNode(TYPE_STATEMENT)
		{
		}
	};

	TestNode t;
	TEST_ASSERT_EQ(t.node_type(), SyntaxNode::TYPE_STATEMENT);
}

TEST(test_expression) {
	using pp::language::syntax::Expression;
	using pp::language::Type;
	using pp::language::Variable;

	// Expression is pure virtual.
	class TestExpression : public Expression {
	 public:
		void evaluate(Variable *out_result)
		{
			out_result->set_int_value(42);
		}
	};

	TestExpression t;
	Variable v(Type::VAR);
	t.evaluate(&v);
	TEST_ASSERT(v.int_value() == 42);
}
#else
TEST(test_failed) {
	TEST_FAIL("test has been commented out");
}
#endif
