#include "language/syntax_tree.h"

#include "pp.h"
#include <map>
#include <string>
#include <vector>
#include "language/type.h"
#include "language/variable.h"

namespace pp {
namespace language {
namespace syntax {

string
Identifier::to_string() const
{
	return m_module + (m_module == "" ? "" : ".") + m_symbol;
}

string
InitializedIdentifier::to_string() const
{
	string ret = m_ident->to_string();
	if (m_init) {
		ret += " = " + m_init->to_string() + "\n";
	}
	return ret;
}

string
Statement::to_string() const
{
	string ret;
	for (size_t i = 0; i < m_labels.size(); i++) {
		ret += m_labels[i]->to_string() + ":\n";
	}
	return ret;
}

string
Argument::to_string() const
{
	return (m_name ? m_name->to_string() + ":" : "")
	     + m_expr->to_string();
}

string
NullStatement::to_string() const
{
	string ret = Statement::to_string();
	return ret + "(undef);";
}

bool
NullStatement::execute()
{
	return true;
}

string
CompoundStatement::to_string() const
{
	string ret = Statement::to_string();
	ret += "{\n";
	for (size_t i = 0; i < m_body->size(); i++) {
		ret += m_body->at(i)->to_string();
	}
	ret += "}\n";
	return ret;
}

bool
CompoundStatement::execute()
{
	//FIXME:
	return true;
}

string
ExpressionStatement::to_string() const
{
	string ret = Statement::to_string();
	return m_expr->to_string();
}

bool
ExpressionStatement::execute()
{
	//FIXME:
	return true;
}

string
ConditionalStatement::to_string() const
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

bool
ConditionalStatement::execute()
{
	//FIXME:
	return false;
}

string
SwitchStatement::to_string() const
{
	string ret = Statement::to_string();
	ret += "switch (" + m_condition->to_string() + ") {\n";
	ret += m_body->to_string();
	ret += "}\n";
	return ret;
}

bool
SwitchStatement::execute()
{
	//FIXME:
	return false;
}

bool
LoopStatement::execute()
{
	//FIXME:
	return false;
}

string
WhileLoopStatement::to_string() const
{
	string ret = Statement::to_string();
	ret += "while (" + expression()->to_string() + ") {\n";
	ret += body()->to_string();
	ret += "}\n";
	return ret;
}

string
DoWhileLoopStatement::to_string() const
{
	string ret = Statement::to_string();
	ret += "do {\n";
	ret += body()->to_string();
	ret += " } while (" + expression()->to_string() + ")\n";
	return ret;
}

string
GotoStatement::to_string() const
{
	string ret = Statement::to_string();
	ret += "goto " + m_target->to_string() + ";\n";
	return ret;
}

bool
GotoStatement::execute()
{
	//FIXME:
	return false;
}

string
CaseStatement::to_string() const
{
	string ret = Statement::to_string();
	ret += "case (" + m_expr->to_string() + "):\n";
	return ret;
}

bool
CaseStatement::execute()
{
	//FIXME:
	return false;
}

string
ReturnStatement::to_string() const
{
	string ret = Statement::to_string();
	ret += "return (" + m_expr->to_string() + ");\n";
	return ret;
}

bool
ReturnStatement::execute()
{
	//FIXME:
	return false;
}

string
DefinitionStatement::to_string() const
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

bool
DefinitionStatement::execute()
{
	//FIXME:
	return true;
}

string
ImportStatement::to_string() const
{
	string ret = Statement::to_string();
	ret += "import \"" + m_argument + "\";\n";
	return ret;
}

bool
ImportStatement::execute()
{
	//FIXME:
	return true;
}

string
ModuleStatement::to_string() const
{
	string ret = Statement::to_string();
	ret += "module " + m_name + ";\n";
	return ret;
}

bool
ModuleStatement::execute()
{
	//FIXME:
	return true;
}

string
DiscoverStatement::to_string() const
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

bool
DiscoverStatement::execute()
{
	//FIXME:
	return true;
}

string
IdentifierExpression::to_string() const
{
	return m_ident->to_string();
}

void
IdentifierExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

string
SubscriptExpression::to_string() const
{
	return "(" + m_expr->to_string() + ")"
	     + "[" + m_index->to_string() + "]";
}

void
SubscriptExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

string
FunctionCallExpression::to_string() const
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

void
FunctionCallExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

string
UnaryExpression::to_string() const
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

void
UnaryExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

string
BinaryExpression::to_string() const
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

void
BinaryExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

string
ConditionalExpression::to_string() const
{
	return "(" + m_condition->to_string() + ") ? "
	     + "(" + m_true->to_string() + ")" + " : "
	     + "(" + m_false->to_string() + ")";
}

void
ConditionalExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

string
ParameterDeclaration::to_string() const
{
	return m_type->to_string() + " " + m_ident->to_string();
}

static string
datum_to_string(const Variable::Datum *datum)
{
	string ret;
	switch (datum->type().primitive()) {
	 case Type::BOOL:
		ret += sprintfxx("%s",
		    datum->bool_value() ? "true" : "false");
		break;
	 case Type::FLDFMT:
		//FIXME: print a fldfmt
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

string
ValueExpression::to_string() const
{
	return datum_to_string(m_value.get());
}

void
ValueExpression::evaluate(Variable *out_result)
{
	//FIXME:
	(void)out_result;
	//*out_result = m_value;
}

string
FunctionLiteralExpression::to_string() const
{
	string ret = "$";
	ret += "(";
	for (size_t i = 0; i < m_params->size(); i++) {
		if (i > 0) {
			ret += ", ";
		}
		ret += m_params->at(i)->to_string();
	}
	ret += ")";
	ret += m_body->to_string();
	return ret;
}

void
FunctionLiteralExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

string
TupleLiteralExpression::to_string() const
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

void
TupleLiteralExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

}  // namespace syntax
}  // namespace language
}  // namespace pp

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
