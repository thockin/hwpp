#ifndef PP_LANGUAGE_TYPE_H__
#define PP_LANGUAGE_TYPE_H__

#include "pp.h"
#include <string>

namespace pp {
namespace language {

// This represents a fully qualified datatype, including type arguments if
// applicable.  Types are almost always passed around as instances, rather
// than pointers because they are much easier to use that way.  They have
// value semantics, so it makes sense.
class Type {
 public:
	enum Primitive {
		BOOL,
		FLDFMT,
		FUNC,
		INT,
		LIST,
		STRING,
		TUPLE,
		VAR,
	};

	// Used for clearer constness management when creating const Types and
	// Variables.  You can add Constness as an argument to most of the ctors
	// because it is always safe to add constness to a reference.  You can
	// not add NON_CONST as a qualifier because it is a logical flub.  If
	// the Variable or Type is already non-const it is a no-op.  If the
	// Variable or Type is const it is an error.
	enum Constness {
		CONST = 1,
		LITERAL = 2,
	};

	// A generic type error.  Prefer using subclasses when possible.
	// Throw this when there is a problem with type arguments.
	struct Error : public std::runtime_error {
		explicit
		Error(const string &str) : runtime_error(str)
		{
		}
	};
	struct ArgumentError : public Error {
		explicit
		ArgumentError(const string &str) : Error(str)
		{
		}
	};
	// Throw this when there is an unknown type.
	struct UnknownError: public Error {
		explicit
		UnknownError(const string &str) : Error(str)
		{
		}
	};

	// Implicit conversion is OK.
	Type(Primitive prim)
	    : m_primitive(prim), m_is_const(0), m_arguments()
	{
	}
	Type(Primitive prim, Constness constness)
	    : m_primitive(prim), m_is_const(constness), m_arguments()
	{
	}
	// Make a deep-copy.
	Type(const Type &other);
	// Non-virtual dtor.
	~Type()
	{
		reinit(VAR);
	}
	// Default copy ctor is OK.

	// Re-initialize this Type for re-use.  Returns 'this'.
	Type *
	reinit(Primitive prim);
	Type *
	reinit(Primitive prim, Constness constness);

	// Add a type argument.  Returns 'this'.
	Type *
	add_argument(const Type &arg);

	static string
	primitive_to_string(Primitive prim);

	string
	to_string() const;

	bool
	is_equal_to(const Type &other) const;

	// Type assignability is strict.  There is no conversion between types
	// at all.  The only soft spot is variables with spec-type var, which
	// can be assigned from any type, but otherwise behave as their
	// actual-type.  Assignments to explicitly typed variables from var
	// typed variables is checked at runtime.
	//   $type = $type                  OK
	//   var = $type                    OK
	//   $type = var                    OK (runtime checked)
	//   list<$type> = list<$type>      OK
	//   list<var> = list<$type>        OK
	//   list<$type> = list<var>        OK (runtime checked)
	bool
	is_assignable_from(const Type &other) const;

	// Type initialization is equivalent to type assignment, except for some
	// subtle details.  Making this a distinct function makes the code easier
	// to follow.
	bool
	is_initializable_from(const Type &other) const;

	bool
	is_comparable_to(const Type &other) const
	{
		return (is_assignable_from(other, IGNORE_CONST)
		     || other.is_assignable_from(*this, IGNORE_CONST));
	}

	Primitive
	primitive() const
	{
		return m_primitive;
	}

	bool
	is_const() const
	{
		return m_is_const != 0;
	}

	Type *
	set_const()
	{
		m_is_const = CONST;
		return this;
	}

	bool
	is_literal() const
	{
		return m_is_const == LITERAL;
	}

	size_t
	n_arguments() const
	{
		return m_arguments.size();
	}
	const Type &
	argument(size_t index) const
	{
		return m_arguments[index];
	}

 private:
	// This is used for internal type comparisons without regard to const
	// qualification.
	enum IgnoreConst {
		DONT_IGNORE_CONST = 0,
		IGNORE_CONST = 1
	};

	// See if a type would be assignable if const did not matter.
	bool
	is_assignable_from(const Type &other, IgnoreConst ignore_const) const;

	Primitive m_primitive;
	// In general, constness applies to a Variable, not a Type.  In order
	// to make Type arguments and some literal assigments easier, we keep a
	// notion of constness here, which we can copy into Variables as they are
	// created.
	int m_is_const;
	std::vector<Type> m_arguments;
};

inline bool
operator==(const Type &lhs, const Type &rhs)
{
	return lhs.is_equal_to(rhs);
}

inline bool
operator!=(const Type &lhs, const Type &rhs)
{
	return !(lhs.is_equal_to(rhs));
}

}  // namespace language
}  // namespace pp

#endif  // PP_LANGUAGE_TYPE_H__

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
