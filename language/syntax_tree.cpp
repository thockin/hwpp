#include "pp/language/syntax_tree.h"

#include "pp/pp.h"
#include <map>
#include <string>
#include <vector>
#include "pp/language/variable.h"

namespace pp {
namespace language {
namespace syntax {

bool
NullStatement::execute()
{
	return true;
}

bool
CompoundStatement::execute()
{
	//FIXME:
	return true;
}

bool
ExpressionStatement::execute()
{
	if (expression()) {
		// NULL statement.
		return true;
	}
	//FIXME:
	return true;
}

bool
ConditionalStatement::execute()
{
	//FIXME:
	return false;
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

bool
GotoStatement::execute()
{
	//FIXME:
	return false;
}

bool
CaseStatement::execute()
{
	//FIXME:
	return false;
}

bool
ReturnStatement::execute()
{
	//FIXME:
	return false;
}

bool
DefinitionStatement::execute()
{
	//FIXME:
	return true;
}

bool
ImportStatement::execute()
{
	//FIXME:
	return true;
}

bool
ModuleStatement::execute()
{
	//FIXME:
	return true;
}

bool
DiscoverStatement::execute()
{
	//FIXME:
	return true;
}

void
IdentifierExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

void
SubscriptExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

void
FunctionCallExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

void
UnaryExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

void
BinaryExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

void
ConditionalExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

void
ValueExpression::evaluate(Variable *out_result)
{
	//FIXME:
	(void)out_result;
	//*out_result = m_value;
}

void
FunctionLiteralExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

void
ListLiteralExpression::evaluate(Variable *out_result)
{
	(void)out_result;//FIXME:
}

}  // namespace syntax
}  // namespace language
}  // namespace pp
