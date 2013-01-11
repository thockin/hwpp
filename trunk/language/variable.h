#ifndef HWPP_LANGUAGE_VARIABLE_H__
#define HWPP_LANGUAGE_VARIABLE_H__

#include "hwpp.h"
#include <string>
#include <boost/smart_ptr.hpp>
#include "type.h"

namespace hwpp {
namespace language {

// This represents a reference to an instance of a specific datatype.  All
// Variables are passed by reference, so this is the primary construct.  The
// relationship between Variable and Variable::Datum is like that of inodes
// and data blocks.
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
	class Container {
	 public:
		typedef std::vector<Variable*> Contents;
		Container() : m_contents()
		{
		}
		Container(const Container &other) : m_contents()
		{
			// Do a deep-copy.
			const Contents &old = other.contents();
			for (size_t i = 0; i < old.size(); i++) {
				m_contents.push_back(new Variable(*(old[i])));
			}
		}
		~Container()
		{
			for (size_t i = 0; i < m_contents.size(); i++) {
				delete m_contents[i];
			}
		}

		void
		append(Variable *variable)
		{
			m_contents.push_back(variable);
		}

		const Contents &
		contents() const
		{
			return m_contents;
		}

	 private:
		Contents m_contents;
	};
	class List : public Container
	{
	 public:
		List() : Container() {}
		List(const List &other) : Container(other) {}
	};
	class Tuple : public Container
	{
	 public:
		Tuple() : Container() {}
		Tuple(const Tuple &other) : Container(other) {}
	};

	// This represents the actual contents of a Variable.  This has no
	// concept of type qualifiers like 'const'.
	class Datum {
	 public:
		// Initialize to the default value for a given type.  This
		// takes ownership of the 'type' argument.
		explicit
		Datum(const Type &type);
		// Make a deep-copy of another Datum.
		Datum(const Datum &other);
		// Initialize a specific type with a given value.
		Datum(const Type &type, bool value);
		Datum(const Type &type, const Value &value);
		Datum(const Type &type, const Func *value);
		Datum(const Type &type, const List *value);
		Datum(const Type &type, const string &value);
		Datum(const Type &type, const Tuple *value);

		const Type &
		type() const
		{
			return m_type;
		}

		bool
		is_assignable_from(const Datum *other) const
		{
			return type().is_assignable_from(other->type());
		}

		// Access this value as a bool.  Throws Variable::TypeError if
		// this Datum is a different type.
		bool
		bool_value() const
		{
			check_type_primitive(Type::BOOL);
			return m_bool_value;
		}

		//FIXME: need fldfmt support

		// Access this value as a function.  Throws
		// Variable::TypeError if this Datum is a different type.
		const Func *
		func_value() const
		{
			check_type_primitive(Type::FUNC);
			return m_func_value.get();
		}

		// Access this value as an int.  Throws Variable::TypeError if
		// this Datum is a different type.
		const Value &
		int_value() const
		{
			check_type_primitive(Type::INT);
			return m_int_value;
		}

		// Access this value as a list.  Throws Variable::TypeError if
		// this Datum is a different type.
		const List &
		list_value() const
		{
			check_type_primitive(Type::LIST);
			return *m_list_value;
		}

		// Access this value as a string.  Throws Variable::TypeError
		// if this Datum is a different type.
		const string &
		string_value() const
		{
			check_type_primitive(Type::STRING);
			return m_string_value;
		}

		// Access this value as a tuple.  Throws Variable::TypeError
		// if this Datum is a different type.
		const Tuple &
		tuple_value() const
		{
			check_type_primitive(Type::TUPLE);
			return *m_tuple_value;
		}

		// Test if this value is undefined.
		bool
		is_undef() const
		{
			// Once the value gets written, the type will no
			// longer be var.
			return (m_type.primitive() == Type::VAR);
		}

	 private:
		void operator=(const Datum &)
		{
			// Not implemented.
		}

		class UndefFunc : public Func {
		 public:
			virtual Variable *
			call() {
				return new Variable(Type::VAR);
			}
		};

		// Throw Variable::TypeError if this Datum is not of the
		// specified primtive type.
		void
		check_type_primitive(Type::Primitive primitive) const;

		Type m_type;
		bool m_type_locked;

		// These are essentially a union.  Only one is ever valid at a
		// time.
		bool m_bool_value;
		boost::shared_ptr<const Func> m_func_value;
		Value m_int_value;
		boost::scoped_ptr<const List> m_list_value;
		boost::scoped_ptr<const Tuple> m_tuple_value;
		string m_string_value;
	};

	// Initialize to the default value for a given type.
	explicit
	Variable(const Type &type)
	    : m_value(new Datum(type)), m_is_const(type.is_const())
	{
	}
	// Initialize from another Variable.  NOTE: These make a reference,
	// not a copy.  If you need a copy, use assign_from().
	Variable(Variable &other)
	    : m_value(other.m_value), m_is_const(other.is_const())
	{
	}
	Variable(Variable &other, Type::Constness constness)
	    : m_value(other.m_value), m_is_const(constness)
	{
	}
	// Init as bool.
	Variable(const Type &type, bool value)
	    : m_value(new Datum(type, value)), m_is_const(type.is_const())
	{
	}
	Variable(const Type &type, Type::Constness constness, bool value)
	    : m_value(new Datum(type, value)), m_is_const(constness)
	{
	}
	// Init as func.
	Variable(const Type &type, const Func *value)
	    : m_value(new Datum(type, value)), m_is_const(type.is_const())
	{
	}
	Variable(const Type &type, Type::Constness constness, const Func *value)
	    : m_value(new Datum(type, value)), m_is_const(constness)
	{
	}
	// Init as int.
	Variable(const Type &type, const Value &value)
	    : m_value(new Datum(type, value)), m_is_const(type.is_const())
	{
	}
	Variable(const Type &type, Type::Constness constness,
	         const Value &value)
	    : m_value(new Datum(type, value)), m_is_const(constness)
	{
	}
	// Init as list.
	Variable(const Type &type, const List *value)
	    : m_value(new Datum(type, value)), m_is_const(type.is_const())
	{
	}
	Variable(const Type &type, Type::Constness constness,
	         const List *value)
	    : m_value(new Datum(type, value)), m_is_const(constness)
	{
	}
	// Init as string.
	Variable(const Type &type, const string &value)
	    : m_value(new Datum(type, value)), m_is_const(type.is_const())
	{
	}
	Variable(const Type &type, Type::Constness constness,
	         const string &value)
	    : m_value(new Datum(type, value)), m_is_const(constness)
	{
	}
	// Init as tuple.
	Variable(const Type &type, const Tuple *value)
	    : m_value(new Datum(type, value)), m_is_const(type.is_const())
	{
	}
	Variable(const Type &type, Type::Constness constness,
	         const Tuple *value)
	    : m_value(new Datum(type, value)), m_is_const(constness)
	{
	}

	const Datum *
	value() const
	{
		return m_value.get();
	}

	bool
	is_const() const
	{
		return m_is_const;
	}

	const Type &
	type() const
	{
		return m_value->type();
	}

	bool
	is_assignable_from(const Variable *other) const
	{
		if (is_const()) {
			return false;
		}
		return m_value->is_assignable_from(other->value());
	}

	void
	assign_from(const Variable &other)
	{
		if (is_const()) {
			throw TypeError(
			    sprintfxx("can't write to const variable"));
		}
		m_value.reset(new Datum(*other.value()));
	}

	// Access as bool.
	bool
	bool_value() const
	{
		return m_value->bool_value();
	}

	// Access as func.
	const Func *
	func_value() const
	{
		return m_value->func_value();
	}

	// Access as int.
	const Value &
	int_value() const
	{
		return m_value->int_value();
	}

	// Access as list.
	const List &
	list_value() const
	{
		return m_value->list_value();
	}

	// Access as string.
	const string &
	string_value() const
	{
		return m_value->string_value();
	}

	// Access as tuple.
	const Tuple &
	tuple_value() const
	{
		return m_value->tuple_value();
	}

 private:
	// This is shared_ptr rather than scoped_ptr for reference semantics.
	boost::shared_ptr<Datum> m_value;
	bool m_is_const;
};

}  // namespace language
}  // namespace hwpp

#endif  // HWPP_LANGUAGE_VARIABLE_H__

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
