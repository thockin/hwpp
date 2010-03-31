#include "pp/language/variable.h"
#include "pp/pp.h"
#include <string>
#include <boost/smart_ptr.hpp>

namespace pp {
namespace language {

//
// Type
//

Type::Type(const Type &other)
{
	m_primitive = other.m_primitive;
	m_is_const = other.m_is_const;
	for (size_t i = 0; i < other.m_arguments.size(); i++) {
		m_arguments.push_back(new Type(*(other.m_arguments[i])));
	}
}

void
Type::reinit(Primitive prim)
{
	// Clean up arguments.
	for (size_t i = 0; i < m_arguments.size(); i++) {
		delete m_arguments[i];
	}
	m_arguments.clear();
	m_primitive = prim;
	m_is_const = false;
}

void
Type::add_argument(const Type *arg)
{
	switch (m_primitive) {
		case LIST:
			if (m_arguments.size() > 0) {
				throw ArgumentError(sprintfxx(
				    "type %s takes only one argument",
				    primitive_to_string(m_primitive)));
			}
			// ...else fall through
		case TUPLE:
			m_arguments.push_back(arg);
			break;
		default:
			throw ArgumentError(sprintfxx(
			    "type %s takes no arguments",
			    primitive_to_string(m_primitive)));
			break;
	}
}

string
Type::primitive_to_string(Primitive prim)
{
	check_known_primitive(prim);
	switch (prim) {
	  case BOOL:   return "bool";
	  case FUNC:   return "func";
	  case INT:    return "int";
	  case LIST:   return "list";
	  case STRING: return "string";
	  case TUPLE:  return "tuple";
	  case VAR:    return "var";
	}
	return "(this can not happen)";
}

string
Type::to_string() const
{
	return (m_is_const ? "const " : "")
	    + primitive_to_string(m_primitive);
}

bool
Type::is_equal_to(const Type *other) const
{
	if (m_is_const != other->m_is_const) {
		return false;
	}
	if (m_primitive != other->m_primitive) {
		return false;
	}
	if (m_arguments.size() != other->m_arguments.size()) {
		return false;
	}
	for (size_t i = 0; i < m_arguments.size(); i++) {
		const Type *a = m_arguments[i];
		const Type *b = other->m_arguments[i];
		if (!a->is_equal_to(b)) {
			return false;
		}
	}
	return true;
}

// See comment in header.
bool
Type::is_assignable_from(const Type *other) const
{
	// Const types are never assignable.
	if (m_is_const) {
		return false;
	}
	// VAR is assignable from anything.
	if (m_primitive == VAR) {
		return true;
	}
	// If the primitives don't match or the number of arguments
	// don't match, it can't possibly be assignable.
	if (m_primitive != other->m_primitive
	 || m_arguments.size() != other->m_arguments.size()) {
		return false;
	}
	// All arguments must also be assignable.
	for (size_t i = 0; i < m_arguments.size(); i++) {
		const Type *lhs = m_arguments[i];
		const Type *rhs = other->m_arguments[i];
		if (!lhs->is_assignable_from(rhs)) {
			return false;
		}
	}
	return true;
}

void
Type::check_known_primitive(Primitive prim)
{
	switch (prim) {
	  case BOOL:
	  case FUNC:
	  case INT:
	  case LIST:
	  case STRING:
	  case TUPLE:
	  case VAR:
		return;
	}
	throw UnknownError(sprintfxx("unknown type (%d)", prim));
}

//
// Variable::Datum
//

Variable::Datum::Datum(Type *type) : m_type(type), m_type_locked(true)
{
	switch (m_type->primitive()) {
	  case Type::BOOL:
		m_bool_value = false;
		break;
	  case Type::FUNC:
		m_func_value.reset(new UndefFunc());
		break;
	  case Type::INT:
		m_int_value = 0;
		break;
	  case Type::LIST:
		if (type->arguments().size() != 1) {
			throw Type::ArgumentError(sprintfxx(
			    "type %s requires exactly one argument",
			    Type::primitive_to_string(Type::LIST)));
		}
		m_list_value.reset(new List());
		break;
	  case Type::STRING:
		m_string_value = "";
		break;
	  case Type::TUPLE:
		if (type->arguments().size() < 1) {
			throw Type::ArgumentError(sprintfxx(
			    "type %s requires at least one argument",
			    Type::primitive_to_string(Type::TUPLE)));
		}
		m_tuple_value.reset(new Tuple());
		break;
	  case Type::VAR:
		// You can create a 'var' Datum, but once you
		// assign to it, its actual type changes.  In the mean
		// time its value is undefined.
		m_type_locked = false;
		break;
	}
}

// Make a deep-copy of another Datum.
Variable::Datum::Datum(const Datum &other)
    : m_type(new Type(*(other.type()))), m_type_locked(true)
{
	switch (m_type->primitive()) {
	  case Type::BOOL:
		m_bool_value = other.bool_value();
		break;
	  case Type::FUNC:
		// func is a shared_ptr, since funcs are immutable
		m_func_value = other.m_func_value;
		break;
	  case Type::INT:
		m_int_value = other.int_value();
		break;
	  case Type::LIST:
		// deep copy
		m_list_value.reset(
		    new List(other.list_value()));
		break;
	  case Type::STRING:
		m_string_value = other.string_value();
		break;
	  case Type::TUPLE:
		// deep copy
		m_tuple_value.reset(
		    new Tuple(other.tuple_value()));
		break;
	  case Type::VAR:
		m_type_locked = false;
		break;
	}
}

// Initialize as bool.
Variable::Datum::Datum(Type *type, bool value)
    : m_type(type), m_type_locked(true)
{
	if (m_type->primitive() == Type::VAR) {
		m_type_locked = false;
		m_type->reinit(Type::BOOL);
	}
	if (m_type->primitive() != Type::BOOL) {
		throw TypeError(sprintfxx(
		    "can't init value of type %s as bool",
		    m_type->to_string()));
	}
	m_bool_value = value;
}

// Initialize as int.
Variable::Datum::Datum(Type *type, const Value &value)
    : m_type(type), m_type_locked(true)
{
	if (m_type->primitive() == Type::VAR) {
		m_type_locked = false;
		m_type->reinit(Type::INT);
	}
	if (m_type->primitive() != Type::INT) {
		throw TypeError(sprintfxx(
		    "can't init value of type %s as int",
		    m_type->to_string()));
	}
	m_int_value = value;
}

// Initialize as func.
Variable::Datum::Datum(Type *type, const Func *value)
    : m_type(type), m_type_locked(true)
{
	if (m_type->primitive() == Type::VAR) {
		m_type_locked = false;
		m_type->reinit(Type::FUNC);
	}
	if (m_type->primitive() != Type::FUNC) {
		throw TypeError(sprintfxx(
		    "can't init value of type %s as func",
		    m_type->to_string()));
	}
	m_func_value.reset(value);
}

// Initialize as list.
Variable::Datum::Datum(Type *type, const List *value)
    : m_type(type), m_type_locked(true)
{
	if (m_type->primitive() == Type::VAR) {
		m_type_locked = false;
		m_type->reinit(Type::LIST);
	}
	if (m_type->primitive() != Type::LIST) {
		throw TypeError(sprintfxx(
		    "can't init value of type %s as list",
		    m_type->to_string()));
	}
	// Validate that the new actual-types are OK for this list.
	const Type *spec_type = m_type->arguments()[0];
	const List::Contents &vars = value->contents();
	for (size_t i = 0; i < vars.size(); i++) {
		const Type *actual_type = vars[i]->type();
		if (!spec_type->is_assignable_from(actual_type)) {
			throw TypeError(sprintfxx(
			    "can't init value of type %s as %s",
			    spec_type->to_string(),
			    actual_type->to_string()));
		}
	}
	m_list_value.reset(value);
}

// Initialize as string.
Variable::Datum::Datum(Type *type, const string &value)
    : m_type(type), m_type_locked(true)
{
	if (m_type->primitive() == Type::VAR) {
		m_type_locked = false;
		m_type->reinit(Type::STRING);
	}
	if (m_type->primitive() != Type::STRING) {
		throw TypeError(sprintfxx(
		    "can't init value of type %s as string",
		    m_type->to_string()));
	}
	m_string_value = value;
}

// Initialize as tuple.
Variable::Datum::Datum(Type *type, const Tuple *value)
    : m_type(type), m_type_locked(true)
{
	if (m_type->primitive() == Type::VAR) {
		m_type_locked = false;
		m_type->reinit(Type::LIST);
	}
	if (m_type->primitive() != Type::TUPLE) {
		throw TypeError(sprintfxx(
		    "can't init value of type %s as list",
		    m_type->to_string()));
	}
	// Validate that the new actual-types are OK for this list.
	const Type *spec_type = m_type->arguments()[0];
	const Tuple::Contents &vars = value->contents();
	for (size_t i = 0; i < vars.size(); i++) {
		const Type *actual_type = vars[i]->type();
		if (!spec_type->is_assignable_from(actual_type)) {
			throw TypeError(sprintfxx(
			    "can't init value of type %s as %s",
			    spec_type->to_string(),
			    actual_type->to_string()));
		}
	}
	m_tuple_value.reset(value);
}

void
Variable::Datum::check_type_primitive(Type::Primitive primitive) const
{
	if (m_type->primitive() != primitive) {
		throw TypeError(
		    sprintfxx("can't access value of type %s as %s",
		              Type::primitive_to_string(m_type->primitive()),
		              Type::primitive_to_string(primitive)));
	}
}

}  // namespace language
}  // namespace pp
