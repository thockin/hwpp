#include "language/variable.h"
#include "hwpp.h"
#include <string>
#include <boost/smart_ptr.hpp>

namespace hwpp {
namespace language {

Variable::Datum::Datum(const Type &type) : m_type(type), m_type_locked(true)
{
	switch (m_type.primitive()) {
	  case Type::BOOL:
		m_bool_value = false;
		break;
	  case Type::FLDFMT:
		//FIXME: fldfmt
		break;
	  case Type::FUNC:
		m_func_value.reset(new UndefFunc());
		break;
	  case Type::INT:
		m_int_value = 0;
		break;
	  case Type::LIST:
		m_list_value.reset(new List());
		break;
	  case Type::STRING:
		m_string_value = "";
		break;
	  case Type::TUPLE:
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
    : m_type(other.type()), m_type_locked(true)
{
	switch (m_type.primitive()) {
	  case Type::BOOL:
		m_bool_value = other.bool_value();
		break;
	  case Type::FLDFMT:
		//FIXME: fldfmt
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
Variable::Datum::Datum(const Type &type, bool value)
    : m_type(type), m_type_locked(true)
{
	if (m_type.primitive() == Type::VAR) {
		m_type_locked = false;
		m_type.reinit(Type::BOOL);
	}
	if (m_type.primitive() != Type::BOOL) {
		throw TypeError(sprintfxx(
		    "can't init value of type '%s' as bool",
		    m_type.to_string()));
	}
	m_bool_value = value;
}

// Initialize as int.
Variable::Datum::Datum(const Type &type, const Value &value)
    : m_type(type), m_type_locked(true)
{
	if (m_type.primitive() == Type::VAR) {
		m_type_locked = false;
		m_type.reinit(Type::INT);
	}
	if (m_type.primitive() != Type::INT) {
		throw TypeError(sprintfxx(
		    "can't init value of type '%s' as int",
		    m_type.to_string()));
	}
	m_int_value = value;
}

// Initialize as func.
Variable::Datum::Datum(const Type &type, const Func *value)
    : m_type(type), m_type_locked(true)
{
	if (m_type.primitive() == Type::VAR) {
		m_type_locked = false;
		m_type.reinit(Type::FUNC);
	}
	if (m_type.primitive() != Type::FUNC) {
		throw TypeError(sprintfxx(
		    "can't init value of type '%s' as func",
		    m_type.to_string()));
	}
	m_func_value.reset(value);
}

// Initialize as list.
Variable::Datum::Datum(const Type &type, const List *value)
    : m_type(type), m_type_locked(true)
{
	if (m_type.primitive() == Type::VAR) {
		m_type_locked = false;
		m_type.reinit(Type::LIST);
	}
	if (m_type.primitive() != Type::LIST) {
		throw TypeError(sprintfxx(
		    "can't init value of type '%s' as list",
		    m_type.to_string()));
	}
	// Validate that the new actual-types are OK for this list.
	const Type &spec_type = m_type.argument(0);
	const List::Contents &vars = value->contents();
	for (size_t i = 0; i < vars.size(); i++) {
		const Type &actual_type = vars[i]->type();
		if (!spec_type.is_assignable_from(actual_type)) {
			throw TypeError(sprintfxx(
			    "can't init value of type '%s' as '%s'",
			    spec_type.to_string(),
			    actual_type.to_string()));
		}
	}
	m_list_value.reset(value);
}

// Initialize as string.
Variable::Datum::Datum(const Type &type, const string &value)
    : m_type(type), m_type_locked(true)
{
	if (m_type.primitive() == Type::VAR) {
		m_type_locked = false;
		m_type.reinit(Type::STRING);
	}
	if (m_type.primitive() != Type::STRING) {
		throw TypeError(sprintfxx(
		    "can't init value of type '%s' as string",
		    m_type.to_string()));
	}
	m_string_value = value;
}

// Initialize as tuple.
Variable::Datum::Datum(const Type &type, const Tuple *value)
    : m_type(type), m_type_locked(true)
{
	if (m_type.primitive() == Type::VAR) {
		m_type_locked = false;
		m_type.reinit(Type::LIST);
	}
	if (m_type.primitive() != Type::TUPLE) {
		throw TypeError(sprintfxx(
		    "can't init value of type '%s' as list",
		    m_type.to_string()));
	}
	// Validate that the new actual-types are OK for this list.
	const Type &spec_type = m_type.argument(0);
	const Tuple::Contents &vars = value->contents();
	for (size_t i = 0; i < vars.size(); i++) {
		const Type &actual_type = vars[i]->type();
		if (!spec_type.is_assignable_from(actual_type)) {
			throw TypeError(sprintfxx(
			    "can't init value of type '%s' as '%s'",
			    spec_type.to_string(),
			    actual_type.to_string()));
		}
	}
	m_tuple_value.reset(value);
}

void
Variable::Datum::check_type_primitive(Type::Primitive primitive) const
{
	if (m_type.primitive() != primitive) {
		throw TypeError(
		    sprintfxx("can't access value of type '%s' as '%s'",
		              Type::primitive_to_string(m_type.primitive()),
		              Type::primitive_to_string(primitive)));
	}
}

}  // namespace language
}  // namespace hwpp

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
