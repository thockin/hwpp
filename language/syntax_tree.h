// SyntaxNode tree support.

#ifndef PP_LANGAUGE_SYNTAX_TREE_H__
#define PP_LANGAUGE_SYNTAX_TREE_H__

// The grammar.l and grammar.y files include this header, which will screw
// them up if they include their own auto-generated headers, so don't include
// auto.*.h here.
#include "pp/pp.h"
#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include "pp/language/variable.h"

namespace pp {
namespace language {
namespace syntax {

// Base class for all nodes in our syntax tree.
class SyntaxNode {
    public:
	enum NodeType {
		TYPE_IDENTIFIER,    // a symbol name
		TYPE_EXPRESSION,    // an expression produces a value
		TYPE_STATEMENT,     // a statement executes
		TYPE_ARGUMENT,      // a named function call argument
	};

	NodeType node_type()
	{
		return m_node_type;
	}

	// Produce a string representation of this node.  For complex nodes
	// which contain other nodes, this will be recursive.
	virtual string to_string() const = 0;

    protected:
	SyntaxNode(NodeType node_type) : m_node_type(node_type)
	{
	}

	virtual ~SyntaxNode()
	{
	}

    private:
	NodeType m_node_type;
};

// Abstract base class for expressions which produce a value.
class Expression : public SyntaxNode {
    public:
	Expression() : SyntaxNode(TYPE_EXPRESSION)
	{
	}

	virtual ~Expression()
	{
	}

	// Evaluate this expression, producing a result.
	virtual void evaluate(Variable *out_result) = 0;
};
typedef std::vector<Expression*> ExpressionList;

class Identifier : public SyntaxNode {
    public:
	Identifier(const string &symbol)
	    : SyntaxNode(TYPE_IDENTIFIER), m_module(""), m_symbol(symbol)
	{
	}
	Identifier(const string &module, const string &symbol)
	    : SyntaxNode(TYPE_IDENTIFIER), m_module(module), m_symbol(symbol)
	{
	}

	const string &module() const
	{
		return m_module;
	}

	const string &symbol() const
	{
		return m_symbol;
	}

	virtual string to_string() const;

    private:
	string m_module;
	string m_symbol;
};

class InitializedIdentifier : public SyntaxNode {
    public:
	InitializedIdentifier(Identifier *ident)
	    : SyntaxNode(TYPE_IDENTIFIER), m_ident(ident), m_init(NULL)
	{
		DASSERT(ident);
	}
	InitializedIdentifier(Identifier *ident, Expression *init)
	    : SyntaxNode(TYPE_IDENTIFIER), m_ident(ident), m_init(init)
	{
		DASSERT(ident);
		DASSERT(init);
	}

	Identifier *identifier()
	{
		return m_ident.get();
	}

	Expression *initializer()
	{
		return m_init.get();
	}

	virtual string to_string() const;

    private:
	boost::scoped_ptr<Identifier> m_ident;
	boost::scoped_ptr<Expression> m_init;
};
typedef std::vector<InitializedIdentifier*> InitializedIdentifierList;

// Abstract base class for statements.
class Statement : public SyntaxNode {
    public:
	Statement() : SyntaxNode(TYPE_STATEMENT), m_labels(NULL)
	{
	}
	virtual ~Statement()
	{
		for (size_t i = 0; i < m_labels.size(); i++) {
			delete m_labels[i];
		}
	}

	// Execute this statement.
	virtual bool execute() = 0;

	// Get a string representation of this statement.
	virtual string to_string() const;

	std::vector<Identifier*> &labels()
	{
		return m_labels;
	}
	void add_label(Identifier *label)
	{
		m_labels.push_back(label);
	}

    private:
	std::vector<Identifier*> m_labels;
};
typedef std::vector<Statement*> StatementList;

class Argument : public SyntaxNode {
    public:
	Argument(Expression *expr)
	    : SyntaxNode(TYPE_ARGUMENT), m_name(NULL), m_expr(expr)
	{
		DASSERT(expr);
	}
	Argument(Identifier *name, Expression *expr)
	    : SyntaxNode(TYPE_ARGUMENT), m_name(name), m_expr(expr)
	{
		DASSERT(name);
		DASSERT(expr);
	}

	Identifier *name()
	{
		return m_name.get();
	}

	Expression *expression()
	{
		return m_expr.get();
	}

	virtual string to_string() const;

    private:
	boost::scoped_ptr<Identifier> m_name;
	boost::scoped_ptr<Expression> m_expr;
};
typedef std::vector<Argument*> ArgumentList;

typedef std::vector<Type*> TypeList;

class NullStatement : public Statement {
    public:
	NullStatement()
	{
	}

	virtual bool execute();

	virtual string to_string() const;
};

class CompoundStatement : public Statement {
    public:
	CompoundStatement(StatementList *body) : m_body(body)
	{
		DASSERT(body);
	}

	StatementList *body()
	{
		return m_body.get();
	}

	virtual bool execute();

	virtual string to_string() const;

    private:
	boost::scoped_ptr<StatementList> m_body;
};

// Both and expression and a statement.
class ExpressionStatement : public Expression, public Statement {
    public:
	ExpressionStatement() : m_expr(NULL)
	{
	}
	ExpressionStatement(Expression *expr) : m_expr(expr)
	{
		DASSERT(expr);
	}

	Expression *expression()
	{
		return m_expr.get();
	}

	virtual void evaluate(Variable *out_result)
	{
		m_expr->evaluate(out_result);
	}

	virtual bool execute();

	virtual string to_string() const;

    private:
	boost::scoped_ptr<Expression> m_expr;
};

class ConditionalStatement : public Statement {
    public:
	ConditionalStatement(Expression *condition, Statement *true_case)
	    : m_condition(condition), m_true(true_case), m_false(NULL)
	{
		DASSERT(condition);
		DASSERT(true_case);
	}
	ConditionalStatement(Expression *condition, Statement *true_case,
	                     Statement *false_case)
	    : m_condition(condition), m_true(true_case), m_false(false_case)
	{
		DASSERT(condition);
		DASSERT(true_case);
		DASSERT(false_case);
	}

	Expression *condition()
	{
		return m_condition.get();
	}

	Statement *true_case()
	{
		return m_true.get();
	}

	Statement *false_case()
	{
		return m_false.get();
	}

	virtual bool execute();

	virtual string to_string() const;

    private:
	boost::scoped_ptr<Expression> m_condition;
	boost::scoped_ptr<Statement> m_true;
	boost::scoped_ptr<Statement> m_false;
};

class SwitchStatement : public Statement {
    public:
	SwitchStatement(Expression *condition, Statement *body)
	    : m_condition(condition), m_body(body)
	{
		DASSERT(condition);
		DASSERT(body);
	}

	Expression *condition()
	{
		return m_condition.get();
	}

	Statement *body()
	{
		return m_body.get();
	}

	virtual bool execute();

	virtual string to_string() const;

    private:
	boost::scoped_ptr<Expression> m_condition;
	boost::scoped_ptr<Statement> m_body;
};

class LoopStatement : public Statement {
    public:
	enum LoopType {
		LOOP_WHILE,
		LOOP_DO_WHILE,
	};
	LoopStatement(LoopType loop_type, Expression *expr,
	                  Statement *body)
	    : m_loop_type(loop_type), m_expr(expr), m_body(body)
	{
		DASSERT(expr);
		DASSERT(body);
	}

	LoopType loop_type() const
	{
		return m_loop_type;
	}

	Expression *expression() const
	{
		return m_expr.get();
	}

	Statement *body() const
	{
		return m_body.get();
	}

	virtual bool execute();

    private:
	LoopType m_loop_type;
	boost::scoped_ptr<Expression> m_expr;
	boost::scoped_ptr<Statement> m_body;
};

class WhileLoopStatement : public LoopStatement {
    public:
	WhileLoopStatement(Expression *expr, Statement *body)
	    : LoopStatement(LOOP_WHILE, expr, body)
	{
		DASSERT(body);
		DASSERT(expr);
	}

	virtual string to_string() const;
};

class DoWhileLoopStatement : public LoopStatement {
    public:
	DoWhileLoopStatement(Statement *body, Expression *expr)
	    : LoopStatement(LOOP_DO_WHILE, expr, body)
	{
		DASSERT(body);
		DASSERT(expr);
	}

	virtual string to_string() const;
};

class GotoStatement : public Statement {
    public:
	GotoStatement(Identifier *target) : m_target(target)
	{
		DASSERT(target);
	}

	Identifier *target()
	{
		return m_target.get();
	}

	virtual bool execute();

	virtual string to_string() const;

    private:
	boost::scoped_ptr<Identifier> m_target;
};

class CaseStatement : public Statement {
    public:
	CaseStatement(Expression *expr, Statement *statement)
	    : m_expr(expr), m_statement(statement)
	{
		DASSERT(expr);
		DASSERT(statement);
	}

	Expression *expression()
	{
		return m_expr.get();
	}

	Statement *statement()
	{
		return m_statement.get();
	}

	virtual bool execute();

	virtual string to_string() const;

    private:
	boost::scoped_ptr<Expression> m_expr;
	boost::scoped_ptr<Statement> m_statement;
};

class ReturnStatement : public Statement {
    public:
	ReturnStatement() : m_expr(NULL)
	{
	}
	ReturnStatement(Expression *expr) : m_expr(expr)
	{
		DASSERT(expr);
	}

	Expression *expression()
	{
		return m_expr.get();
	}

	virtual bool execute();

	virtual string to_string() const;

    private:
	boost::scoped_ptr<Expression> m_expr;
};

class DefinitionStatement : public Statement {
    public:
	DefinitionStatement(const Type *type,
	                    const InitializedIdentifierList *vars)
	    : m_public(false), m_type(type), m_vars(vars)
	{
		DASSERT(type);
		DASSERT(vars);
	}

	void set_public()
	{
		m_public = true;
	}

	virtual bool execute();

	virtual string to_string() const;

    private:
	bool m_public;
	boost::scoped_ptr<const Type> m_type;
	boost::scoped_ptr<const InitializedIdentifierList> m_vars;
};

class ImportStatement : public Statement {
    public:
	ImportStatement(const string &arg) : m_argument(arg)
	{
	}

	const string &argument() const
	{
		return m_argument;
	}

	virtual bool execute();

	virtual string to_string() const;

    private:
	const string m_argument;
};

class ModuleStatement : public Statement {
    public:
	ModuleStatement(const string &arg) : m_argument(arg)
	{
	}

	const string &argument() const
	{
		return m_argument;
	}

	virtual bool execute();

	virtual string to_string() const;

    private:
	const string m_argument;
};

class DiscoverStatement : public Statement {
    public:
	DiscoverStatement(ArgumentList *args) : m_args(args)
	{
		DASSERT(args);
	}

	ArgumentList *args() const
	{
		return m_args.get();
	}

	virtual bool execute();

	virtual string to_string() const;

    private:
	boost::scoped_ptr<ArgumentList> m_args;
};

class IdentifierExpression : public Expression {
    public:
	IdentifierExpression(Identifier *ident) : m_ident(ident)
	{
		DASSERT(ident);
	}

	Identifier *identifier()
	{
		return m_ident.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

    private:
	boost::scoped_ptr<Identifier> m_ident;
};

class SubscriptExpression : public Expression {
    public:
	SubscriptExpression(Expression *expr, Expression *index)
	    : m_expr(expr), m_index(index)
	{
		DASSERT(expr);
		DASSERT(index);
	}

	Expression *expression()
	{
		return m_expr.get();
	}

	Expression *index()
	{
		return m_index.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

    private:
	boost::scoped_ptr<Expression> m_expr;
	boost::scoped_ptr<Expression> m_index;
};

class FunctionCallExpression : public Expression {
    public:
	FunctionCallExpression(Expression *expr)
	    : m_expr(expr), m_args(NULL)
	{
		DASSERT(expr);
	}
	FunctionCallExpression(Expression *expr, ArgumentList *args)
	    : m_expr(expr), m_args(args)
	{
		DASSERT(expr);
		DASSERT(args);
	}

	Expression *expression()
	{
		return m_expr.get();
	}

	ArgumentList *args()
	{
		return m_args.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

    private:
	boost::scoped_ptr<Expression> m_expr;
	boost::scoped_ptr<ArgumentList> m_args;
};

class UnaryExpression : public Expression {
    public:
	enum Operator {
		OP_POS,     // +foo
		OP_NEG,     // -foo
		OP_NOT,     // !foo
		OP_BITNOT,  // ~foo
		OP_PREINC,  // ++foo
		OP_PREDEC,  // --foo
		OP_POSTINC, // foo++
		OP_POSTDEC, // foo--
	};

	UnaryExpression(Operator op, Expression *expr)
	    : m_op(op), m_expr(expr)
	{
		DASSERT(expr);
	}

	Operator op() const
	{
		return m_op;
	}

	Expression *expression()
	{
		return m_expr.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

    private:
	Operator m_op;
	boost::scoped_ptr<Expression> m_expr;
};

class BinaryExpression : public Expression {
    public:
	enum Operator {
		OP_EQ,
		OP_NEQ,
		OP_LT,
		OP_GT,
		OP_LE,
		OP_GE,
		OP_MUL,
		OP_DIV,
		OP_MOD,
		OP_ADD,
		OP_SUB,
		OP_SHL,
		OP_SHR,
		OP_AND,
		OP_OR,
		OP_XOR,
		OP_COMMA,
		OP_ASSIGN,
		OP_MUL_ASSIGN,
		OP_DIV_ASSIGN,
		OP_MOD_ASSIGN,
		OP_ADD_ASSIGN,
		OP_SUB_ASSIGN,
		OP_SHL_ASSIGN,
		OP_SHR_ASSIGN,
		OP_AND_ASSIGN,
		OP_OR_ASSIGN,
		OP_XOR_ASSIGN,
	};

	BinaryExpression(Operator op, Expression *lhs,
	                     Expression *rhs)
	    : m_op(op), m_lhs(lhs), m_rhs(rhs)
	{
		DASSERT(lhs);
		DASSERT(rhs);
	}

	Operator op() const
	{
		return m_op;
	}

	Expression *lhs()
	{
		return m_lhs.get();
	}

	Expression *rhs()
	{
		return m_rhs.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

    private:
	Operator m_op;
	boost::scoped_ptr<Expression> m_lhs;
	boost::scoped_ptr<Expression> m_rhs;
};

class ConditionalExpression : public Expression {
    public:
	ConditionalExpression(Expression *condition,
	                      Expression *true_case,
	                      Expression *false_case)
	    : m_condition(condition), m_true(true_case), m_false(false_case)
	{
		DASSERT(condition);
		DASSERT(true_case);
		DASSERT(false_case);
	}

	Expression *condition()
	{
		return m_condition.get();
	}

	Expression *true_case()
	{
		return m_true.get();
	}

	Expression *false_case()
	{
		return m_false.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

    private:
	boost::scoped_ptr<Expression> m_condition;
	boost::scoped_ptr<Expression> m_true;
	boost::scoped_ptr<Expression> m_false;
};

class ParameterDeclaration {
    public:
	ParameterDeclaration(const Type *type, const Identifier *ident)
	    : m_type(type), m_ident(ident)
	{
		DASSERT(type);
		DASSERT(ident);
	}

	virtual string to_string() const;

    private:
	boost::scoped_ptr<const Type> m_type;
	boost::scoped_ptr<const Identifier> m_ident;
};
typedef std::vector<ParameterDeclaration*> ParameterDeclarationList;

class ValueExpression : public Expression {
    public:
	ValueExpression(const Variable::Datum *value) : m_value(value)
	{
		DASSERT(value);
	}

	const Variable::Datum *value() const
	{
		return m_value.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

    private:
	boost::scoped_ptr<const Variable::Datum> m_value;
};

class FunctionLiteralExpression : public Expression {
    public:
	FunctionLiteralExpression() : m_body(NULL)
	{
	}
	FunctionLiteralExpression(Statement *body) : m_body(body)
	{
		DASSERT(body);
	}
	FunctionLiteralExpression(ParameterDeclarationList *params,
	                          Statement *body)
	    : m_params(params), m_body(body)
	{
		DASSERT(params);
		DASSERT(body);
	}

	Statement *body()
	{
		return m_body.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

    private:
	boost::scoped_ptr<ParameterDeclarationList> m_params;
	boost::scoped_ptr<Statement> m_body;
};

class ListLiteralExpression : public Expression {
    public:
	ListLiteralExpression() : m_contents(NULL)
	{
	}
	ListLiteralExpression(ArgumentList *contents)
	    : m_contents(contents)
	{
		DASSERT(contents);
	}

	ArgumentList *contents()
	{
		return m_contents.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

    private:
	boost::scoped_ptr<ArgumentList> m_contents;
};

}  // namespace syntax
}  // namespace language
}  // namespace pp

#endif  // PP_LANGAUGE_SYNTAX_TREE_H__
