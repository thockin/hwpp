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
#include "pp/util/pointer.h"

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

	struct Error: public std::runtime_error
	{
		explicit Error(const std::string &str)
		    : runtime_error(str)
		{
		}
	};

	NodeType node_type() const
	{
		return m_node_type;
	}

	// Produce a string representation of this node.  For complex nodes
	// which contain other nodes, this will be recursive.
	virtual string to_string() const = 0;

	// Validate this node.  What 'validate' means depends entirely on the
	// node and the flags, which is a mask of enum ValidateFlag.  Returns
	// the number of warnings.  Throws SyntaxNode::Error when a hard error
	// is detected.
	enum ValidateFlag {
		VALIDATE_TYPES   = 0x1, // Make sure types match where possible.
		VALIDATE_VAR_USE = 0x2, // Warn when a 'var' type is used in a
		                        //   potentially unsafe way.
		VALIDATE_SYMBOLS = 0x4, // Validate all symbol references.
	};
	virtual int validate(uint64_t flags) const = 0;

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

	// Get the type of this expression.
	virtual const Type &result_type() const = 0;

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

	virtual int validate(uint64_t flags) const {
		//FIXME: ensure that the module and symbol are valid
		(void)flags;
		return 0;
	}

    private:
	string m_module;
	string m_symbol;
};

class InitializedIdentifier : public SyntaxNode {
    public:
	InitializedIdentifier(Identifier *ident)
	    : SyntaxNode(TYPE_IDENTIFIER), m_ident(ident), m_init(NULL)
	{
	}
	InitializedIdentifier(Identifier *ident, Expression *init)
	    : SyntaxNode(TYPE_IDENTIFIER), m_ident(ident), m_init(init)
	{
	}

	Identifier *identifier() const
	{
		return m_ident.get();
	}

	Expression *initializer() const
	{
		return m_init.get();
	}

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	util::NeverNullScopedPtr<Identifier> m_ident;
	util::MaybeNullScopedPtr<Expression> m_init;
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

	const std::vector<Identifier*> &labels() const
	{
		return m_labels;
	}
	void add_label(Identifier *label)
	{
		m_labels.push_back(label);
	}

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME: check labels
		return 0;
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
	}
	Argument(Identifier *name, Expression *expr)
	    : SyntaxNode(TYPE_ARGUMENT), m_name(name), m_expr(expr)
	{
	}

	Identifier *name() const
	{
		return m_name.get();
	}

	Expression *expression() const
	{
		return m_expr.get();
	}

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	util::MaybeNullScopedPtr<Identifier> m_name;
	util::NeverNullScopedPtr<Expression> m_expr;
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

	virtual int validate(uint64_t /*flags*/) const
	{
		return 0;
	}
};

class CompoundStatement : public Statement {
    public:
	CompoundStatement(StatementList *body) : m_body(body)
	{
	}

	StatementList *body() const
	{
		return m_body.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		int warnings = 0;
		for (size_t i = 0; i < m_body->size(); i++) {
			warnings += m_body->at(i)->validate(flags);
		}
		return warnings;
	}

    private:
	util::NeverNullScopedPtr<StatementList> m_body;
};

class ExpressionStatement : public Statement {
    public:
	ExpressionStatement(Expression *expr) : m_expr(expr)
	{
	}

	Expression *expression() const
	{
		return m_expr.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	util::NeverNullScopedPtr<Expression> m_expr;
};

class ConditionalStatement : public Statement {
    public:
	ConditionalStatement(Expression *condition, Statement *true_case)
	    : m_condition(condition), m_true(true_case), m_false(NULL)
	{
	}
	ConditionalStatement(Expression *condition, Statement *true_case,
	                     Statement *false_case)
	    : m_condition(condition), m_true(true_case), m_false(false_case)
	{
	}

	Expression *condition() const
	{
		return m_condition.get();
	}

	Statement *true_case() const
	{
		return m_true.get();
	}

	Statement *false_case() const
	{
		return m_false.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	util::NeverNullScopedPtr<Expression> m_condition;
	util::NeverNullScopedPtr<Statement> m_true;
	util::MaybeNullScopedPtr<Statement> m_false;
};

class SwitchStatement : public Statement {
    public:
	SwitchStatement(Expression *condition, Statement *body)
	    : m_condition(condition), m_body(body)
	{
	}

	Expression *condition() const
	{
		return m_condition.get();
	}

	Statement *body() const
	{
		return m_body.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	util::NeverNullScopedPtr<Expression> m_condition;
	util::NeverNullScopedPtr<Statement> m_body;
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
	util::NeverNullScopedPtr<Expression> m_expr;
	util::NeverNullScopedPtr<Statement> m_body;
};

class WhileLoopStatement : public LoopStatement {
    public:
	WhileLoopStatement(Expression *expr, Statement *body)
	    : LoopStatement(LOOP_WHILE, expr, body)
	{
	}

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}
};

class DoWhileLoopStatement : public LoopStatement {
    public:
	DoWhileLoopStatement(Statement *body, Expression *expr)
	    : LoopStatement(LOOP_DO_WHILE, expr, body)
	{
	}

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}
};

class GotoStatement : public Statement {
    public:
	GotoStatement(Identifier *target) : m_target(target)
	{
	}

	Identifier *target() const
	{
		return m_target.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	util::NeverNullScopedPtr<Identifier> m_target;
};

class CaseStatement : public Statement {
    public:
	CaseStatement(Expression *expr, Statement *statement)
	    : m_expr(expr), m_statement(statement)
	{
	}

	Expression *expression() const
	{
		return m_expr.get();
	}

	Statement *statement() const
	{
		return m_statement.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	util::NeverNullScopedPtr<Expression> m_expr;
	util::NeverNullScopedPtr<Statement> m_statement;
};

class ReturnStatement : public Statement {
    public:
	ReturnStatement() : m_expr(NULL)
	{
	}
	ReturnStatement(Expression *expr) : m_expr(expr)
	{
	}

	Expression *expression() const
	{
		return m_expr.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	util::MaybeNullScopedPtr<Expression> m_expr;
};

class DefinitionStatement : public Statement {
    public:
	DefinitionStatement(const Type *type,
	                    const InitializedIdentifierList *vars)
	    : m_public(false), m_type(type), m_vars(vars)
	{
	}

	void set_public()
	{
		m_public = true;
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	bool m_public;
	util::NeverNullScopedPtr<const Type> m_type;
	util::NeverNullScopedPtr<const InitializedIdentifierList> m_vars;
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

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

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

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	const string m_argument;
};

class DiscoverStatement : public Statement {
    public:
	DiscoverStatement(ArgumentList *args) : m_args(args)
	{
	}

	ArgumentList *args() const
	{
		return m_args.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	util::NeverNullScopedPtr<ArgumentList> m_args;
};

static Type undefined_type(Type::VAR); //FIXME: get rid of this

class IdentifierExpression : public Expression {
    public:
	IdentifierExpression(Identifier *ident) : m_ident(ident)
	{
	}

	Identifier *identifier()
	{
		return m_ident.get();
	}

	virtual const Type &result_type() const
	{
		//FIXME:
		return undefined_type;
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	util::NeverNullScopedPtr<Identifier> m_ident;
};

class SubscriptExpression : public Expression {
    public:
	SubscriptExpression(Expression *expr, Expression *index)
	    : m_expr(expr), m_index(index)
	{
	}

	Expression *expression() const
	{
		return m_expr.get();
	}

	Expression *index() const
	{
		return m_index.get();
	}

	virtual const Type &result_type() const
	{
		//FIXME:
		return undefined_type;
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	util::NeverNullScopedPtr<Expression> m_expr;
	util::NeverNullScopedPtr<Expression> m_index;
};

class FunctionCallExpression : public Expression {
    public:
	FunctionCallExpression(Expression *expr)
	    : m_expr(expr), m_args(NULL)
	{
	}
	FunctionCallExpression(Expression *expr, ArgumentList *args)
	    : m_expr(expr), m_args(args)
	{
	}

	Expression *expression() const
	{
		return m_expr.get();
	}

	ArgumentList *args() const
	{
		return m_args.get();
	}

	virtual const Type &result_type() const
	{
		//FIXME:
		return undefined_type;
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	util::NeverNullScopedPtr<Expression> m_expr;
	util::MaybeNullScopedPtr<ArgumentList> m_args;
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
	    : m_op(op), m_expr(expr), m_result_type(Type::VAR)
	{
		const Type &expr_type = m_expr->result_type();

		switch (m_op) {
		 case OP_NOT:
			if (expr_type != Type::BOOL && expr_type != Type::VAR) {
				throw syntax_error(
				    "expected bool expression, found '%s'",
				    expr_type);
			}
			m_result_type = Type::BOOL;
			break;
		 case OP_POS:
		 case OP_NEG:
		 case OP_BITNOT:
		 case OP_PREINC:
		 case OP_PREDEC:
		 case OP_POSTINC:
		 case OP_POSTDEC:
			if (expr_type != Type::INT && expr_type != Type::VAR) {
				throw syntax_error(
				    "expected int expression, found '%s'",
				    expr_type);
			}
			m_result_type = Type::INT;
			break;
		}
	}

	Operator op() const
	{
		return m_op;
	}

	Expression *expression() const
	{
		return m_expr.get();
	}

	virtual const Type &result_type() const
	{
		return m_result_type;
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		int warnings = 0;
		if (flags & (VALIDATE_TYPES | VALIDATE_VAR_USE)) {
			const Type &t = m_expr->result_type();
			if (flags & VALIDATE_VAR_USE) {
				warnings += validate_var_use(t);
			}
		}
		return warnings;
	}

	//FIXME: make file static
	int validate_var_use(const Type &t) const {
		if (t.primitive() == Type::VAR) {
			// FIXME:
			// emit_warning(
			//     "potential runtime error: %s of a 'var' type");
			return 1;
		}
		return 0;
	}

    private:
	SyntaxNode::Error
	syntax_error(const string &fmt, const Type &type)
	{
		return SyntaxNode::Error(sprintfxx(fmt, type.to_string()));
	}

	Operator m_op;
	util::NeverNullScopedPtr<Expression> m_expr;
	Type m_result_type;
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

	BinaryExpression(Operator op, Expression *lhs, Expression *rhs)
	    : m_op(op), m_lhs(lhs), m_rhs(rhs), m_result_type(Type::VAR)
	{
		const Type &ltype = m_lhs->result_type();
		const Type &rtype = m_rhs->result_type();
		Type::Primitive lprim = ltype.primitive();
		Type::Primitive rprim = rtype.primitive();

		switch (m_op) {
		 case OP_EQ:
		 case OP_NEQ:
			if (!ltype.is_comparable_to(rtype)) {
				throw syntax_error("can't compare '%s' to '%s'",
				    ltype, rtype);
			}
			m_result_type = Type::BOOL;
			break;
		 case OP_ASSIGN:
			if (ltype.is_const()) {
				throw syntax_error("can't assign to '%s'",
				    ltype);
			}
			if (!ltype.is_assignable_from(rtype)) {
				throw syntax_error("can't assign '%s' to '%s'",
				    rtype, ltype);
			}
			m_result_type = ltype != Type::VAR ? ltype : rtype;
			break;
		 case OP_LT:
		 case OP_GT:
		 case OP_LE:
		 case OP_GE:
			if ((lprim != Type::INT && lprim != Type::VAR)
			 || (rprim != Type::INT && rprim != Type::VAR)) {
				throw syntax_error(
				    "expected two int expressions,"
				    " found '%s' and '%s'", lprim, rprim);
			}
			m_result_type = Type::BOOL;
			break;
		 case OP_MUL:
		 case OP_DIV:
		 case OP_MOD:
		 case OP_ADD:
		 case OP_SUB:
		 case OP_SHL:
		 case OP_SHR:
		 case OP_AND:
		 case OP_OR:
		 case OP_XOR:
		 case OP_MUL_ASSIGN:
		 case OP_DIV_ASSIGN:
		 case OP_MOD_ASSIGN:
		 case OP_ADD_ASSIGN:
		 case OP_SUB_ASSIGN:
		 case OP_SHL_ASSIGN:
		 case OP_SHR_ASSIGN:
		 case OP_AND_ASSIGN:
		 case OP_OR_ASSIGN:
		 case OP_XOR_ASSIGN:
			if ((lprim != Type::INT && lprim != Type::VAR)
			 || (rprim != Type::INT && rprim != Type::VAR)) {
				throw syntax_error(
				    "expected two int expressions,"
				    "found '%s' and '%s'", lprim, rprim);
			}
			m_result_type = Type::INT;
			break;
		 case OP_COMMA:
			m_result_type = m_rhs->result_type();
			break;
		}
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

	virtual const Type &result_type() const
	{
		//FIXME: warn if either side is var
		return undefined_type;
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		return 0;
	}

    private:
	SyntaxNode::Error
	syntax_error(const string &fmt, const Type &type)
	{
		return SyntaxNode::Error(sprintfxx(fmt, type.to_string()));
	}
	SyntaxNode::Error
	syntax_error(const string &fmt, const Type &lhs, const Type &rhs)
	{
		return SyntaxNode::Error(
		    sprintfxx(fmt, lhs.to_string(), rhs.to_string()));
	}

	Operator m_op;
	util::NeverNullScopedPtr<Expression> m_lhs;
	util::NeverNullScopedPtr<Expression> m_rhs;
	Type m_result_type;
};

class ConditionalExpression : public Expression {
    public:
	ConditionalExpression(Expression *condition,
	                      Expression *true_case,
	                      Expression *false_case)
	    : m_condition(condition), m_true(true_case), m_false(false_case),
	      m_result_type(Type::VAR)
	{
		const Type &t1 = true_case->result_type();
		const Type &t2 = false_case->result_type();
		if (t1 == t2) {
			m_result_type = t1;
		} else {
			m_result_type.reinit(Type::VAR);
		}
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

	virtual const Type &result_type() const
	{
		//FIXME:
		return undefined_type;
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		(void)flags; //FIXME:
		//FIXME: warn if true and false are different types
		return 0;
	}

    private:
	util::NeverNullScopedPtr<Expression> m_condition;
	util::NeverNullScopedPtr<Expression> m_true;
	util::NeverNullScopedPtr<Expression> m_false;
	Type m_result_type;
};

class ParameterDeclaration {
    public:
	ParameterDeclaration(const Type *type, const Identifier *ident)
	    : m_type(type), m_ident(ident)
	{
	}

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		//FIXME:
		(void)flags;
		return 0;
	}

    private:
	util::NeverNullScopedPtr<const Type> m_type;
	util::NeverNullScopedPtr<const Identifier> m_ident;
};
typedef std::vector<ParameterDeclaration*> ParameterDeclarationList;

class LiteralExpression : public Expression {
    public:
	LiteralExpression(const Variable::Datum *value) : m_value(value)
	{
		// It doesn't make much sense to have a non-const literal.
		DASSERT(value->type().is_const());
	}

	const Variable::Datum *value() const
	{
		return m_value.get();
	}

	virtual const Type &result_type() const
	{
		return m_value->type();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(uint64_t /*flags*/) const
	{
		return 0;
	}

    private:
	util::NeverNullScopedPtr<const Variable::Datum> m_value;
};

class FunctionLiteralExpression : public Expression {
    public:
	FunctionLiteralExpression(Statement *body)
	    : m_params(NULL), m_body(body),
	      m_result_type(Type::FUNC, Type::CONST)
	{
	}
	FunctionLiteralExpression(ParameterDeclarationList *params,
	                          Statement *body)
	    : m_params(params), m_body(body),
	      m_result_type(Type::FUNC, Type::CONST)
	{
	}

	Statement *body() const
	{
		return m_body.get();
	}

	virtual const Type &result_type() const
	{
		return m_result_type;
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(uint64_t flags) const
	{
		int warnings = 0;
		if (m_params) {
			for (size_t i = 0; i < m_params->size(); i++) {
				warnings += m_params->at(i)->validate(flags);
			}
		}
		warnings += m_body->validate(flags);
		return warnings;
	}

    private:
	util::MaybeNullScopedPtr<ParameterDeclarationList> m_params;
	util::NeverNullScopedPtr<Statement> m_body;
	Type m_result_type;
};

//FIXME: tuple literal?  assignable from list>
class ListLiteralExpression : public Expression {
    public:
	ListLiteralExpression(ArgumentList *contents)
	    : m_contents(contents), m_result_type(Type::LIST, Type::CONST)
	{
		// Figure out the actual type of this list literal.
		Type content_type(Type::VAR);
		for (size_t i = 0; i < m_contents->size(); i++) {
			const Argument *arg = m_contents->at(i);
			const Type &t = arg->expression()->result_type();
			if (content_type == Type::VAR) {
				content_type = t;
			} else if (t != content_type) {
				content_type = Type::VAR;
				break;
			}
		}
		if (content_type != Type::VAR) {
			m_result_type.add_argument(content_type);
		} else {
			m_result_type.add_argument(Type::VAR);
		}
		m_result_type.set_const();
	}

	ArgumentList *contents()
	{
		return m_contents.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual const Type &result_type() const
	{
		return m_result_type;
	}

	virtual int validate(uint64_t flags) const
	{
		int warnings = 0;
		if (m_contents) {
			for (size_t i = 0; i < m_contents->size(); i++) {
				warnings += m_contents->at(i)->validate(flags);
			}
		}
		return warnings;
	}

    private:
	util::NeverNullScopedPtr<ArgumentList> m_contents;
	Type m_result_type;
};

}  // namespace syntax
}  // namespace language
}  // namespace pp

#endif  // PP_LANGAUGE_SYNTAX_TREE_H__
