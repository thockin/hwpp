// SyntaxNode tree support.

#ifndef HWPP_LANGUAGE_SYNTAX_TREE_H__
#define HWPP_LANGUAGE_SYNTAX_TREE_H__

// The grammar.l and grammar.y files include this header, which will screw
// them up if they include their own auto-generated headers, so don't include
// auto.*.h here.
#include "hwpp.h"
#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include "language/environment.h"
#include "language/errors.h"
#include "language/language.h"
#include "language/type.h"
#include "language/variable.h"
#include "util/pointer.h"

namespace hwpp {
namespace language {
namespace syntax {

// Base class for all nodes in our syntax tree.
class SyntaxNode {
 public:
	enum NodeType {
		NODE_TYPE_IDENTIFIER,    // a symbol name
		NODE_TYPE_DEFINITION,    // an object definition
		NODE_TYPE_EXPRESSION,    // an expression produces a value
		NODE_TYPE_STATEMENT,     // a statement executes
		NODE_TYPE_ARGUMENT,      // a named argument (e.g. function call)
		NODE_TYPE_PARAMETER,     // a parameter declaration
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
	// node and the flags.
	//
	// Returns the number of warnings.  Throws SyntaxError when a hard error
	// is detected.
	virtual int
	validate(const ValidateOptions &flags, Environment *env) = 0;

	// This is a helper method to ensure that calls to validate are
	// idempotent.
	int
	validate_once(const ValidateOptions &flags, Environment *env)
	{
		if (m_validated) {
			return 0;
		}
		m_validated = true;
		return validate(flags, env);
	}

 protected:
	SyntaxNode(const Parser::Position &pos, NodeType node_type)
	    : m_position(pos), m_node_type(node_type), m_validated(false)
	{
	}

	virtual
	~SyntaxNode()
	{
	}

 private:
	Parser::Position m_position;
	NodeType m_node_type;
	bool m_validated;
};

// Abstract base class for expressions which produce a value.
class Expression : public SyntaxNode {
 public:
	explicit
	Expression(const Parser::Position &pos)
	    : SyntaxNode(pos, NODE_TYPE_EXPRESSION), m_result_type(Type::VAR)
	{
	}

	virtual
	~Expression()
	{
	}

	// Evaluate this expression, producing a result.
	virtual void
	evaluate(Variable *out_result) = 0;

	// Get the resulting type of this expression.
	virtual const Type &
	result_type() const
	{
		return m_result_type;
	}

	// Test whether this expression is compile-time evaluatable.
	virtual bool
	is_constexpr() const
	{
		return false;
	}

 protected:
	void
	set_result_type(const Type &type)
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
	    : SyntaxNode(pos, NODE_TYPE_IDENTIFIER),
	      m_module(""), m_symbol(symbol)
	{
	}
	Identifier(const Parser::Position &pos,
	           const string &module, const string &symbol)
	    : SyntaxNode(pos, NODE_TYPE_IDENTIFIER),
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

	const Definition *
	definition() const
	{
		return m_definition;
	}

	virtual string to_string() const;

	//FIXME: move all of the validate methods to the .cpp file.
	virtual int
	validate(const ValidateOptions & /*flags*/, Environment * /*env*/)
	{
		return 0;
	}

 private:
	string m_module;
	string m_symbol;
	Definition *m_definition;
};

class InitializedIdentifier : public SyntaxNode {
 public:
	InitializedIdentifier(const Parser::Position &pos, Identifier *ident)
	    : SyntaxNode(pos, NODE_TYPE_IDENTIFIER),
	      m_ident(ident), m_init(NULL)
	{
	}
	InitializedIdentifier(const Parser::Position &pos,
	                      Identifier *ident, Expression *init)
	    : SyntaxNode(pos, NODE_TYPE_IDENTIFIER),
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = m_ident->validate_once(flags, env);
		if (m_init) {
			warnings += m_init->validate_once(flags, env);
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
	    : SyntaxNode(pos, NODE_TYPE_STATEMENT), m_labels()
	{
	}
	virtual
	~Statement()
	{
		for (size_t i = 0; i < m_labels.size(); i++) {
			delete m_labels[i];
		}
	}

	// Execute this statement.
	virtual bool
	execute() = 0;

	// Get a string representation of this statement.
	virtual string
	to_string() const;

	const std::vector<Identifier*> &
	labels() const
	{
		return m_labels;
	}
	void
	add_label(Identifier *label)
	{
		m_labels.push_back(label);
	}

	virtual int
	validate(const ValidateOptions &flags, Environment *env)
	{
		(void)flags; //FIXME: check labels for conflicts
		(void)env;
		return 0;
	}

 private:
	std::vector<Identifier*> m_labels;
};
typedef std::vector<Statement*> StatementList;

class Definition : public SyntaxNode {
 public:
	Definition(const Parser::Position &pos,
	           const Type &type, InitializedIdentifier *init_ident)
	    : SyntaxNode(pos, NODE_TYPE_DEFINITION),
	      m_type(type), m_init_ident(init_ident)
	{
	}

	const Type &
	type()
	{
		return m_type;
	}

	InitializedIdentifier *
	init_ident()
	{
		return m_init_ident.get();
	}

	Identifier *
	identifier() const
	{
		return m_init_ident->identifier();
	}

	Expression *
	initializer() const
	{
		return m_init_ident->initializer();
	}
	
	virtual string
	to_string() const;

	virtual int
	validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = m_init_ident->validate_once(flags, env);

		Expression *init_expr = m_init_ident->initializer();

		// Some types are not completely defined until init time.
		detect_final_type_from_init(&m_type, init_expr);

		//FIXME: undo flexibility inside Type, call sanity_check()
		// Make sure the type can be initialized correctly.
		if (init_expr != NULL
		 && !m_type.is_initializable_from(init_expr->result_type())) {
			throw SyntaxError(parse_position(),
			    sprintfxx("can't init type '%s' from '%s'",
			              m_type.to_string(),
			              init_expr->result_type().to_string()));
		}

		// Keep track of the symbol, making sure it does not already exist.
		string symbol = m_init_ident->identifier()->symbol();
		if (env->add_symbol(symbol, this) == false) {
			throw SyntaxError(parse_position(),
			    sprintfxx("symbol '%s' redefined", symbol));
		}
		return warnings;
	}

 private:
	Type m_type;
	util::NeverNullScopedPtr<InitializedIdentifier> m_init_ident;

	//FIXME: move to .cpp, make file static
	void
	detect_final_type_from_init(Type *type, Expression *init_expr)
		//FIXME: handle list<list>
		//FIXME: list x = [0]; => list<int-literal>.  Const is bad.
	{
		// Handle "list x;" style definitions.  If there is no initializer,
		// it is equivalent to "list<var> x;".  Otherwise we'll try to
		// detect the best type-arg.
		if (type->primitive() == Type::LIST && type->n_arguments() == 0) {
			if (init_expr == NULL) {
				type->add_argument(Type::VAR);
				return;
			}

			const Type &init_type = init_expr->result_type();
			if (init_type.primitive() == Type::LIST) {
				type->add_argument(init_type.argument(0));
			} else if (init_type.primitive() == Type::TUPLE) {
				Type arg_type = init_type.argument(0);
				for (size_t i = 1; i < init_type.n_arguments(); i++) {
					if (init_type.argument(i) != arg_type) {
						arg_type.reinit(Type::VAR);
						break;
					}
				}
				type->add_argument(arg_type);
			}
			return;
		}

		// Handle "tuple t;" style definitions.  If there is no initializer,
		// it is an error.  Otherwise we'll try to detect the type-args.
		if (type->primitive() == Type::TUPLE && type->n_arguments() == 0) {
			if (init_expr == NULL) {
				throw SyntaxError(parse_position(),
				    "zero-argument tuple definitions must be initialized");
			}

			const Type &init_type = init_expr->result_type();
			if (init_type.primitive() == Type::TUPLE) {
				for (size_t i = 0; i < init_type.n_arguments(); i++) {
					type->add_argument(init_type.argument(i));
				}
			}
		}
	}
};
typedef std::vector<Definition*> DefinitionList;

class Argument : public SyntaxNode {
 public:
	Argument(const Parser::Position &pos, Expression *expr)
	    : SyntaxNode(pos, NODE_TYPE_ARGUMENT), m_name(NULL), m_expr(expr)
	{
	}
	Argument(const Parser::Position &pos,
	         Identifier *name, Expression *expr)
	    : SyntaxNode(pos, NODE_TYPE_ARGUMENT), m_name(name), m_expr(expr)
	{
	}

	Identifier *
	name() const
	{
		return m_name.get();
	}

	Expression *
	expression() const
	{
		return m_expr.get();
	}

	virtual string
	to_string() const;

	virtual int
	validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = m_expr->validate_once(flags, env);
		if (m_name) {
			warnings += m_name->validate_once(flags, env);
		}
		return warnings;
	}

 private:
	util::MaybeNullScopedPtr<Identifier> m_name;
	util::NeverNullScopedPtr<Expression> m_expr;
};
typedef std::vector<Argument*> ArgumentList;

typedef std::vector<Type*> TypeList;

class EmptyStatement : public Statement {
 public:
	explicit
	EmptyStatement(const Parser::Position &pos)
	    : Statement(pos)
	{
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int
	validate(const ValidateOptions &flags, Environment *env)
	{
		return Statement::validate(flags, env);
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		env->new_symbol_scope();
		int warnings = Statement::validate(flags, env);
		for (size_t i = 0; i < m_body->size(); i++) {
			warnings += m_body->at(i)->validate_once(flags, env);
		}
		env->end_symbol_scope();
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		return Statement::validate(flags, env)
		     + m_expr->validate_once(flags, env);
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = Statement::validate(flags, env)
		    + m_condition->validate_once(flags, env)
		    + m_true->validate_once(flags, env);
		if (m_false) {
			warnings += m_false->validate_once(flags, env);
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = Statement::validate(flags, env)
		    + m_condition->validate_once(flags, env)
		    + m_body->validate_once(flags, env);
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = Statement::validate(flags, env)
		    + m_expr->validate_once(flags, env)
		    + m_body->validate_once(flags, env);
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = Statement::validate(flags, env)
		    + m_target->validate_once(flags, env);
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = Statement::validate(flags, env)
		    + m_expr->validate_once(flags, env)
		    + m_statement->validate_once(flags, env);
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = Statement::validate(flags, env);
		if (m_expr) {
			warnings += m_expr->validate_once(flags, env);
		}
		return warnings;
	}

 private:
	util::MaybeNullScopedPtr<Expression> m_expr;
};

class DefinitionStatement : public Statement {
 public:
	DefinitionStatement(const Parser::Position &pos,
	                    Type *def_type, InitializedIdentifierList *def_vars)
	    : Statement(pos)
	{
		// Just for RAII.
		util::NeverNullScopedPtr<Type> type_ptr(def_type);
		util::NeverNullScopedPtr<InitializedIdentifierList> vars_ptr(def_vars);

		// Expand "int x=0, y, z=1;" into "int x=0; int y; int z=1;" for
		// easier use later.
		for (size_t i = 0; i < def_vars->size(); i++) {
			m_vars.push_back(new Definition(pos, *def_type, def_vars->at(i)));
		}
	}

	const DefinitionList &
	vars() const
	{
		return m_vars;
	}

	virtual bool
	execute();

	virtual string
	to_string() const;

	virtual int
	validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = Statement::validate(flags, env);
		for (size_t i = 0; i < m_vars.size(); i++) {
			warnings += m_vars[i]->validate_once(flags, env);
		}
		return warnings;
	}

 private:
	DefinitionList m_vars;
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		//FIXME: validate the arg
		return Statement::validate(flags, env);
	}

 private:
	const string m_argument;
};

class ModuleStatement : public Statement {
 public:
	ModuleStatement(const Parser::Position &pos, const string &name)
	    : Statement(pos), m_name(name)
	{
	}

	const string &name() const
	{
		return m_name;
	}

	virtual bool execute();

	virtual string to_string() const;

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		//FIXME: validate that name does not exist and that this file
		//       has only one module
		return Statement::validate(flags, env);
	}

 private:
	string m_name;
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = Statement::validate(flags, env);
		for (size_t i = 0; i < m_args->size(); i++) {
			warnings += m_args->at(i)->validate_once(flags, env);
		}
		return warnings;
	}

 private:
	util::NeverNullScopedPtr<ArgumentList> m_args;
};

class IdentifierExpression : public Expression {
 public:
	IdentifierExpression(const Parser::Position &pos, Identifier *ident)
	    : Expression(pos), m_ident(ident)
	{
	}

	Identifier *
	identifier()
	{
		return m_ident.get();
	}

	virtual void
	evaluate(Variable *out_result);

	virtual string
	to_string() const;

	virtual int
	validate(const ValidateOptions &flags, Environment *env)
	{
		Definition *def = env->lookup_symbol(m_ident->symbol());
		if (!def) {
			string why = "symbol '" + to_string() + "' can't be resolved";
			if (flags.error_on_unresolved_symbols()) {
				throw SyntaxError(parse_position(), why);
			}
			flags.warnings_stream() << why << std::endl;
			set_result_type(Type::VAR);
			return 1;
		}
		int warnings = def->validate_once(flags, env);
		set_result_type(def->type());
		m_definition = def;
		return warnings;
	}

	//FIXME: comments/docs - All nodes MUST be validated before any other
	// methods are called on them.
	virtual bool
	is_constexpr() const
	{
		if (result_type().is_const()) {
			return true;
		}
		return false;
	}

 private:
	util::NeverNullScopedPtr<Identifier> m_ident;
	Definition *m_definition;  // cached for easy access
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

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = m_expr->validate_once(flags, env)
		    + m_index->validate_once(flags, env);

		// Make sure it can be subscripted.
		const Type &expr_type = m_expr->result_type();
		Type::Primitive expr_prim = expr_type.primitive();
		if (expr_prim != Type::LIST && expr_prim != Type::TUPLE
		 && expr_prim != Type::VAR) {
			string why = "can't subscript type '"
			            + expr_type.to_string() + "'";
			throw SyntaxError(parse_position(), why);
		}

		// Make sure the subscript is legit.
		const Type &index_type = m_index->result_type();
		Type::Primitive index_prim = index_type.primitive();
		if (index_prim != Type::INT && index_prim != Type::VAR) {
			string why = "subscript index must be type '"
			           + Type::primitive_to_string(Type::INT)
			           + "', found type '" + index_type.to_string() + "'";
			throw SyntaxError(parse_position(), why);
		}

		// Lists have at most 1 type-arg.
		if (expr_prim == Type::LIST) {
			// See if it is a list<>, which can be any list type.
			if (expr_type.n_arguments() == 0) {
				//FIXME: const list l = [0,1,2]; bool b = l[0]; must fail.
				set_result_type(Type::VAR);
			} else {
				set_result_type(expr_type.argument(0));
			}
		}

		// Tuples have zero or more type-args.  If the index is a constexpr,
		// we know the type.  If it is not, this expression must be runtime
		// evaluated.
		if (expr_prim == Type::TUPLE) {
			if (m_index->is_constexpr()) {
				//FIXME: const tuple t = [ false ]; bool b = t[0]; must succeed.
				//FIXME: const tuple t = [ false ]; int i = t[0]; must fail.
				try {
					Variable v(Type::INT);
					m_index->evaluate(&v);
					size_t index = v.int_value().as_uint();
					if (index >= expr_type.n_arguments()) {
						string why = sprintfxx(
						    "type '%s' has no [%d] member",
						    expr_type.to_string(), index);
						throw SyntaxError(parse_position(), why);
					}
					set_result_type(expr_type.argument(index));
				} catch (Variable::TypeError &type_error) {
					string why = sprintfxx(
					    "subscript index must be type '%s', found type '%s'",
					    Type::primitive_to_string(Type::INT),
					    index_type.to_string());
					throw SyntaxError(parse_position(), why);
				}
			} else {
				set_result_type(Type::VAR);
			}
		}
		return warnings;
	}

	virtual bool
	is_constexpr() const
	{
		return (m_expr->is_constexpr() && m_index->is_constexpr());
	}

 private:
	util::NeverNullScopedPtr<Expression> m_expr;
	util::NeverNullScopedPtr<Expression> m_index;
};

class FunctionCallExpression : public Expression {
 public:
	//FIXME: reduce to 1 ctor
	FunctionCallExpression(const Parser::Position &pos, Expression *callee)
	    : Expression(pos), m_callee(callee), m_args(NULL)
	{
	}
	FunctionCallExpression(const Parser::Position &pos,
	                       Expression *callee, ArgumentList *args)
	    : Expression(pos), m_callee(callee), m_args(args)
	{
	}

	Expression *callee() const
	{
		return m_callee.get();
	}

	ArgumentList *args() const
	{
		return m_args.get();
	}

	virtual const Type &result_type() const
	{
		static Type t(Type::VAR);
		return t; //FIXME:
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = m_callee->validate_once(flags, env);
		if (m_args) {
			for (size_t i = 0; i < m_args->size(); i++) {
				warnings += m_args->at(i)->validate_once(flags, env);
			}
		}
		return warnings;
	}

 private:
	util::NeverNullScopedPtr<Expression> m_callee;
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = m_expr->validate_once(flags, env);

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
				throw syntax_error("expected non-const expression, found '%s'",
				                   expr_type);
			}
			// fall through
		 case OP_POS:
		 case OP_NEG:
		 case OP_BITNOT:
			// These ops do not about constness.
			if (expr_prim != Type::INT
			 && expr_prim != Type::VAR) {
				throw syntax_error("expected int expression, found '%s'",
				                   expr_type);
			}
			set_result_type(Type::INT);
			break;
		}

		return warnings;
	}

	virtual bool
	is_constexpr() const
	{
		if (m_expr->is_constexpr()) {
			switch (m_op) {
			 case OP_POS:
			 case OP_NEG:
			 case OP_NOT:
			 case OP_BITNOT:
				return true;
			 default:
				break;
			}
		}
		return false;
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		//FIXME: warn if either side is var
		int warnings = m_lhs->validate_once(flags, env)
		    + m_rhs->validate_once(flags, env);

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
				throw syntax_error("can't assign '%s' from '%s'",
				    ltype, rtype);
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

	virtual bool
	is_constexpr() const
	{
		if (m_lhs->is_constexpr() && m_rhs->is_constexpr()) {
			switch (m_op) {
			 case OP_EQ:
			 case OP_NEQ:
			 case OP_LT:
			 case OP_GT:
			 case OP_LE:
			 case OP_GE:
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
			 case OP_COMMA:
				return true;
			 default:
				break;
			}
		}
		return false;
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

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = m_condition->validate_once(flags, env)
		    + m_true->validate_once(flags, env)
		    + m_false->validate_once(flags, env);

		const Type &t1 = m_true->result_type();
		const Type &t2 = m_false->result_type();
		if (t1 == t2) {
			set_result_type(t1);
		}  // else leave it as VAR
		//FIXME: warn if true and false are different types

		return warnings;
	}

	virtual bool
	is_constexpr() const
	{
		return (m_condition->is_constexpr()
		    && m_true->is_constexpr() && m_false->is_constexpr());
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
	    : SyntaxNode(pos, NODE_TYPE_PARAMETER), m_type(type), m_ident(ident)
	{
	}

	virtual string to_string() const;

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = m_ident->validate_once(flags, env);
		return warnings;
	}

 private:
	util::NeverNullScopedPtr<Type> m_type;
	util::NeverNullScopedPtr<Identifier> m_ident;
};
typedef std::vector<ParameterDeclaration*> ParameterDeclarationList;

class ValueExpression : public Expression {
 public:
	ValueExpression(const Parser::Position &pos, const Variable::Datum *value)
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

	virtual int
	validate(const ValidateOptions & /*flags*/, Environment * /*env*/)
	{
		return 0;
	}

	virtual bool
	is_constexpr() const
	{
		return true;
	}

 private:
	util::NeverNullScopedPtr<const Variable::Datum> m_value;
};

// Helpers for easier usage.
class BoolLiteralExpression : public ValueExpression {
 public:
	BoolLiteralExpression(const Parser::Position &pos, bool val)
	    : ValueExpression(pos,
	          new Variable::Datum(Type(Type::BOOL, Type::LITERAL), val))
	{
	}
};
class IntLiteralExpression : public ValueExpression {
 public:
	IntLiteralExpression(const Parser::Position &pos, const Value &val)
	    : ValueExpression(pos,
	          new Variable::Datum(Type(Type::INT, Type::LITERAL), val))
	{
	}
};
class StringLiteralExpression : public ValueExpression {
 public:
	StringLiteralExpression(const Parser::Position &pos, const string &val)
	    : ValueExpression(pos,
	          new Variable::Datum(Type(Type::STRING, Type::LITERAL), val))
	{
	}
};

class FunctionLiteralExpression : public Expression {
 public:
	FunctionLiteralExpression(const Parser::Position &pos,
	                          ParameterDeclarationList *params,
	                          Statement *body)
	    : Expression(pos), m_params(params), m_body(body)
	{
		set_result_type(Type(Type::FUNC, Type::LITERAL));
	}

	Statement *body() const
	{
		return m_body.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = 0;
		env->new_symbol_scope();
		static Definition *args_defn = new_args_definition();
		env->add_symbol("args", args_defn);
		warnings += m_body->validate_once(flags, env);
		for (size_t i = 0; i < m_params->size(); i++) {
			warnings += m_params->at(i)->validate_once(flags, env);
		}
		env->end_symbol_scope();
		return warnings;
	}

	virtual bool
	is_constexpr() const
	{
		return true;
	}

 private:
	// Make a new Definition that is the built-in function args list.
	Definition *
	new_args_definition()
	{
		const Parser::Position &pos = parse_position();
		Type type(Type::LIST);
		type.add_argument(Type::VAR);
		InitializedIdentifier *init_ident
		    = new InitializedIdentifier(pos, new Identifier(pos, "args"));
		return new Definition(pos, type, init_ident);
	}

	util::NeverNullScopedPtr<ParameterDeclarationList> m_params;
	util::NeverNullScopedPtr<Statement> m_body;
};

class TupleLiteralExpression : public Expression {
 public:
	TupleLiteralExpression(const Parser::Position &pos, ArgumentList *contents)
	    : Expression(pos), m_contents(contents)
	{
		set_result_type(Type(Type::TUPLE, Type::LITERAL));
	}

	ArgumentList *contents()
	{
		return m_contents.get();
	}

	virtual void evaluate(Variable *out_result);

	virtual string to_string() const;

	virtual int validate(const ValidateOptions &flags, Environment *env)
	{
		int warnings = 0;
		for (size_t i = 0; i < m_contents->size(); i++) {
			warnings += m_contents->at(i)->validate_once(flags, env);
		}

		// Figure out the type-args of this tuple literal.
		Type my_result_type = result_type();
		for (size_t i = 0; i < m_contents->size(); i++) {
			const Argument *arg = m_contents->at(i);
			const Type &t = arg->expression()->result_type();
			my_result_type.add_argument(t);
		}
		set_result_type(my_result_type);

		return warnings;
	}

	virtual bool
	is_constexpr() const
	{
		return true;
	}

 private:
	util::NeverNullScopedPtr<ArgumentList> m_contents;
};

}  // namespace syntax
}  // namespace language
}  // namespace hwpp

#endif  // HWPP_LANGUAGE_SYNTAX_TREE_H__

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
