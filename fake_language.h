//
// Copyright 2007 Google, Inc. All Rights Reserved.
// Copyright 2008 Tim Hockin.
//
#ifndef PP_FAKE_LANGUAGE_H__
#define PP_FAKE_LANGUAGE_H__

#include "pp.h"
#include "language.h"
#include "pp_dirent.h"
#include "pp_registers.h"
#include "pp_field.h"
#include "pp_regbits.h"
#include "pp_binding.h"
#include "pp_lambda.h"
#include "drivers.h"

#include <vector>
#include <iostream>
#include <unistd.h>

#include "pp_fields.h"

//
// Print a message
//
// this is a helper for WARN()
struct fkl_formatted_arglist {
	explicit fkl_formatted_arglist(const string &fmt): result(fmt) {}
	// This class will almost always be used as a temporary (const),
	// but we really want to allow operator%() on the result.  This
	// saves us having to take a non-const copy of this class in
	// operator,() below.
	mutable boost::format result;
};
template <typename Trhs>
inline const fkl_formatted_arglist &
operator,(const fkl_formatted_arglist &lhs, const Trhs &rhs)
{
	lhs.result % rhs;
	return lhs;
}
inline void
WARNF(const fkl_formatted_arglist &arglist)
{
	PP_WARN(to_string(arglist.result));
}
// WARN() operates like printf(), but can take C++ objects (such as string
// and pp_value) and has a newline added automatically.
#define WARN(fmt, ...) WARNF((fkl_formatted_arglist(fmt), ##__VA_ARGS__))

//
// Pause
//
inline void
MSLEEP(int msecs)
{
	usleep(msecs * 1000);
}

//
// Lookup dirents by name.  These are rarely needed.
//
extern pp_dirent_const_ptr
fkl_get_dirent(const parse_location &loc, const string &path);
#define GET_DIRENT(...)		fkl_get_dirent(THIS_LOCATION, ##__VA_ARGS__)
extern pp_field_const_ptr
fkl_get_field(const parse_location &loc, const string &path);
#define GET_FIELD(...)		fkl_get_field(THIS_LOCATION, ##__VA_ARGS__)
extern pp_register_const_ptr
fkl_get_register(const parse_location &loc, const string &path);
#define GET_REGISTER(...)	fkl_get_register(THIS_LOCATION, ##__VA_ARGS__)

//
// Test for the existence of a dirent.
//
extern bool
fkl_defined(const parse_location &loc, const string &path);
#define DEFINED(...)		fkl_defined(THIS_LOCATION, ##__VA_ARGS__)

//
// Read/write a field, register, or regbits.
//
extern pp_value
fkl_read(const parse_location &loc, const string &path);
extern pp_value
fkl_read(const parse_location &loc, const pp_regbits &bits);
#define READ(...)		fkl_read(THIS_LOCATION, ##__VA_ARGS__)
extern void
fkl_write(const parse_location &loc,
          const string &path, const pp_value &value);
extern void
fkl_write(const parse_location &loc,
          const pp_regbits &bits, const pp_value &value);
#define WRITE(...)		fkl_write(THIS_LOCATION, ##__VA_ARGS__)

//
// Perform comparison operations on fields.
// These are macros, rather than inlines, in order to get easy
// parse_location tracking.
//
#define FIELD_COMPARE(field, comparator) GET_FIELD(field)->compare(comparator)
#define FIELD_EQ(field, comparator)	(FIELD_COMPARE(field, comparator) == 0)
#define FIELD_NE(field, comparator)	(FIELD_COMPARE(field, comparator) != 0)
#define FIELD_LT(field, comparator)	(FIELD_COMPARE(field, comparator) < 0)
#define FIELD_LE(field, comparator)	(FIELD_COMPARE(field, comparator) <= 0)
#define FIELD_GT(field, comparator)	(FIELD_COMPARE(field, comparator) > 0)
#define FIELD_GE(field, comparator)	(FIELD_COMPARE(field, comparator) >= 0)
#define FIELD_BOOL(field)		(FIELD_COMPARE(field, pp_value(0)) != 0)
#define FIELD_AND(field, comparator)	GET_FIELD(field)->test(comparator)
#define FIELD_TEST(field, comparator)	(FIELD_AND(field, comparator) != 0)

//
// Create a new scope.  Once you open a scope, all subsequent operations
// (such as REG8() or FIELD()) will be relative to this new scope. Scopes
// can be renamed via CLOSE_SCOPE().
//
extern void
fkl_open_scope(const parse_location &loc,
		const string &name, const pp_binding_ptr &binding);
inline void
fkl_open_scope(const parse_location &loc, const string &name)
{
	fkl_open_scope(loc, name, pp_binding_ptr());
}
#define OPEN_SCOPE(...)		fkl_open_scope(THIS_LOCATION, ##__VA_ARGS__)

//
// Close the current scope.
//
extern void
fkl_close_scope(const parse_location &loc);
extern void
fkl_close_scope(const parse_location &loc, const string &new_name);
#define CLOSE_SCOPE(...)	fkl_close_scope(THIS_LOCATION, ##__VA_ARGS__)

//
// Bookmark the current scope.
//
extern void
fkl_bookmark(const parse_location &loc, const string &name);
#define BOOKMARK(...)		fkl_bookmark(THIS_LOCATION, ##__VA_ARGS__)

// These are helpers for ARGS()
struct fkl_valarg {
	fkl_valarg(const pp_value &val): value(val) {}
	pp_value value;
};
typedef std::vector<pp_value> fkl_valarg_list;
inline fkl_valarg_list
operator,(const fkl_valarg &lhs, const pp_value &rhs)
{
	fkl_valarg_list tmp;
	tmp.push_back(lhs.value);
	tmp.push_back(rhs);
	return tmp;
}
inline fkl_valarg_list
operator,(fkl_valarg_list lhs, const pp_value &rhs)
{
	lhs.push_back(rhs);
	return lhs;
}
#define ARGS(...) ((fkl_valarg)__VA_ARGS__)

//
// Create a new binding.
//
// This can take an "unlimited" list of arguments, such as:
// 	BIND("mem", ARGS(1, 2, 3, 4));
//
inline pp_binding_ptr
BIND(const string &driver, const fkl_valarg_list &args)
{
	return pp_find_driver(driver)->new_binding(args);
}
inline pp_binding_ptr
BIND(const string &driver, const fkl_valarg &arg)
{
	fkl_valarg_list al;
	al.push_back(arg.value);
	return BIND(driver, al);
}

//
// Declare a named register.
//
extern void
fkl_reg(const parse_location &loc,
        const string &name, const pp_value &address, pp_bitwidth width);
extern void
fkl_reg(const parse_location &loc,
        const string &name, const pp_binding_const_ptr &binding,
        const pp_value &address, pp_bitwidth width);
#define REG8(...)   fkl_reg(THIS_LOCATION, ##__VA_ARGS__, BITS8)
#define REG16(...)  fkl_reg(THIS_LOCATION, ##__VA_ARGS__, BITS16)
#define REG32(...)  fkl_reg(THIS_LOCATION, ##__VA_ARGS__, BITS32)
#define REG64(...)  fkl_reg(THIS_LOCATION, ##__VA_ARGS__, BITS64)
#define REG128(...) fkl_reg(THIS_LOCATION, ##__VA_ARGS__, BITS128)

//
// Declare a named proc-register
//
extern void
fkl_reg(const parse_location &loc,
        const string &name, const pp_rwprocs_ptr &access,
        pp_bitwidth width);
#define PROCS(procs)		pp_rwprocs_ptr(new procs)

//
// Create a register-bits structure.
//
extern pp_regbits
fkl_bits(const parse_location &loc, const string &regname);
extern pp_regbits
fkl_bits(const parse_location &loc, const string &regname, pp_bitwidth bit);
extern pp_regbits
fkl_bits(const parse_location &loc,
         const string &regname, pp_bitwidth hi_bit, pp_bitwidth lo_bit);
#define BITS(...)		fkl_bits(THIS_LOCATION, ##__VA_ARGS__)

#include "pp_datatype.h"

//
// Create a register and a field that consumes it.
//
extern void
fkl_regfield(const parse_location &loc,
             const string &name, const pp_value &address,
             const pp_datatype_const_ptr &type, pp_bitwidth width);
extern void
fkl_regfield(const parse_location &loc,
             const string &name, const pp_value &address,
             const string &type, pp_bitwidth width);
extern void
fkl_regfield(const parse_location &loc,
             const string &name, const pp_binding_const_ptr &binding,
             const pp_value &address, const pp_datatype_const_ptr &type,
             pp_bitwidth width);
extern void
fkl_regfield(const parse_location &loc,
             const string &name, const pp_binding_const_ptr &binding,
             const pp_value &address, const string &type, pp_bitwidth width);
#define REGFIELD8(...) \
		fkl_regfield(THIS_LOCATION, ##__VA_ARGS__, BITS8)
#define REGFIELD16(...) \
		fkl_regfield(THIS_LOCATION, ##__VA_ARGS__, BITS16)
#define REGFIELD32(...) \
		fkl_regfield(THIS_LOCATION, ##__VA_ARGS__, BITS32)
#define REGFIELD64(...) \
		fkl_regfield(THIS_LOCATION, ##__VA_ARGS__, BITS64)
#define REGFIELD128(...) \
		fkl_regfield(THIS_LOCATION, ##__VA_ARGS__, BITS128)

//
// Declare a proc regfield
//
extern void
fkl_regfield(const parse_location &loc,
        const string &name, const pp_rwprocs_ptr &access,
        const pp_datatype_const_ptr &type, pp_bitwidth width);
extern void
fkl_regfield(const parse_location &loc,
        const string &name, const pp_rwprocs_ptr &access,
        const string &type, pp_bitwidth width);

//
// Create a field which gets it's value from regbits.
//
// It can take an "unlimited" number of regbits, in the form:
// 	FIELD("name", "type", BITS("abc", 1, 0) + BITS("def", 7, 2))
//
extern void
fkl_field(const parse_location &loc,
          const string &name, const pp_datatype_const_ptr &type,
          const pp_regbits &bits);
extern void
fkl_field(const parse_location &loc,
          const string &name, const string &type, const pp_regbits &bits);

//
// Create a field from a constant value.
//
extern void
fkl_field(const parse_location &loc,
          const string &name, const pp_datatype_const_ptr &type,
          const pp_value &value);
extern void
fkl_field(const parse_location &loc,
          const string &name, const string &type, const pp_value &value);

//
// Create a field which triggers procedures on read/write.
//
extern void
fkl_field(const parse_location &loc,
          const string &name, const pp_datatype_const_ptr &type,
          const pp_rwprocs_ptr &access);
extern void
fkl_field(const parse_location &loc,
          const string &name, const string &type,
          const pp_rwprocs_ptr &access);

#define FIELD(...)		fkl_field(THIS_LOCATION, ##__VA_ARGS__)

#include "pp_datatypes.h"

// helper for types, exposed here because of templates
extern void
fkl_validate_type_name(const string &name, const parse_location &loc);

//
// Declare an integer datatype.
//
extern pp_datatype_ptr
fkl_int(const parse_location &loc, const string &name, const string &units);
inline pp_datatype_ptr
fkl_int(const parse_location &loc, const string &name)
{
	return fkl_int(loc, name, "");
}
inline pp_datatype_ptr
fkl_anon_int(const parse_location &loc, const string &units)
{
	return fkl_int(loc, "", units);
}
inline pp_datatype_ptr
fkl_anon_int(const parse_location &loc)
{
	return fkl_int(loc, "", "");
}
#define INT(...)		fkl_int(THIS_LOCATION, ##__VA_ARGS__)
#define ANON_INT(...)		fkl_anon_int(THIS_LOCATION, ##__VA_ARGS__)

//
// Declare a hex datatype.
//
extern pp_datatype_ptr
fkl_hex(const parse_location &loc,
        const string &name, const pp_bitwidth width, const string &units);
inline pp_datatype_ptr
fkl_hex(const parse_location &loc, const string &name, const pp_bitwidth width)
{
	return fkl_hex(loc, name, width, "");
}
inline pp_datatype_ptr
fkl_hex(const parse_location &loc, const string &name, const string &units)
{
	return fkl_hex(loc, name, BITS0, units);
}
inline pp_datatype_ptr
fkl_hex(const parse_location &loc, const string &name)
{
	return fkl_hex(loc, name, BITS0, "");
}
inline pp_datatype_ptr
fkl_anon_hex(const parse_location &loc, const pp_bitwidth width)
{
	return fkl_hex(loc, "", width, "");
}
inline pp_datatype_ptr
fkl_anon_hex(const parse_location &loc, const string &units)
{
	return fkl_hex(loc, "", BITS0, units);
}
inline pp_datatype_ptr
fkl_anon_hex(const parse_location &loc)
{
	return fkl_hex(loc, "", BITS0, "");
}
#define HEX(...)		fkl_hex(THIS_LOCATION, ##__VA_ARGS__)
#define ANON_HEX(...)		fkl_anon_hex(THIS_LOCATION, ##__VA_ARGS__)

// These are helpers for type safety in pp_enum and pp_bitmask.
struct fkl_kvpair
{
	fkl_kvpair(const string &k, const pp_value &v): key(k), value(v) {}
	string key;
	pp_value value;
};
#define KV(k,v) fkl_kvpair(k, v)
typedef keyed_vector<string, pp_value> fkl_kvpair_list;
inline fkl_kvpair_list
operator,(const fkl_kvpair &lhs, const fkl_kvpair &rhs)
{
	fkl_kvpair_list tmp;
	tmp.insert(lhs.key, lhs.value);
	tmp.insert(rhs.key, rhs.value);
	return tmp;
}
inline fkl_kvpair_list
operator,(const fkl_kvpair_list &lhs, const fkl_kvpair &rhs)
{
	fkl_kvpair_list tmp(lhs);
	tmp.insert(rhs.key, rhs.value);
	return tmp;
}

//
// Declare a bitmask datatype.
//
// It can take an "unlimited" number of arguments, in the form:
// 	BITMASK("name", KV("abc", 1), KV("def", 2))
//
// NOTE: In calling this, the extra parens around the KV list are required.
// Why?  Well, C++ won't call operator,() on function arguments, so we force
// the arguments to be a single argument, which is an expression.
// Fortunately, a touch of macro magic makes it look like an unlimited
// argument list.  I wish I was less proud of this.
//
extern pp_datatype_ptr
fkl_bitmask(const parse_location &loc,
            const string &name, const fkl_kvpair_list &kvlist);
inline pp_datatype_ptr
fkl_bitmask(const parse_location &loc,
            const string &name, const fkl_kvpair &kvpair)
{
	fkl_kvpair_list tmp;
	tmp.insert(kvpair.key, kvpair.value);
	return fkl_bitmask(loc, name, tmp);
}
#define BITMASK(name, ...)	fkl_bitmask(THIS_LOCATION, name, (__VA_ARGS__))
#define ANON_BITMASK(...)	fkl_bitmask(THIS_LOCATION, "", (__VA_ARGS__))

//
// Declare an enum datatype.
//
// It can take an "unlimited" number of arguments, in the form:
// 	ENUM("name", KV("abc", 1), KV("def", 2))
//
// See the NOTE for BITMASK, above.
//
extern pp_datatype_ptr
fkl_enum(const parse_location &loc,
         const string &name, const fkl_kvpair_list &kvlist);

inline pp_datatype_ptr
fkl_enum(const parse_location &loc,
         const string &name, const fkl_kvpair &kvpair)
{
	fkl_kvpair_list tmp;
	tmp.insert(kvpair.key, kvpair.value);
	return fkl_enum(loc, name, tmp);
}
#define ENUM(name, ...)		fkl_enum(THIS_LOCATION, name, (__VA_ARGS__))
#define ANON_ENUM(...)		fkl_enum(THIS_LOCATION, "", (__VA_ARGS__))

//
// Declare a boolean datatype.
//
extern pp_datatype_ptr
fkl_bool(const parse_location &loc,
         const string &name, const string &true_str, const string &false_str);
#define BOOL(name, true_, false_) \
				fkl_bool(THIS_LOCATION, name, true_, false_)
#define ANON_BOOL(true_, false_) \
				fkl_bool(THIS_LOCATION, "", true_, false_)

//
// Declare a transform datatype wrapper.
// This has to be inline because it is a template.  It has to be a
// template because of boost::lambda.
//
template<typename Tdefunc, typename Tenfunc>
inline pp_datatype_ptr
fkl_xform(const parse_location &loc,
          const string &name, const pp_datatype_const_ptr &real_type,
          const Tdefunc &decode_func, const Tenfunc &encode_func)
{
	DTRACE(TRACE_TYPES, "xform: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
	    "current_context is read-only");

	DASSERT_MSG(real_type, "found NULL real_type for xform " + name);

	pp_transform_datatype_ptr xform_ptr = new_pp_transform_datatype(
	    real_type, decode_func, encode_func);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		current_context.add_datatype(name, xform_ptr);
	}
	return xform_ptr;
}
template<typename Tdefunc, typename Tenfunc>
inline pp_datatype_ptr
fkl_xform(const parse_location &loc,
          const string &name, const string &real_type,
          const Tdefunc &decode_func, const Tenfunc &encode_func)
{
	return fkl_xform(loc, name,
	    current_context.resolve_datatype(real_type),
	    decode_func, encode_func);
}
#define XFORM(...)		fkl_xform(THIS_LOCATION, ##__VA_ARGS__)
#define ANON_XFORM(...)		fkl_xform(THIS_LOCATION, "", ##__VA_ARGS__)
#define LAMBDA // for clarity in callers

#endif // PP_FAKE_LANGUAGE_H__
