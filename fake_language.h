//
// Copyright 2007 Google, Inc. All Rights Reserved.
// Copyright 2008 Tim Hockin.
//
#ifndef PP_UTILS_H__
#define PP_UTILS_H__

#include "pp.h"
#include "pp_path.h"
#include "pp_field.h"
#include "pp_register.h"
#include "pp_dirent.h"
#include "pp_scope.h"
#include "pp_binding.h"
#include "drivers.h"

#include <vector>

#include "pp_fields.h"

//
// Lookup dirents by name.
//
extern const pp_dirent *
GET_DIRENT(const string &path);
extern const pp_field *
GET_FIELD(const string &path);
extern const pp_register *
GET_REGISTER(const string &path);
extern bool
DEFINED(const string &path);

//
// Read/write a field, register, or regbits.
//
extern pp_value
READ(const string &path);
extern void
WRITE(const string &path, const pp_value &value);
extern pp_value
READ(const pp_regbits &bits);
extern void
WRITE(const pp_regbits &bits, const pp_value &value);

//
// Perform comparison operations on fields.
//
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

//
// Create a new scope.  Once you open a scope, all subsequent operations
// (such as REGN() or FIELD()) will be relative to this new scope. Unnamed
// scopes can be named via CLOSE_SCOPE().
//
extern void
OPEN_SCOPE(const string &name,
		const pp_binding_ptr &binding = pp_binding_ptr());
inline void
OPEN_SCOPE(const pp_binding_ptr &binding = pp_binding_ptr())
{
	OPEN_SCOPE("", binding);
}

//
// Close the current scope.
//
extern void
CLOSE_SCOPE();
extern void
CLOSE_SCOPE(const string &new_name);

// These are helpers for BIND()
struct pp_helper_driver_arg {
	pp_helper_driver_arg(const pp_value &val): m_val(val) {}
	pp_value m_val;
};
typedef std::vector<pp_value> pp_helper_driver_arg_list;
inline pp_helper_driver_arg_list
operator,(const pp_helper_driver_arg &lhs, const pp_value &rhs)
{
	pp_helper_driver_arg_list tmp;
	tmp.push_back(lhs.m_val);
	tmp.push_back(rhs);
	return tmp;
}
inline pp_helper_driver_arg_list
operator,(const pp_helper_driver_arg_list &lhs, const pp_value &rhs)
{
	pp_helper_driver_arg_list tmp(lhs);
	tmp.push_back(rhs);
	return tmp;
}
#define ARGS(...) ((pp_helper_driver_arg)__VA_ARGS__)
//
// Create a new binding.
//
// This can take an "unlimited" list of arguments, such as:
// 	BIND("mem", ARGS(1, 2, 3, 4));
//
inline pp_binding_ptr
BIND(const string &driver, const pp_helper_driver_arg_list &args)
{
	return pp_find_driver(driver)->new_binding(args);
}
inline pp_binding_ptr
BIND(const string &driver, const pp_helper_driver_arg &arg)
{
	pp_helper_driver_arg_list al;
	al.push_back(arg.m_val);
	return BIND(driver, al);
}

//
// Declare a named register.
//
extern void
REGN(const string &name, const pp_value &address, pp_bitwidth width);
#define REG8(name, address)   REGN(name, address, BITS8)
#define REG16(name, address)  REGN(name, address, BITS16)
#define REG32(name, address)  REGN(name, address, BITS32)
#define REG64(name, address)  REGN(name, address, BITS64)
#define REG128(name, address) REGN(name, address, BITS128)

#include "pp_regbits.h"

//
// Create a register-bits structure.
//
extern pp_regbits
BITS(const string &regname);
extern pp_regbits
BITS(const string &regname, pp_bitwidth bit);
extern pp_regbits
BITS(const string &regname, pp_bitwidth hi_bit, pp_bitwidth lo_bit);

#include "pp_datatype.h"
#include "pp_datatypes.h"

//
// Create a register and a field that consumes it.
//
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

//
// Create a field which gets it's value from regbits.
//
// It can take an "unlimited" number of regbits, in the form:
// 	FIELD("name", "type", BITS("abc", 1, 0) + BITS("def", 7, 2))
//
extern void
FIELD(const string &name, const pp_datatype *type, const pp_regbits &bits);
extern void
FIELD(const string &name, const string &type, const pp_regbits &bits);

//
// Create a field from a constant value.
//
extern void
FIELD(const string &name, const pp_datatype *type, const pp_value &value);
extern void
FIELD(const string &name, const string &type, const pp_value &value);

//
// Create a field which triggers procedures on read/write.
//
extern void
FIELD(const string &name, const pp_datatype *type,
		const proc_field_accessor_ptr &access);
extern void
FIELD(const string &name, const string &type,
		const proc_field_accessor_ptr &access);
#define PROCS(procs) proc_field_accessor_ptr(new procs)

//
// Declare an integer datatype.
//
extern pp_int *
INT(const string &name, const string &units="");
inline pp_int *
ANON_INT(const string &units="")
{
	return INT("", units);
}

//
// Declare an hex datatype.
//
extern pp_hex *
HEX(const string &name, const pp_bitwidth width=BITS0, const string &units="");
inline pp_hex *
ANON_HEX(const pp_bitwidth width=BITS0, const string &units="")
{
	return HEX("", width, units);
}

// These are helpers for type safety in pp_enum and pp_bitmask.
struct pp_helper_kvpair
{
	pp_helper_kvpair(const string &k, const pp_value &v)
	    : m_key(k), m_val(v) {}
	string m_key;
	pp_value m_val;
};
#define KV(k,v) pp_helper_kvpair(k, v)
typedef keyed_vector<string, pp_value> pp_helper_kvpair_list;
inline pp_helper_kvpair_list
operator,(const pp_helper_kvpair &lhs, const pp_helper_kvpair &rhs)
{
	pp_helper_kvpair_list tmp;
	tmp.insert(lhs.m_key, lhs.m_val);
	tmp.insert(rhs.m_key, rhs.m_val);
	return tmp;
}
inline pp_helper_kvpair_list
operator,(const pp_helper_kvpair_list &lhs, const pp_helper_kvpair &rhs)
{
	pp_helper_kvpair_list tmp(lhs);
	tmp.insert(rhs.m_key, rhs.m_val);
	return tmp;
}

//
// Declare a bitmask datatype.
//
// It can take an "unlimited" number of arguments, in the form:
// 	BITMASK("name", (KV("abc", 1), KV("def", 2)))
//
// NOTE: In calling this, the extra parens around the KV list are required.
// Why?  Well, C++ won't call operator,() on function arguments, so we force
// the arguments to be a single argument, which is an expression.
// Fortunately, a touch of macro magic makes it look like an unlimited
// argument list.  I wish I was less proud of this.
//
extern pp_bitmask *
BITMASK_(const string &name, const string &dflt,
		const pp_helper_kvpair_list &kvlist);
#define BITMASK_DFLT(name, dflt, ...)	BITMASK_(name, dflt, (__VA_ARGS__))
#define BITMASK(name, ...)		BITMASK_(name, "", (__VA_ARGS__))
#define ANON_BITMASK(...)		BITMASK_("", "", (__VA_ARGS__))

//
// Declare an enum datatype.
//
// It can take an "unlimited" number of arguments, in the form:
// 	ENUM("name", (KV("abc", 1), KV("def", 2)))
//
// See the NOTE for BITMASK, above.
//
extern pp_enum *
ENUM_(const string &name, const pp_helper_kvpair_list &kvlist);
#define ENUM(name, ...)			ENUM_(name, (__VA_ARGS__))
#define ANON_ENUM(...)			ENUM_("", (__VA_ARGS__))

//
// Declare a boolean datatype.
//
extern pp_bool *
BOOL(const string &name, const string &true_str, const string &false_str);
#define ANON_BOOL(true_str, false_str) BOOL("", true_str, false_str)

#endif // PP_UTILS_H__
