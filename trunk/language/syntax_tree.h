// SyntaxNode tree support.

#ifndef PP_LANGAUGE_SYNTAX_TREE_H__
#define PP_LANGAUGE_SYNTAX_TREE_H__

// The grammar.l and grammar.y files include this header, which will screw
// them up if they include their own auto-generated headers, so don't include
// auto.*.h here.
#include "pp/pp.h"
#include <map>
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

	virtual string to_string() const = 0;
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

	string to_string() const
	{
		return m_module + (m_module == "" ? "" : ".") + m_symbol;
	}

	const string &module() const
	{
		return m_module;
	}

	const string &symbol() const
	{
		return m_symbol;
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

	string to_string() const
	{
		string ret = m_ident->to_string();
		if (m_init) {
			ret += " = " + m_init->to_string() + "\n";
		}
		return ret;
	}

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
	virtual string to_string() const
	{
		string ret;
		for (size_t i = 0; i < m_labels.size(); i++) {
			ret += m_labels[i]->to_string() + ":\n";
		}
		return ret;
	}

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

	string to_string() const
	{
		return (m_name ? m_name->to_string() + ":" : "")
		     + m_expr->to_string();
	}

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

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		return ret + "(undef);";
	}
};

class CompoundStatement : public Statement {
    public:
	CompoundStatement() : m_body(NULL)
	{
	}
	CompoundStatement(StatementList *body) : m_body(body)
	{
		DASSERT(body);
	}

	StatementList *body()
	{
		return m_body.get();
	}

	virtual bool execute();

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		ret += "{\n";
		if (m_body) {
			for (size_t i = 0; i < m_body->size(); i++) {
				ret += m_body->at(i)->to_string();
			}
		}
		ret += "}\n";
		return ret;
	}

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

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		return m_expr->to_string();
	}

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

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		ret += "if (" + m_condition->to_string() + ") {\n";
		ret += m_true->to_string();
		if (m_false) {
			ret += "} else {\n";
			ret += m_false->to_string();
		}
		ret += "}\n";
		return ret;
	}

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

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		ret += "switch (" + m_condition->to_string() + ") {\n";
		ret += m_body->to_string();
		ret += "}\n";
		return ret;
	}

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

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		ret += "while (" + expression()->to_string() + ") {\n";
		ret += body()->to_string();
		ret += "}\n";
		return ret;
	}
};

class DoWhileLoopStatement : public LoopStatement {
    public:
	DoWhileLoopStatement(Statement *body, Expression *expr)
	    : LoopStatement(LOOP_DO_WHILE, expr, body)
	{
		DASSERT(body);
		DASSERT(expr);
	}

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		ret += "do {\n";
		ret += body()->to_string();
		ret += " } while (" + expression()->to_string() + ")\n";
		return ret;
	}
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

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		ret += "goto " + m_target->to_string() + ";\n";
		return ret;
	}

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

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		ret += "case (" + m_expr->to_string() + "):\n";
		return ret;
	}

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

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		ret += "return (" + m_expr->to_string() + ");\n";
		return ret;
	}

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

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		ret += m_type->to_string() + " ";
		for (size_t i = 0; i < m_vars->size(); i++) {
			if (i > 0) {
				ret += ", ";
			}
			ret += m_vars->at(i)->to_string();
		}
		ret += "\n";
		return ret;
	}

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

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		ret += "import \"" + m_argument + "\";\n";
		return ret;
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

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		ret += "module " + m_argument + ";\n";
		return ret;
	}

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

	virtual string to_string() const
	{
		string ret = Statement::to_string();
		ret += "discover(";
		for (size_t i = 0; i < m_args->size(); i++) {
			if (i > 0) {
				ret += ", ";
			}
			ret += m_args->at(i)->to_string();
		}
		ret += ");";
		return ret;
	}

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

	virtual string to_string() const
	{
		return m_ident->to_string();
	}

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

	virtual string to_string() const
	{
		return "(" + m_expr->to_string() + ")"
		     + "[" + m_index->to_string() + "]";
	}

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

	virtual string to_string() const
	{
		string ret = "(" + m_expr->to_string() + ")" + "(";
		if (m_args) {
			for (size_t i = 0; i < m_args->size(); i++) {
				if (i > 0) {
					ret += ", ";
				}
				ret += m_args->at(i)->to_string();
			}
		}
		ret += ")";
		return ret;
	}

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

	virtual string to_string() const
	{
		string ret;
		switch (m_op) {
		 case OP_POS: ret += "+"; break;
		 case OP_NEG: ret += "-"; break;
		 case OP_NOT: ret += "!"; break;
		 case OP_BITNOT: ret += "~"; break;
		 case OP_PREINC: ret += "++"; break;
		 case OP_PREDEC: ret += "--"; break;
		 default: break;
		}
		ret += "(" + m_expr->to_string() + ")";
		switch (m_op) {
		 case OP_POSTINC: ret += "++"; break;
		 case OP_POSTDEC: ret += "--"; break;
		 default: break;
		}
		return ret;
	}

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

	virtual string to_string() const
	{
		string ret = m_lhs->to_string() + " ";
		switch (m_op) {
		 case OP_EQ: ret += "=="; break;
		 case OP_NEQ: ret += "!="; break;
		 case OP_LT: ret += "<"; break;
		 case OP_GT: ret += ">"; break;
		 case OP_LE: ret += "<="; break;
		 case OP_GE: ret += ">="; break;
		 case OP_MUL: ret += "*"; break;
		 case OP_DIV: ret += "/"; break;
		 case OP_MOD: ret += "%"; break;
		 case OP_ADD: ret += "+"; break;
		 case OP_SUB: ret += "-"; break;
		 case OP_SHL: ret += "<<"; break;
		 case OP_SHR: ret += ">>"; break;
		 case OP_AND: ret += "&"; break;
		 case OP_OR: ret += "|"; break;
		 case OP_XOR: ret += "^"; break;
		 case OP_COMMA: ret += ","; break;
		 case OP_ASSIGN: ret += "="; break;
		 case OP_MUL_ASSIGN: ret += "*="; break;
		 case OP_DIV_ASSIGN: ret += "/="; break;
		 case OP_MOD_ASSIGN: ret += "%="; break;
		 case OP_ADD_ASSIGN: ret += "+="; break;
		 case OP_SUB_ASSIGN: ret += "-="; break;
		 case OP_SHL_ASSIGN: ret += "<<="; break;
		 case OP_SHR_ASSIGN: ret += ">>="; break;
		 case OP_AND_ASSIGN: ret += "&="; break;
		 case OP_OR_ASSIGN: ret += "|="; break;
		 case OP_XOR_ASSIGN: ret += "^="; break;
		}
		ret += " " + m_rhs->to_string();
		return ret;
	}

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

	virtual string to_string() const
	{
		return "(" + m_condition->to_string() + ") ? "
		     + "(" + m_true->to_string() + ")" + " : "
		     + "(" + m_false->to_string() + ")";
	}

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

	string to_string() const
	{
		return m_type->to_string() + " " + m_ident->to_string();
	}

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

	//FIXME: move to Datum class
	static string datum_to_string(const Variable::Datum *datum)
	{
		string ret;
		switch (datum->type()->primitive()) {
		 case Type::BOOL:
			ret += sprintfxx("%s",
			    datum->bool_value() ? "true" : "false");
			break;
		 case Type::FLDFMT:
			//FIXME: fldfmt
			break;
		 case Type::FUNC: {
			const Variable::Func *f = datum->func_value();
			(void)f;//FIXME: print functions
			break;
		 }
		 case Type::INT:
			ret += sprintfxx("%d", datum->int_value());
			break;
		 case Type::LIST: {
			ret += "[ ";
			const Variable::List &l = datum->list_value();
			const std::vector<Variable*> &v = l.contents();
			for (size_t i = 0; i < v.size(); i++) {
				const Variable::Datum *d = v[i]->value();
				ret += datum_to_string(d);
			}
			ret += " ]";
			break;
		 }
		 case Type::STRING:
			ret += sprintfxx("\"%s\"", datum->string_value());
			break;
		 case Type::TUPLE: {
			ret += "[ ";
			const Variable::Tuple &t = datum->tuple_value();
			const std::vector<Variable*> &v = t.contents();
			for (size_t i = 0; i < v.size(); i++) {
				const Variable::Datum *d = v[i]->value();
				ret += datum_to_string(d);
			}
			ret += " ]";
			break;
		 }
		 case Type::VAR:
			ret += "undef";
			break;
		}
		return ret;
	}

	virtual string to_string() const
	{
		return datum_to_string(m_value.get());
	}

    private:
	boost::scoped_ptr<const Variable::Datum> m_value;
};

class FunctionLiteralExpression : public Expression {
    public:
	FunctionLiteralExpression() : m_body(NULL)
	{
	}
	FunctionLiteralExpression(StatementList *body) : m_body(body)
	{
		DASSERT(body);
	}
	FunctionLiteralExpression(ParameterDeclarationList *params,
	                          StatementList *body)
	    : m_params(params), m_body(body)
	{
		DASSERT(params);
		DASSERT(body);
	}

	StatementList *body()
	{
		return m_body.get();
	}

	virtual void evaluate(Variable *out_result);

	//FIXME: grammar: take ()${} or $(){} as args-spec to a func literal?
	virtual string to_string() const
	{
		string ret;
		if (m_params) {
			ret += "(";
			for (size_t i = 0; i < m_params->size(); i++) {
				if (i > 0) {
					ret += ", ";
				}
				ret += m_params->at(i)->to_string();
			}
			ret += ")";
		}
		ret += "${";
		if (m_body) {
			ret += "\n";
			for (size_t i = 0; i < m_body->size(); i++) {
				if (i > 0) {
					ret += ";\n";
				}
				ret += m_body->at(i)->to_string();
			}
		}
		ret += "}\n";
		return ret;
	}

    private:
	boost::scoped_ptr<ParameterDeclarationList> m_params;
	boost::scoped_ptr<StatementList> m_body;
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

	virtual string to_string() const
	{
		string ret = "[";
		if (m_contents) {
			for (size_t i = 0; i < m_contents->size(); i++) {
				if (i > 0) {
					ret += ", ";
				}
				ret += m_contents->at(i)->to_string();
			}
		}
		ret += "]";
		return ret;
	}

    private:
	boost::scoped_ptr<ArgumentList> m_contents;
};

}  // namespace syntax
}  // namespace language
}  // namespace pp

#endif  // PP_LANGAUGE_SYNTAX_TREE_H__
