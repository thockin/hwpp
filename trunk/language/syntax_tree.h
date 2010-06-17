// SyntaxNode tree support.

#ifndef PP_LANGUAGE_SYNTAX_TREE_H__
#define PP_LANGUAGE_SYNTAX_TREE_H__

// The grammar.l and grammar.y files include this header, which will screw
// them up if they include their own auto-generated headers, so don't include
// auto.*.h here.
#include "pp.h"
#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include "language/language.h"
#include "language/variable.h"
#include "util/pointer.h"

namespace pp {
namespace language {
namespace syntax {

struct SyntaxError: public std::runtime_error
{
	//FIXME: format the what() on the fly.
	SyntaxError(const Parser::Position &pos, const std::string &str)
	    : runtime_error(sprintfxx("[%s:%d] : %s", pos.file, pos.line, str))
	{
	}
};

// Base class for all nodes in our syntax tree.
class SyntaxNode {
    public:
	enum NodeType {
		TYPE_IDENTIFIER,    // a symbol name
		TYPE_EXPRESSION,    // an expression produces a value
		TYPE_STATEMENT,     // a statement executes
		TYPE_ARGUMENT,      // a named argument (e.g. function call)
		TYPE_PARAMETER,     // a parameter declaration
	};

	NodeType
	node_type() const
	{
		return m_node_type;
	}

	const Parser::Position &
	parse_position() const
	{
		return m_position;
	}

	// Produce a string representation of this node.  For complex nodes
	// which contain other nodes, this will be recursive.
	virtual string
	to_string() const = 0;

	// Validate this node.  What 'validate' means depends entirely on the
	// node and the flags, which is a mask of enum ValidateFlag.  Returns
	// the number of warnings.  Throws SyntaxError when a hard error
	// is detected.
	enum ValidateFlag {
		VALIDATE_TYPES   = 0x1, // Make sure types match where possible.
		VALIDATE_VAR_USE = 0x2, // Warn when a 'var' type is used in a
		                        //   potentially unsafe way.
		VALIDATE_SYMBOLS = 0x4, // Validate all symbol references.
	};
	virtual int
	validate(uint64_t flags) = 0;

    protected:
	SyntaxNode(const Parser::Position &pos, NodeType node_type)
	    : m_position(pos), m_node_type(node_type)
	{
	}

	virtual
	~SyntaxNode()
	{
	}

    private:
	Parser::Position m_position;
	NodeType m_node_type;
};

// Abstract base class for expressions which produce a value.
class Expression : public SyntaxNode {
    public:
	explicit
	Expression(const Parser::Position &pos)
	    : SyntaxNode(pos, TYPE_EXPRESSION), m_result_type(Type::VAR)
	{
	}

	virtual ~Expression()
	{
	}

	// Evaluate this expression, producing a result.
	virtual void evaluate(Variable *out_result) = 0;

	// Get the resulting type of this expression.
	virtual const Type &result_type() const
	{
		return m_result_type;
	}

    protected:
	void set_result_type(const Type &type)
	{
		m_result_type = type;
	}

    private:
	// The resulting datatype of the Expression.
	Type m_result_type;
};
typedef std::vector<Expression*> ExpressionList;

class Identifier : public SyntaxNode {
    public:
	Identifier(const Parser::Position &pos, const string &symbol)
	    : SyntaxNode(pos, TYPE_IDENTIFIER),
	      m_module(""), m_symbol(symbol)
	{
	}
	Identifier(const Parser::Position &pos,
	           const string &module, const string &symbol)
	    : SyntaxNode(pos, TYPE_IDENTIFIER),
	      m_module(module), m_symbol(symbol)
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

	virtual int validate(uint64_t flags) {
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
	InitializedIdentifier(const Parser::Position &pos, Identifier *ident)
	    : SyntaxNode(pos, TYPE_IDENTIFIER),
	      m_ident(ident), m_init(NULL)
	{
	}
	InitializedIdentifier(const Parser::Position &pos,
	                      Identifier *ident, Expression *init)
	    : SyntaxNode(pos, TYPE_IDENTIFIER),
	                 m_ident(ident), m_init(init)
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

	virtual int validate(uint64_t flags)
	{
		int warnings = m_ident->validate(flags);
		if (m_init) {
			warnings += m_init->validate(flags);
		}
		return warnings;
	}

    private:
	util::NeverNullScopedPtr<Identifier> m_ident;
	util::MaybeNullScopedPtr<Expression> m_init;
};
typedef std::vector<InitializedIdentifier*> InitializedIdentifierList;

// Abstract base class for statements.
class Statement : public SyntaxNode {
    public:
	explicit
	Statement(const Parser::Position &pos)
	    : SyntaxNode(pos, TYPE_STATEMENT), m_labels(NULL)
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

	virtual int validate(uint64_t flags)
	{
		//FIXME: make all subclasses call this
		(void)flags; //FIXME: check labels
		return 0;
	}

    private:
	std::vector<Identifier*> m_labels;
};
typedef std::vector<Statement*> StatementList;

class Argument : public SyntaxNode {
    public:
	Argument(const Parser::Position &pos, Expression *expr)
	    : SyntaxNode(pos, TYPE_ARGUMENT), m_name(NULL), m_expr(expr)
	{
	}
	Argument(const Parser::Position &pos,
	         Identifier *name, Expression *expr)
	    : SyntaxNode(pos, TYPE_ARGUMENT), m_name(name), m_expr(expr)
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

	virtual int validate(uint64_t flags)
	{
		int warnings = m_expr->validate(flags);
		if (m_name) {
			warnings += m_name->validate(flags);
		}
		return warnings;
	}

    private:
	util::MaybeNullScopedPtr<Identifier> m_name;
	util::NeverNullScopedPtr<Expression> m_expr;
};
typedef std::vector<Argument*> ArgumentList;

typedef std::vector<Type*> TypeList;

class NullStatement : public Statement {
    public:
	explicit
	NullStatement(const Parser::Position &pos)
	    : Statement(pos)
	{
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t /*flags*/)
	{
		return 0;
	}
};

class CompoundStatement : public Statement {
    public:
	CompoundStatement(const Parser::Position &pos, StatementList *body)
	    : Statement(pos), m_body(body)
	{
	}

	StatementList *body() const
	{
		return m_body.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags)
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
	ExpressionStatement(const Parser::Position &pos, Expression *expr)
	    : Statement(pos), m_expr(expr)
	{
	}

	Expression *expression() const
	{
		return m_expr.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags)
	{
		return m_expr->validate(flags);
	}

    private:
	util::NeverNullScopedPtr<Expression> m_expr;
};

class ConditionalStatement : public Statement {
    public:
	ConditionalStatement(const Parser::Position &pos,
	                     Expression *condition, Statement *true_case)
	    : Statement(pos),
	      m_condition(condition), m_true(true_case), m_false(NULL)
	{
	}
	ConditionalStatement(const Parser::Position &pos,
	                     Expression *condition, Statement *true_case,
	                     Statement *false_case)
	    : Statement(pos),
	      m_condition(condition), m_true(true_case), m_false(false_case)
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

	virtual int validate(uint64_t flags)
	{
		int warnings = m_condition->validate(flags)
		    + m_true->validate(flags);
		if (m_false) {
			warnings += m_false->validate(flags);
		}
		return warnings;
	}

    private:
	util::NeverNullScopedPtr<Expression> m_condition;
	util::NeverNullScopedPtr<Statement> m_true;
	util::MaybeNullScopedPtr<Statement> m_false;
};

class SwitchStatement : public Statement {
    public:
	SwitchStatement(const Parser::Position &pos,
	                Expression *condition, Statement *body)
	    : Statement(pos), m_condition(condition), m_body(body)
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

	virtual int validate(uint64_t flags)
	{
		int warnings = m_condition->validate(flags)
		    + m_body->validate(flags);
		return warnings;
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
	LoopStatement(const Parser::Position &pos,
	              LoopType loop_type, Expression *expr, Statement *body)
	    : Statement(pos),
	      m_loop_type(loop_type), m_expr(expr), m_body(body)
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

	virtual int validate(uint64_t flags)
	{
		int warnings = m_expr->validate(flags)
		    + m_body->validate(flags);
		return warnings;
	}

	virtual bool execute();

    private:
	LoopType m_loop_type;
	util::NeverNullScopedPtr<Expression> m_expr;
	util::NeverNullScopedPtr<Statement> m_body;
};

class WhileLoopStatement : public LoopStatement {
    public:
	WhileLoopStatement(const Parser::Position &pos,
	                   Expression *expr, Statement *body)
	    : LoopStatement(pos, LOOP_WHILE, expr, body)
	{
	}

	virtual string to_string() const;
};

class DoWhileLoopStatement : public LoopStatement {
    public:
	DoWhileLoopStatement(const Parser::Position &pos,
	                     Statement *body, Expression *expr)
	    : LoopStatement(pos, LOOP_DO_WHILE, expr, body)
	{
	}

	virtual string to_string() const;
};

class GotoStatement : public Statement {
    public:
	GotoStatement(const Parser::Position &pos, Identifier *target)
	    : Statement(pos), m_target(target)
	{
	}

	Identifier *target() const
	{
		return m_target.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags)
	{
		int warnings = m_target->validate(flags);
		return warnings;
	}

    private:
	util::NeverNullScopedPtr<Identifier> m_target;
};

class CaseStatement : public Statement {
    public:
	CaseStatement(const Parser::Position &pos,
	              Expression *expr, Statement *statement)
	    : Statement(pos), m_expr(expr), m_statement(statement)
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

	virtual int validate(uint64_t flags)
	{
		int warnings = m_expr->validate(flags)
		    + m_statement->validate(flags);
		return warnings;
	}

    private:
	util::NeverNullScopedPtr<Expression> m_expr;
	util::NeverNullScopedPtr<Statement> m_statement;
};

class ReturnStatement : public Statement {
    public:
	explicit
	ReturnStatement(const Parser::Position &pos)
	    : Statement(pos), m_expr(NULL)
	{
	}
	ReturnStatement(const Parser::Position &pos, Expression *expr)
	    : Statement(pos), m_expr(expr)
	{
	}

	Expression *expression() const
	{
		return m_expr.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags)
	{
		int warnings = 0;
		if (m_expr) {
			warnings += m_expr->validate(flags);
		}
		return warnings;
	}

    private:
	util::MaybeNullScopedPtr<Expression> m_expr;
};

class DefinitionStatement : public Statement {
    public:
	DefinitionStatement(const Parser::Position &pos,
	                    Type *type, InitializedIdentifierList *vars)
	    : Statement(pos), m_public(false), m_type(type), m_vars(vars)
	{
	}

	Type *type() const
	{
		return m_type.get();
	}

	InitializedIdentifierList *vars() const
	{
		return m_vars.get();
	}

	void set_public()
	{
		m_public = true;
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags)
	{
		int warnings = 0;
		for (size_t i = 0; i < m_vars->size(); i++) {
			warnings += m_vars->at(i)->validate(flags);
		}
		return warnings;
	}

    private:
	bool m_public;
	util::NeverNullScopedPtr<Type> m_type;
	util::NeverNullScopedPtr<InitializedIdentifierList> m_vars;
};

class ImportStatement : public Statement {
    public:
	ImportStatement(const Parser::Position &pos, const string &arg)
	    : Statement(pos), m_argument(arg)
	{
	}

	const string &argument() const
	{
		return m_argument;
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags)
	{
		(void)flags; //FIXME: validate the arg
		return 0;
	}

    private:
	const string m_argument;
};

class ModuleStatement : public Statement {
    public:
	ModuleStatement(const Parser::Position &pos, const string &arg)
	    : Statement(pos), m_argument(arg)
	{
	}

	const string &argument() const
	{
		return m_argument;
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags)
	{
		(void)flags; //FIXME: validate the arg
		return 0;
	}

    private:
	string m_argument;
};

class DiscoverStatement : public Statement {
    public:
	DiscoverStatement(const Parser::Position &pos, ArgumentList *args)
	    : Statement(pos), m_args(args)
	{
	}

	ArgumentList *args() const
	{
		return m_args.get();
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(uint64_t flags)
	{
		int warnings = 0;
		for (size_t i = 0; i < m_args->size(); i++) {
			warnings += m_args->at(i)->validate(flags);
		}
		return warnings;
	}

    private:
	util::NeverNullScopedPtr<ArgumentList> m_args;
};

static Type undefined_type(Type::VAR); //FIXME: get rid of this

class IdentifierExpression : public Expression {
    public:
	IdentifierExpression(const Parser::Position &pos, Identifier *ident)
	    : Expression(pos), m_ident(ident)
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

	virtual int validate(uint64_t flags)
	{
		int warnings = m_ident->validate(flags);
		return warnings;
	}

    private:
	util::NeverNullScopedPtr<Identifier> m_ident;
};

class SubscriptExpression : public Expression {
    public:
	SubscriptExpression(const Parser::Position &pos,
	                    Expression *expr, Expression *index)
	    : Expression(pos), m_expr(expr), m_index(index)
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

	virtual int validate(uint64_t flags)
	{
		int warnings = m_expr->validate(flags)
		    + m_index->validate(flags);
		return warnings;
	}

    private:
	util::NeverNullScopedPtr<Expression> m_expr;
	util::NeverNullScopedPtr<Expression> m_index;
};

class FunctionCallExpression : public Expression {
    public:
	FunctionCallExpression(const Parser::Position &pos, Expression *expr)
	    : Expression(pos), m_expr(expr), m_args(NULL)
	{
	}
	FunctionCallExpression(const Parser::Position &pos,
	                       Expression *expr, ArgumentList *args)
	    : Expression(pos), m_expr(expr), m_args(args)
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

	virtual int validate(uint64_t flags)
	{
		int warnings = m_expr->validate(flags);
		if (m_args) {
			for (size_t i = 0; i < m_args->size(); i++) {
				warnings += m_args->at(i)->validate(flags);
			}
		}
		return warnings;
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

	UnaryExpression(const Parser::Position &pos,
	                Operator op, Expression *expr)
	    : Expression(pos), m_op(op), m_expr(expr)
	{
	}

	Operator op() const
	{
		return m_op;
	}

	Expression *expression() const
	{
		return m_expr.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(uint64_t flags)
	{
		int warnings = m_expr->validate(flags);

		const Type &expr_type = m_expr->result_type();
		Type::Primitive expr_prim = expr_type.primitive();

		switch (m_op) {
		 case OP_NOT:
			// Does not care about constness.
			if (expr_prim != Type::BOOL
			 && expr_prim != Type::VAR) {
				throw syntax_error(
				    "expected bool expression, found '%s'",
				    expr_type);
			}
			set_result_type(Type::BOOL);
			break;
		 case OP_PREINC:
		 case OP_PREDEC:
		 case OP_POSTINC:
		 case OP_POSTDEC:
			// These ops require non-const expressions.
		 	if (expr_type.is_const()) {
				throw syntax_error(
				    "expected non-const int expression,"
				    " found '%s'", expr_type);
			}
			// fall through
		 case OP_POS:
		 case OP_NEG:
		 case OP_BITNOT:
			// These ops do not about constness.
			if (expr_prim != Type::INT
			 && expr_prim != Type::VAR) {
				throw syntax_error(
				    "expected int expression, found '%s'",
				    expr_type);
			}
			set_result_type(Type::INT);
			break;
		}

		if (flags & VALIDATE_VAR_USE) {
			warnings += validate_var_use(expr_type);
		}

		return warnings;
	}

	//FIXME: make file static
	int validate_var_use(const Type &t) {
		if (t.primitive() == Type::VAR) {
			// FIXME:
			// emit_warning(
			//     "potential runtime error: %s of a 'var' type");
			return 1;
		}
		return 0;
	}

    private:
	SyntaxError
	syntax_error(const string &fmt, const Type &type) const
	{
		return SyntaxError(parse_position(),
		                   sprintfxx(fmt, type.to_string()));
	}

	Operator m_op;
	util::NeverNullScopedPtr<Expression> m_expr;
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

	BinaryExpression(const Parser::Position &pos,
	                 Operator op, Expression *lhs, Expression *rhs)
	    : Expression(pos),
	      m_op(op), m_lhs(lhs), m_rhs(rhs)
	{
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

	virtual int validate(uint64_t flags)
	{
		//FIXME: warn if either side is var
		int warnings = m_lhs->validate(flags) + m_rhs->validate(flags);

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
			set_result_type(Type::BOOL);
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
			set_result_type(ltype != Type::VAR ? ltype : rtype);
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
			set_result_type(Type::BOOL);
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
				    " found '%s' and '%s'", lprim, rprim);
			}
			set_result_type(Type::INT);
			break;
		 case OP_COMMA:
			set_result_type(m_rhs->result_type());
			break;
		}

		return warnings;
	}

    private:
	SyntaxError
	syntax_error(const string &fmt, const Type &type) const
	{
		return SyntaxError(parse_position(),
		                   sprintfxx(fmt, type.to_string()));
	}
	SyntaxError
	syntax_error(const string &fmt, const Type &lhs, const Type &rhs) const
	{
		return SyntaxError(parse_position(),
		    sprintfxx(fmt, lhs.to_string(), rhs.to_string()));
	}

	Operator m_op;
	util::NeverNullScopedPtr<Expression> m_lhs;
	util::NeverNullScopedPtr<Expression> m_rhs;
};

class ConditionalExpression : public Expression {
    public:
	ConditionalExpression(const Parser::Position &pos,
	                      Expression *condition,
	                      Expression *true_case,
	                      Expression *false_case)
	    : Expression(pos),
	      m_condition(condition), m_true(true_case), m_false(false_case)
	{
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

	virtual int validate(uint64_t flags)
	{
		int warnings = m_condition->validate(flags)
		    + m_true->validate(flags) + m_false->validate(flags);

		const Type &t1 = m_true->result_type();
		const Type &t2 = m_false->result_type();
		if (t1 == t2) {
			set_result_type(t1);
		}  // else leave it as VAR
		//FIXME: warn if true and false are different types

		return warnings;
	}

    private:
	util::NeverNullScopedPtr<Expression> m_condition;
	util::NeverNullScopedPtr<Expression> m_true;
	util::NeverNullScopedPtr<Expression> m_false;
};

class ParameterDeclaration : public SyntaxNode {
    public:
	ParameterDeclaration(const Parser::Position &pos,
	                     Type *type, Identifier *ident)
	    : SyntaxNode(pos, TYPE_PARAMETER), m_type(type), m_ident(ident)
	{
	}

	virtual string to_string() const;

	virtual int validate(uint64_t flags)
	{
		int warnings = m_ident->validate(flags);
		return warnings;
	}

    private:
	util::NeverNullScopedPtr<Type> m_type;
	util::NeverNullScopedPtr<Identifier> m_ident;
};
typedef std::vector<ParameterDeclaration*> ParameterDeclarationList;

class LiteralExpression : public Expression {
    public:
	LiteralExpression(const Parser::Position &pos,
	                  const Variable::Datum *value)
	    : Expression(pos), m_value(value)
	{
		// It doesn't make much sense to have a non-const literal.
		DASSERT(value->type().is_const());
		set_result_type(m_value->type());
	}

	const Variable::Datum *value() const
	{
		return m_value.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(uint64_t /*flags*/)
	{
		return 0;
	}

    private:
	util::NeverNullScopedPtr<const Variable::Datum> m_value;
};

// Helpers for easier usage.
class BoolLiteralExpression : public LiteralExpression {
    public:
	BoolLiteralExpression(const Parser::Position &pos, bool val)
	    : LiteralExpression(pos,
	          new Variable::Datum(Type(Type::BOOL, Type::CONST), val))
	{
	}
};
class IntLiteralExpression : public LiteralExpression {
    public:
	IntLiteralExpression(const Parser::Position &pos, const Value &val)
	    : LiteralExpression(pos,
	          new Variable::Datum(Type(Type::INT, Type::CONST), val))
	{
	}
};
class StringLiteralExpression : public LiteralExpression {
    public:
	StringLiteralExpression(const Parser::Position &pos, const string &val)
	    : LiteralExpression(pos,
	          new Variable::Datum(Type(Type::STRING, Type::CONST), val))
	{
	}
};

class FunctionLiteralExpression : public Expression {
    public:
	FunctionLiteralExpression(const Parser::Position &pos, Statement *body)
	    : Expression(pos),
	      m_params(NULL), m_body(body)
	{
		set_result_type(Type(Type::FUNC, Type::CONST));
	}
	FunctionLiteralExpression(const Parser::Position &pos,
	                          ParameterDeclarationList *params,
	                          Statement *body)
	    : Expression(pos), m_params(params), m_body(body)
	{
		set_result_type(Type(Type::FUNC, Type::CONST));
	}

	Statement *body() const
	{
		return m_body.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(uint64_t flags)
	{
		int warnings = m_body->validate(flags);
		if (m_params) {
			for (size_t i = 0; i < m_params->size(); i++) {
				warnings += m_params->at(i)->validate(flags);
			}
		}
		return warnings;
	}

    private:
	util::MaybeNullScopedPtr<ParameterDeclarationList> m_params;
	util::NeverNullScopedPtr<Statement> m_body;
};

//FIXME: tuple literal?  assignable from list>
class ListLiteralExpression : public Expression {
    public:
	ListLiteralExpression(const Parser::Position &pos,
	                      ArgumentList *contents)
	    : Expression(pos), m_contents(contents)
	{
		set_result_type(Type(Type::LIST, Type::CONST));
	}

	ArgumentList *contents()
	{
		return m_contents.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(uint64_t flags)
	{
		int warnings = 0;
		for (size_t i = 0; i < m_contents->size(); i++) {
			warnings += m_contents->at(i)->validate(flags);
		}

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
		Type full_result_type = result_type();
		if (content_type != Type::VAR) {
			full_result_type.add_argument(content_type);
		} else {
			full_result_type.add_argument(Type::VAR);
		}
		set_result_type(full_result_type);

		return warnings;
	}

    private:
	util::NeverNullScopedPtr<ArgumentList> m_contents;
};

}  // namespace syntax
}  // namespace language
}  // namespace pp

#endif  // PP_LANGUAGE_SYNTAX_TREE_H__
