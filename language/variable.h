#ifndef PP_LANGUAGE_VARIABLE_H__
#define PP_LANGUAGE_VARIABLE_H__

#include "pp/pp.h"
#include <string>
#include <boost/smart_ptr.hpp>

namespace pp {
namespace language {

// This represents a fully qualified datatype, including type arguments if
// applicable.
class Type {
    public:
	enum Primitive {
		BOOL,
		FUNC,
		INT,
		LIST,
		STRING,
		TUPLE,
		VAR,
	};

	// Throw this when there is a problem with type arguments.
	struct ArgumentError : public std::runtime_error {
		explicit
		ArgumentError(const string &str) : runtime_error(str)
		{
		}
	};
	// Throw this when there is an unknown type.
	struct UnknownError: public std::runtime_error {
		explicit
		UnknownError(const string &str) : runtime_error(str)
		{
		}
	};

	// Implicit conversion is OK.
	Type(Primitive prim) : m_primitive(prim), m_const(false)
	{
	}
	~Type()
	{
		reinit(VAR);
	}
	// Default copy ctor is OK.

	void
	reinit(Primitive prim)
	{
		// Clean up arguments.
		for (size_t i = 0; i < m_arguments.size(); i++) {
			delete m_arguments[i];
		}
		m_arguments.clear();
		m_primitive = prim;
		m_const = false;
	}

	// This takes ownership of the argument type object.
	void
	add_argument(const Type *arg)
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

	static string
	primitive_to_string(Primitive prim)
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
	to_string() const
	{
		return primitive_to_string(m_primitive);
	}

	bool
	is_equal_to(const Type *other) const
	{
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

	// Type assignability is strict.  There is no conversion between types
	// at all.  The only soft spot is variables with spec-type var, which
	// can be assigned from any type, but otherwise behave as their
	// actual-type.
	//   $type = $type			OK
	//   var = $type			OK
	//   $type = var			NOT OK
	//   list<$type> = list<$type>		OK
	//   list<var> = list<$type>		OK
	//   list<$type> = list<var>		NOT OK
	bool
	is_assignable_from(const Type *other) const
	{
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

	Primitive
	primitive() const
	{
		return m_primitive;
	}

	const std::vector<const Type*> &
	arguments() const {
		return m_arguments;
	}

	bool
	is_const() const
	{
		return m_const;
	}
	void
	set_const()
	{
		m_const = true;
	}

    private:
	// Paranoia.
	static void
	check_known_primitive(Primitive prim)
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

	Primitive m_primitive;
	std::vector<const Type*> m_arguments;
	bool m_const;
};
inline bool
operator==(const Type &lhs, const Type &rhs)
{
	return lhs.is_equal_to(&rhs);
}
inline bool
operator!=(const Type &lhs, const Type &rhs)
{
	return !(lhs.is_equal_to(&rhs));
}

//FIXME: need a Variable::Value (data) and Variable (inode).  Variable is
//  what is really const (consider passing as reference a non-const and
//  accepting it as const.
// This represents an instance of a specific datatype with value.
class Variable {
    public:
	// Throw this when there is a type mismatch or const violation.
	struct TypeError: public std::runtime_error {
		explicit
		TypeError(const string &str) : runtime_error(str)
		{
		}
	};

	// A base class for callable functions.
	class Func {
	    public:
		virtual ~Func()
		{
		}

		//FIXME: need args;
		virtual Variable *call() = 0;
	};
	typedef std::vector<Variable*> List;
	typedef std::vector<Variable*> Tuple;

	// Initialize to the default value for a given type.
	explicit
	Variable(Type *type) : m_type(type), m_type_locked(true)
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
		  	m_vector_value.reset(new List());
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
		  	m_vector_value.reset(new Tuple());
		  	break;
		  case Type::VAR:
		  	// You can create a 'var' Variable, but once you
			// assign to it, its actual type changes.  In the mean
			// time its value is undefined.
		  	m_type_locked = false;
		  	break;
		}
	}
	// Initialize a specific type with a given value.
	Variable(Type *type, bool value)
	    : m_type(type), m_type_locked(true)
	{
		if (m_type->primitive() == Type::VAR) {
			m_type_locked = false;
			bool is_const = m_type->is_const();
			m_type->reinit(Type::BOOL);
			if (is_const) {
				m_type->set_const();
			}
		}
		if (m_type->primitive() != Type::BOOL) {
			throw TypeError(sprintfxx(
			    "can't init variable of type %s as bool",
			    m_type->to_string()));
		}
		m_bool_value = value;
	}
	Variable(Type *type, const Value &value)
	    : m_type(type), m_type_locked(true)
	{
		if (m_type->primitive() == Type::VAR) {
			m_type_locked = false;
			bool is_const = m_type->is_const();
			m_type->reinit(Type::INT);
			if (is_const) {
				m_type->set_const();
			}
		}
		if (m_type->primitive() != Type::INT) {
			throw TypeError(sprintfxx(
			    "can't init variable of type %s as int",
			    m_type->to_string()));
		}
		m_int_value = value;
	}
	Variable(Type *type, const Func *value)
	    : m_type(type), m_type_locked(true)
	{
		if (m_type->primitive() == Type::VAR) {
			m_type_locked = false;
			bool is_const = m_type->is_const();
			m_type->reinit(Type::FUNC);
			if (is_const) {
				m_type->set_const();
			}
		}
		if (m_type->primitive() != Type::FUNC) {
			throw TypeError(sprintfxx(
			    "can't init variable of type %s as func",
			    m_type->to_string()));
		}
		m_func_value.reset(value);
	}
	Variable(Type *type, const List *value)
	    : m_type(type), m_type_locked(true)
	{
		if (m_type->primitive() == Type::VAR) {
			m_type_locked = false;
			bool is_const = m_type->is_const();
			m_type->reinit(Type::LIST);
			if (is_const) {
				m_type->set_const();
			}
		}
		if (m_type->primitive() != Type::LIST) {
			throw TypeError(sprintfxx(
			    "can't init variable of type %s as list",
			    m_type->to_string()));
		}
		// Validate that the new actual-types are OK for this list.
		const Type *spec_type = m_type->arguments()[0];
		for (size_t i = 0; i < value->size(); i++) {
			const Type *actual_type = value->at(i)->type();
			if (!spec_type->is_assignable_from(actual_type)) {
				throw TypeError(sprintfxx(
				    "can't init variable of type %s as %s",
				    spec_type->to_string(),
				    actual_type->to_string()));
			}
		}
		m_vector_value.reset(value);
	}
	Variable(Type *type, const string &value)
	    : m_type(type), m_type_locked(true)
	{
		if (m_type->primitive() == Type::VAR) {
			m_type_locked = false;
			bool is_const = m_type->is_const();
			m_type->reinit(Type::STRING);
			if (is_const) {
				m_type->set_const();
			}
		}
		if (m_type->primitive() != Type::STRING) {
			throw TypeError(sprintfxx(
			    "can't init variable of type %s as string",
			    m_type->to_string()));
		}
		m_string_value = value;
	}

	bool
	is_const() const
	{
		return m_type->is_const();
	}

	const Type *
	type() const
	{
		return m_type.get();
	}

#if 0
	// Access this variable as a bool.
	bool
	bool_value() const
	{
		check_type_primitive(Type::BOOL);
		return m_bool_value;
	}
	void
	set_bool_value(bool new_value)
	{
		if (m_type_locked) {
			check_type_primitive(Type::BOOL);
		}
		check_not_const();
		//FIXME: wasteful to reallocate if not needed
		m_type->reinit(Type::BOOL);
		m_bool_value = new_value;
	}

	// Access this variable as a function.
	const Func *
	func_value() const
	{
		check_type_primitive(Type::FUNC);
		return m_func_value.get();
	}
	void
	set_func_value(const Func *new_value)
	{
		if (m_type_locked) {
			check_type_primitive(Type::FUNC);
		}
		check_not_const();
		m_type->reinit(Type::FUNC);
		m_func_value.reset(new_value);
	}

	// Access this variable as an int.
	const Value &
	int_value() const
	{
		check_type_primitive(Type::INT);
		return m_int_value;
	}
	void
	set_int_value(const Value &new_value)
	{
		if (m_type_locked) {
			check_type_primitive(Type::INT);
		}
		check_not_const();
		m_type->reinit(Type::INT);
		m_int_value = new_value;
	}

	// Access this variable as a string.
	const string &
	string_value() const
	{
		check_type_primitive(Type::STRING);
		return m_string_value;
	}
	void
	set_string_value(const string &new_value)
	{
		if (m_type_locked) {
			check_type_primitive(Type::STRING);
		}
		check_not_const();
		m_type->reinit(Type::STRING);
		m_string_value = new_value;
	}

	// Access this variable as a list.
	const List *
	list_value() const
	{
		check_type_primitive(Type::LIST);
		return m_vector_value.get();
	}
	void
	set_list_value(List *new_value)
	{
		if (m_type_locked) {
			//FIXME: dup comment
			// Careful here:
			//   list<int> = list<int> is always OK
			//   list<int> = list<string> is never OK
			//   list<var> = list<var> is always OK
			//   list<var> = list<int> is always OK
			//   list<int> = list<var> is OK iff all values are int
			check_type_primitive(Type::LIST);
			const Type *contained_type = m_type->arguments()[0];
			// We don't have the new list's spec'ed type, so we
			// have to iterate the contents and compare actual
			// types.
			for (size_t i = 0; i < new_value->size(); i++) {
				const Type *t = new_value->at(i)->type();
				if (!contained_type->is_assignable_from(t)) {
					//FIXME: left off here
					//throw TypeError(sprintfxx(
					//    "can't init variable of type %s as bool",
					//    m_type->to_string()));
				}
			}
		}
		check_not_const();
		//FIXME: handle var
		//m_type->reinit(Type::LIST);
		m_vector_value.reset(new_value);
	}
#endif

    private:
	class UndefFunc : public Func {
	    public:
		virtual Variable *
		call() {
			//FIXME: make undef be a value rather than a type?
			return new Variable(new Type(Type::VAR));
		}
	};

	void
	check_type_primitive(Type::Primitive primitive) const
	{
		if (m_type->primitive() != primitive) {
			throw TypeError(
			    sprintfxx("can't access variable of type %s as %s",
			        Type::primitive_to_string(m_type->primitive()),
			        Type::primitive_to_string(primitive)));
		}
	}

	void
	check_not_const() const
	{
		if (is_const()) {
			throw TypeError(
			    sprintfxx("can't write to const variable"));
		}
	}

	boost::scoped_ptr<Type> m_type;
	bool m_type_locked;
	bool m_bool_value;
	boost::scoped_ptr<const Func> m_func_value;
	Value m_int_value;
	boost::scoped_ptr<const List> m_vector_value;
	string m_string_value;
};

}  // namespace language
}  // namespace pp

#endif  // PP_LANGUAGE_VARIABLE_H__
