#ifndef PP_UTILS_H__
#define PP_UTILS_H__

/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 */

#include "pp.h"
#include "pp_path.h"
#include "pp_field.h"
#include "pp_register.h"
#include "pp_dirent.h"
#include "pp_scope.h"
#include "pp_context.h"
#include "pp_binding.h"
#include "drivers.h"

/*
 * Get/set the current active context.
 */
extern pp_context
GET_CURRENT_CONTEXT();

class pp_saved_context_impl;
typedef boost::shared_ptr<pp_saved_context_impl> pp_saved_context;

extern pp_saved_context
SET_CURRENT_CONTEXT(const pp_context &new_context);


#include "pp_fields.h"

//FIXME: comments
extern const pp_dirent *
GET_DIRENT(const pp_path &path);
extern const pp_field *
GET_FIELD(const pp_path &path);
extern const pp_register *
GET_REGISTER(const pp_path &path);
extern bool
DEFINED(const pp_path &path);

extern pp_value
READ(const pp_path &path);
extern void
WRITE(const pp_path &path, const pp_value &value);
extern pp_value
READ(const pp_regbits &bits);
extern void
WRITE(const pp_regbits &bits, const pp_value &value);

inline int
FIELD_COMPARE(const string &field, const pp_value &comparator)
{
	return GET_FIELD(field)->compare(comparator);
}
inline int
FIELD_COMPARE(const string &field, const string &comparator)
{
	return GET_FIELD(field)->compare(comparator);
}
inline bool
FIELD_EQ(const string &field, const pp_value &comparator)
{
	return (FIELD_COMPARE(field, comparator) == 0);
}
inline bool
FIELD_EQ(const string &field, const string &comparator)
{
	return (FIELD_COMPARE(field, comparator) == 0);
}
inline bool
FIELD_NE(const string &field, const pp_value &comparator)
{
	return (FIELD_COMPARE(field, comparator) != 0);
}
inline bool
FIELD_NE(const string &field, const string &comparator)
{
	return (FIELD_COMPARE(field, comparator) != 0);
}
inline bool
FIELD_LT(const string &field, const pp_value &comparator)
{
	return (FIELD_COMPARE(field, comparator) < 0);
}
inline bool
FIELD_LT(const string &field, const string &comparator)
{
	return (FIELD_COMPARE(field, comparator) < 0);
}
inline bool
FIELD_LE(const string &field, const pp_value &comparator)
{
	return (FIELD_COMPARE(field, comparator) <= 0);
}
inline bool
FIELD_LE(const string &field, const string &comparator)
{
	return (FIELD_COMPARE(field, comparator) <= 0);
}
inline bool
FIELD_GT(const string &field, const pp_value &comparator)
{
	return (FIELD_COMPARE(field, comparator) > 0);
}
inline bool
FIELD_GT(const string &field, const string &comparator)
{
	return (FIELD_COMPARE(field, comparator) > 0);
}
inline bool
FIELD_GE(const string &field, const pp_value &comparator)
{
	return (FIELD_COMPARE(field, comparator) >= 0);
}
inline bool
FIELD_GE(const string &field, const string &comparator)
{
	return (FIELD_COMPARE(field, comparator) >= 0);
}
inline bool
FIELD_BOOL(const string &field)
{
	// compare to 0 => false, else true
	return (FIELD_COMPARE(field, pp_value(0)) != 0);
}
inline pp_value
FIELD_AND(const string &field, const pp_value &comparator)
{
	return GET_FIELD(field)->test(comparator);
}

/*
 * NEW_PLATFORM
 * Create a new platform scope and initialize it with the built-in
 * primitive types as well as global language-defined types.
 */
extern pp_scope *
NEW_PLATFORM();

/*
 * OPEN_SCOPE
 * Create a new scope with the input name.  This pair of open and close
 * scope shortcut functions works like a matching bracket stack.  After
 * you open a scope all subsequent operations (from these shortcut
 * operations, eg ONE_BIT_FIELD) will be placed on this scope.
 */
extern void
OPEN_SCOPE(const string &name, pp_const_binding_ptr binding = pp_binding_ptr());
inline void
OPEN_SCOPE(pp_const_binding_ptr binding)
{
	OPEN_SCOPE(binding->to_string(), binding);
}

/*
 * CLOSE_SCOPE
 * Closes the current scope context.
 */
extern void
CLOSE_SCOPE();

/*
 * REGN
 * A set of shortcut functions for the creation of registers.  The created
 * register will be added onto the present scope (opened by the
 * OPEN_SCOPE function).
 */
extern void
REGN(const string &name, const pp_value &address, pp_bitwidth width);
#define REG8(name, address)   REGN(name, address, BITS8)
#define REG16(name, address)  REGN(name, address, BITS16)
#define REG32(name, address)  REGN(name, address, BITS32)
#define REG64(name, address)  REGN(name, address, BITS64)
#define REG128(name, address) REGN(name, address, BITS128)

#include "pp_regbits.h"

extern pp_regbits
BITS(const string &regname);
extern pp_regbits
BITS(const string &regname, pp_bitwidth bit);
extern pp_regbits
BITS(const string &regname, pp_bitwidth hi_bit, pp_bitwidth lo_bit);

#include "pp_datatype.h"
#include "pp_datatypes.h"

/*
 * Create a field and add it to the current scope.
 *
 * It can take an "unlimited" number of regbits, in the form:
 * 	FIELD("name", "type", BITS("abc", 1, 0) + BITS("def", 7, 2))
 */
extern void
FIELD(const string &name, const pp_datatype *type, const pp_regbits &bits);
extern void
FIELD(const string &name, const string &type, const pp_regbits &bits);

/*
 * Create a field with a constant value.
 */
extern void
FIELD(const string &name, const pp_datatype *type, const pp_value &value);
extern void
FIELD(const string &name, const string &type, const pp_value &value);


/*
 * Create a field which triggers procedures on read/write.
 */
extern void
FIELD(const string &name, const pp_datatype *type,
    const proc_field_accessor_ptr &access);
extern void
FIELD(const string &name, const string &type,
    const proc_field_accessor_ptr &access);
#define PROCS(ptr) proc_field_accessor_ptr(ptr)

/*
 * Create a register and a field that consumes it.
 */
extern void
REGFIELDN(const string &name, const pp_value &address,
		const pp_datatype *type, pp_bitwidth width);
extern void
REGFIELDN(const string &name, const pp_value &address,
		const string &type, pp_bitwidth width);
#define REGFIELD8(name, address, type)   REGFIELDN(name, address, type, BITS8)
#define REGFIELD16(name, address, type)  REGFIELDN(name, address, type, BITS16)
#define REGFIELD32(name, address, type)  REGFIELDN(name, address, type, BITS32)
#define REGFIELD64(name, address, type)  REGFIELDN(name, address, type, BITS64)
#define REGFIELD128(name, address, type) REGFIELDN(name, address, type, BITS128)

// These are helpers for BIND()
struct driver_arg {
	driver_arg(const pp_value &val): m_val(val) {}
	pp_value m_val;
};
typedef std::vector<pp_value> driver_arg_list;
inline driver_arg_list
operator,(const driver_arg &lhs, const pp_value &rhs)
{
	driver_arg_list tmp;
	tmp.push_back(lhs.m_val);
	tmp.push_back(rhs);
	return tmp;
}
inline driver_arg_list
operator,(const driver_arg_list &lhs, const pp_value &rhs)
{
	driver_arg_list tmp(lhs);
	tmp.push_back(rhs);
	return tmp;
}
#define ARGS(...) ((driver_arg)__VA_ARGS__)
/*
 * Create a new binding.
 *
 * This can take an arbitrary list of arguments, such as:
 * 	BIND("mem", ARGS(1, 2, 3, 4));
 */
inline pp_const_binding_ptr
BIND(const string &driver, const driver_arg_list &args)
{
	return find_driver(driver)->new_binding(args);
}
inline pp_const_binding_ptr
BIND(const std::string &driver, const driver_arg &arg)
{
	driver_arg_list al;
	al.push_back(arg.m_val);
	return BIND(driver, al);
}

/*
 * INT
 * A shortcut for creating a pp_int.
 */
extern pp_int *
INT(const string &name, const string &units="");
#define ANON_INT(units) INT("", units)

#include <vector>

// These are helpers for type safety in pp_enum and pp_bitmask.
class kvpair
{
    public:
	kvpair(const string &k, const pp_value &v): m_key(k), m_value(v)
	{}
	const string &
	key() const
	{
		return m_key;
	}
	const pp_value &
	value() const
	{
		return m_value;
	}

    private:
	string m_key;
	pp_value m_value;
};
#define KV(k,v) kvpair(k, v)
typedef keyed_vector<string, pp_value> kvpair_list;
inline kvpair_list
operator,(const kvpair &lhs, const kvpair &rhs)
{
	kvpair_list tmp;
	tmp.insert(lhs.key(), lhs.value());
	tmp.insert(rhs.key(), rhs.value());
	return tmp;
}
inline kvpair_list
operator,(const kvpair_list &lhs, const kvpair &rhs)
{
	kvpair_list tmp(lhs);
	tmp.insert(rhs.key(), rhs.value());
	return tmp;
}

/*
 * Create a pp_bitmask.
 *
 * It can take an "unlimited" number of arguments, in the form:
 * 	BITMASK("name", (KV("abc", 1), KV("def", 2)))
 *
 * NOTE: In calling this, the extra parens around the KV list are required.
 * Why?  Well, C++ won't call operator,() on function arguments, so we force
 * the arguments to be a single argument, which is an expression.
 * Fortunately, a touch of macro magic makes it look like an unlimited
 * argument list.  I wish I was less proud of this.
 */
extern pp_bitmask *
BITMASK_(const string &name, const string &dflt, const kvpair_list &kvlist);
#define BITMASK_DFLT(name, dflt, ...)	BITMASK_(name, dflt, (__VA_ARGS__))
#define BITMASK(name, ...)		BITMASK_(name, "", (__VA_ARGS__))
#define ANON_BITMASK(...)		BITMASK_("", "", (__VA_ARGS__))

/*
 * Create a pp_enum.
 *
 * It can take an "unlimited" number of arguments, in the form:
 * 	ENUM("name", (KV("abc", 1), KV("def", 2)))
 *
 * See the NOTE for BITMASK, above.
 */
extern pp_enum *
ENUM_(const string &name, const kvpair_list &kvlist);
#define ENUM(name, ...)			ENUM_(name, (__VA_ARGS__))
#define ANON_ENUM(...)			ENUM_("", (__VA_ARGS__))

/*
 * BOOL
 * Shortcut to create a boolean.
 */
extern pp_bool *
BOOL(const string &name, const string &true_str, const string &false_str);
#define ANON_BOOL(true_str, false_str) BOOL("", true_str, false_str)

#endif // PP_UTILS_H__
