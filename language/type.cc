#include "language/type.h"
#include "hwpp.h"
#include <string>
#include <boost/smart_ptr.hpp>

namespace hwpp {
namespace language {

Type::Type(const Type &other)
{
	m_primitive = other.m_primitive;
	m_constness = other.m_constness;
	for (size_t i = 0; i < other.m_arguments.size(); i++) {
		m_arguments.push_back(other.m_arguments[i]);
	}
}

Type *
Type::reinit(Primitive prim)
{
	// Clean up arguments.
	m_arguments.clear();
	m_primitive = prim;
	m_constness = 0;
	return this;
}
Type *
Type::reinit(Primitive prim, Type::Constness constness)
{
	// Clean up arguments.
	m_arguments.clear();
	m_primitive = prim;
	m_constness = constness;
	return this;
}

Type *
Type::add_argument(const Type &arg)
{
	switch (m_primitive) {
	 case LIST:
		if (m_arguments.size() > 0) {
			throw ArgumentError(sprintfxx(
			    "type '%s' takes zero or one arguments",
			    primitive_to_string(m_primitive)));
		}
		// ...else fall through
	 case TUPLE:
		m_arguments.push_back(arg);
		break;
	 default:
		throw ArgumentError(sprintfxx(
		    "type '%s' takes no arguments",
		    primitive_to_string(m_primitive)));
		break;
	}
	return this;
}

static void
check_known_primitive(Type::Primitive prim)
{
	switch (prim) {
	  case Type::BOOL:
	  case Type::FLDFMT:
	  case Type::FUNC:
	  case Type::INT:
	  case Type::LIST:
	  case Type::STRING:
	  case Type::TUPLE:
	  case Type::VAR:
		return;
	}
	throw Type::UnknownError(sprintfxx("unknown type (%d)", prim));
}

static int
primitive_max_args(Type::Primitive prim)
{
	switch (prim) {
	 case Type::BOOL:
	 case Type::FLDFMT:
	 case Type::FUNC:
	 case Type::INT:
	 case Type::STRING:
	 case Type::VAR:
		return 0;
	 case Type::LIST:
		return 1;
	 case Type::TUPLE:
		return INT_MAX;
	}
	throw Type::UnknownError(sprintfxx("unknown type (%d)", prim));
}

string
Type::primitive_to_string(Primitive prim)
{
	check_known_primitive(prim);
	switch (prim) {
	  case BOOL:   return "bool";
	  case FLDFMT: return "fldfmt";
	  case FUNC:   return "func";
	  case INT:    return "int";
	  case LIST:   return "list";
	  case STRING: return "string";
	  case TUPLE:  return "tuple";
	  case VAR:    return "var";
	}
	throw Type::UnknownError(sprintfxx("unknown type (%d)", prim));
}

string
Type::to_string() const
{
	string str = (is_const() ? "const " : "")
	    + primitive_to_string(m_primitive);
	if (primitive_max_args(m_primitive) > 0) {
		str += "<";
	}
	for (size_t i = 0; i < m_arguments.size(); i++) {
		if (i > 0) {
			str += ", ";
		}
		str += m_arguments[i].to_string();
	}
	if (primitive_max_args(m_primitive) > 0) {
		str += ">";
	}
	return str;
}

bool
Type::is_equal_to(const Type &other) const
{
	if (is_const() != other.is_const()) {
		return false;
	}
	if (m_primitive != other.m_primitive) {
		return false;
	}
	if (m_arguments.size() != other.m_arguments.size()) {
		return false;
	}
	for (size_t i = 0; i < m_arguments.size(); i++) {
		const Type &a = m_arguments[i];
		const Type &b = other.m_arguments[i];
		if (!a.is_equal_to(b)) {
			return false;
		}
	}
	return true;
}

// See comment in header.
bool
Type::is_assignable_from(const Type &other, IgnoreConst ignore_const) const
{
	if (!ignore_const) {
		// Const types are never assignable.
		if (is_const()) {
			return false;
		}
	}
	// VAR is assignable from anything.  Assignments from VAR are checked
	// at runtime.
	if (m_primitive == VAR || other.m_primitive == VAR) {
		return true;
	}
	// Special case: LIST with no type args can be assigned from any LIST or
	// TUPLE.
	if (m_primitive == LIST && m_arguments.size() == 0) {
		if (other.m_primitive == LIST || other.m_primitive == TUPLE) {
			return true;
		}
		return false;
	}
	// Special case: TUPLE with no type args can be assigned from any TUPLE.
	if (m_primitive == TUPLE && m_arguments.size() == 0) {
		if (other.m_primitive == TUPLE) {
			return true;
		}
		return false;
	}
	// Special case: LIST is assignable from TUPLE, as long as the type-args
	// allow it.
	if (m_primitive == LIST && other.m_primitive == TUPLE) {
		// We can only assign LIST = TUPLE if the LIST type-arg is assignable
		// from all of the TUPLE type-args.
		const Type &lhs = m_arguments[0];
		for (size_t i = 0; i < other.m_arguments.size(); i++) {
			const Type &rhs = other.m_arguments[i];
			if (!lhs.is_assignable_from(rhs, ignore_const)) {
				return false;
			}
		}
	} else {
		// If the primitives don't match, it can't be assignable.
		if (m_primitive != other.m_primitive) {
			return false;
		}
		// If the number of type-args don't match, it can't be assignable.
		if (m_arguments.size() != other.m_arguments.size()) {
			// Special case: a TUPLE can be initialized from a literal that is
			// a strict prefix of the TUPLE's type.
			if (m_primitive == TUPLE && other.m_primitive == TUPLE
			 && m_arguments.size() > other.m_arguments.size()
			 && other.is_literal()) {
				// The code is easier to read this way.
			} else {
				return false;
			}
		}
		// This allows the strict prefix init of tuples.
		size_t n_args = std::min(m_arguments.size(), other.m_arguments.size());
		// The type-args must also be assignable.
		for (size_t i = 0; i < n_args; i++) {
			const Type &lhs = m_arguments[i];
			const Type &rhs = other.m_arguments[i];
			if (!lhs.is_assignable_from(rhs, ignore_const)) {
				return false;
			}
		}
	}
	return true;
}
bool
Type::is_assignable_from(const Type &other) const
{
	return is_assignable_from(other, DONT_IGNORE_CONST);
}

bool
Type::is_initializable_from(const Type &other) const
{
	return is_assignable_from(other, IGNORE_CONST);
}

}  // namespace language
}  // namespace hwpp

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
