//
// Copyright 2007 Google, Inc. All Rights Reserved.
// Copyright 2008 Tim Hockin.
//
#ifndef PP_FAKE_LANGUAGE_H__
#define PP_FAKE_LANGUAGE_H__

#include "pp/pp.h"
#include "language.h"
#include "pp/dirent.h"
#include "pp/register_types.h"
#include "pp/field.h"
#include "pp/alias.h"
#include "pp/regbits.h"
#include "pp/binding.h"
#include "pp/util/bignum_lambda.h"
#include "drivers.h"

#include <vector>
#include <iostream>
#include <unistd.h>

#include "pp/field_types.h"

namespace pp {

//
// Print a message
//
// this is a helper for WARN()
struct FklFormattedArglist {
	explicit FklFormattedArglist(const string &fmt): result(fmt) {}
	// This class will almost always be used as a temporary (const),
	// but we really want to allow operator%() on the result.  This
	// saves us having to take a non-const copy of this class in
	// operator,() below.
	mutable boost::format result;
};
template <typename Trhs>
inline const FklFormattedArglist &
operator,(const FklFormattedArglist &lhs, const Trhs &rhs)
{
	lhs.result % rhs;
	return lhs;
}
inline void
WARNF(const FklFormattedArglist &arglist)
{
	WARN(to_string(arglist.result));
}
// WARN() operates like printf(), but can take C++ objects (such as string
// and Value) and has a newline added automatically.
#define WARN(fmt, ...) WARNF((FklFormattedArglist(fmt), ##__VA_ARGS__))

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
extern ConstDirentPtr
fkl_get_dirent(const ParseLocation &loc, const string &path);
#define GET_DIRENT(...)    ::pp::fkl_get_dirent(THIS_LOCATION, ##__VA_ARGS__)
extern ConstFieldPtr
fkl_get_field(const ParseLocation &loc, const string &path);
#define GET_FIELD(...)     ::pp::fkl_get_field(THIS_LOCATION, ##__VA_ARGS__)
extern ConstRegisterPtr
fkl_get_register(const ParseLocation &loc, const string &path);
#define GET_REGISTER(...)  ::pp::fkl_get_register(THIS_LOCATION, ##__VA_ARGS__)

//
// Test for the existence of a dirent.
//
extern bool
fkl_defined(const ParseLocation &loc, const string &path);
#define DEFINED(...)  ::pp::fkl_defined(THIS_LOCATION, ##__VA_ARGS__)

//
// Read/write a field, register, or regbits.
//
extern Value
fkl_read(const ParseLocation &loc, const string &path);
extern Value
fkl_read(const ParseLocation &loc, const RegBits &bits);
#define READ(...)  ::pp::fkl_read(THIS_LOCATION, ##__VA_ARGS__)
extern void
fkl_write(const ParseLocation &loc,
          const string &path, const Value &value);
extern void
fkl_write(const ParseLocation &loc,
          const RegBits &bits, const Value &value);
#define WRITE(...)  ::pp::fkl_write(THIS_LOCATION, ##__VA_ARGS__)

//
// Perform comparison operations on fields.
// These are macros, rather than inlines, in order to get easy
// ParseLocation tracking.
//
#define FIELD_COMPARE(field, comparator) GET_FIELD(field)->compare(comparator)
#define FIELD_EQ(field, comparator)     (FIELD_COMPARE(field, comparator) == 0)
#define FIELD_NE(field, comparator)     (FIELD_COMPARE(field, comparator) != 0)
#define FIELD_LT(field, comparator)     (FIELD_COMPARE(field, comparator) < 0)
#define FIELD_LE(field, comparator)     (FIELD_COMPARE(field, comparator) <= 0)
#define FIELD_GT(field, comparator)     (FIELD_COMPARE(field, comparator) > 0)
#define FIELD_GE(field, comparator)     (FIELD_COMPARE(field, comparator) >= 0)
#define FIELD_BOOL(field)		(FIELD_COMPARE(field, Value(0)) != 0)
#define FIELD_AND(field, comparator)    (GET_FIELD(field)->test(comparator))
#define FIELD_TEST(field, comparator)   (FIELD_AND(field, comparator) != 0)

//
// Create a new scope.  Once you open a scope, all subsequent operations
// (such as REG8() or FIELD()) will be relative to this new scope. Scopes
// can be renamed via CLOSE_SCOPE().
//
extern void
fkl_open_scope(const ParseLocation &loc,
		const string &name, const BindingPtr &binding);
inline void
fkl_open_scope(const ParseLocation &loc, const string &name)
{
	fkl_open_scope(loc, name, BindingPtr());
}
#define OPEN_SCOPE(...)  ::pp::fkl_open_scope(THIS_LOCATION, ##__VA_ARGS__)

//
// Close the current scope.
//
extern void
fkl_close_scope(const ParseLocation &loc);
#define CLOSE_SCOPE(...)  ::pp::fkl_close_scope(THIS_LOCATION, ##__VA_ARGS__)

//
// Bookmark the current scope.
//
extern void
fkl_bookmark(const ParseLocation &loc, const string &name);
#define BOOKMARK(...)  ::pp::fkl_bookmark(THIS_LOCATION, ##__VA_ARGS__)

// These are helpers for ARGS()
struct FklValArg {
	FklValArg(const Value &val): value(val) {}
	Value value;
};
typedef std::vector<Value> FklValArgList;
inline FklValArgList
operator,(const FklValArg &lhs, const Value &rhs)
{
	FklValArgList tmp;
	tmp.push_back(lhs.value);
	tmp.push_back(rhs);
	return tmp;
}
inline FklValArgList
operator,(FklValArgList lhs, const Value &rhs)
{
	lhs.push_back(rhs);
	return lhs;
}
#define ARGS(...) ((FklValArg)__VA_ARGS__)

//
// Create a new binding.
//
// This can take an "unlimited" list of arguments, such as:
// 	BIND("mem", ARGS(1, 2, 3, 4));
//
inline BindingPtr
BIND(const string &driver, const FklValArgList &args)
{
	return find_driver(driver)->new_binding(args);
}
inline BindingPtr
BIND(const string &driver, const FklValArg &arg)
{
	FklValArgList al;
	al.push_back(arg.value);
	return BIND(driver, al);
}

//
// Define a register.
//
extern RegisterPtr
fkl_reg(const ParseLocation &loc,
        const string &name, const Value &address, BitWidth width);
extern RegisterPtr
fkl_reg(const ParseLocation &loc,
        const string &name, const ConstBindingPtr &binding,
        const Value &address, BitWidth width);
extern RegisterPtr
fkl_reg(const ParseLocation &loc,
        const Value &address, BitWidth width);
extern RegisterPtr
fkl_reg(const ParseLocation &loc,
        const ConstBindingPtr &binding, const Value &address, BitWidth width);
#define REG8(...)    ::pp::fkl_reg(THIS_LOCATION, ##__VA_ARGS__, pp::BITS8)
#define REG16(...)   ::pp::fkl_reg(THIS_LOCATION, ##__VA_ARGS__, pp::BITS16)
#define REG32(...)   ::pp::fkl_reg(THIS_LOCATION, ##__VA_ARGS__, pp::BITS32)
#define REG64(...)   ::pp::fkl_reg(THIS_LOCATION, ##__VA_ARGS__, pp::BITS64)
#define REG128(...)  ::pp::fkl_reg(THIS_LOCATION, ##__VA_ARGS__, pp::BITS128)

//
// Declare a proc-register
//
extern RegisterPtr
fkl_reg(const ParseLocation &loc,
        const string &name, const RwProcsPtr &access, BitWidth width);
extern RegisterPtr
fkl_reg(const ParseLocation &loc,
        const RwProcsPtr &access, BitWidth width);
#define PROCS(procs)  pp::RwProcsPtr(new procs)

//
// Create a register-bits structure.
//
extern RegBits
fkl_bits(const ParseLocation &loc, const string &regname);
extern RegBits
fkl_bits(const ParseLocation &loc, const string &regname, BitWidth bit);
extern RegBits
fkl_bits(const ParseLocation &loc,
         const string &regname, BitWidth hi_bit, BitWidth lo_bit);
extern RegBits
fkl_bits(const ParseLocation &loc, const ConstRegisterPtr &regname);
extern RegBits
fkl_bits(const ParseLocation &loc,
         const ConstRegisterPtr &regname, BitWidth bit);
extern RegBits
fkl_bits(const ParseLocation &loc,
         const ConstRegisterPtr &regname,
         BitWidth hi_bit, BitWidth lo_bit);
#define BITS(...)  ::pp::fkl_bits(THIS_LOCATION, ##__VA_ARGS__)

}  // namespace pp

#include "pp/datatype.h"

namespace pp {

//
// Create a field which gets it's value from regbits.
//
// It can take an "unlimited" number of regbits, in the form:
// 	FIELD("name", "type", BITS("abc", 1, 0) + BITS("def", 7, 2))
//
extern FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const ConstDatatypePtr &type,
          const RegBits &bits);
extern FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const string &type, const RegBits &bits);
// these are needed to disambiguate smart pointers
inline FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const ConstDatatypePtr &type,
          const RegisterPtr &reg)
{
	return fkl_field(loc, name, type, fkl_bits(loc, reg));
}
inline FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const string &type,
          const RegisterPtr &reg)
{
	return fkl_field(loc, name, type, fkl_bits(loc, reg));
}
inline FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const ConstDatatypePtr &type,
          const ConstRegisterPtr &reg)
{
	return fkl_field(loc, name, type, fkl_bits(loc, reg));
}
inline FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const string &type,
          const ConstRegisterPtr &reg)
{
	return fkl_field(loc, name, type, fkl_bits(loc, reg));
}

//
// Create a field from a constant value.
//
extern FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const ConstDatatypePtr &type,
          const Value &value);
extern FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const string &type, const Value &value);

//
// Create a field which triggers procedures on read/write.
//
extern FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const ConstDatatypePtr &type,
          const RwProcsPtr &access);
extern FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const string &type,
          const RwProcsPtr &access);

#define FIELD(...)  ::pp::fkl_field(THIS_LOCATION, ##__VA_ARGS__)

//
// Create an alias to another dirent
//
extern AliasPtr
fkl_alias(const ParseLocation &loc, const string &name, const string &tgt_path);
#define ALIAS(...)  ::pp::fkl_alias(THIS_LOCATION, ##__VA_ARGS__)

}  // namespace pp

#include "pp/datatype_types.h"

namespace pp {

// helper for types, exposed here because of templates
extern void
fkl_validate_type_name(const string &name, const ParseLocation &loc);

//
// Declare an integer datatype.
//
extern DatatypePtr
fkl_int(const ParseLocation &loc, const string &name, const string &units);
inline DatatypePtr
fkl_int(const ParseLocation &loc, const string &name)
{
	return fkl_int(loc, name, "");
}
inline DatatypePtr
fkl_anon_int(const ParseLocation &loc, const string &units)
{
	return fkl_int(loc, "", units);
}
inline DatatypePtr
fkl_anon_int(const ParseLocation &loc)
{
	return fkl_int(loc, "", "");
}
#define INT(...)       ::pp::fkl_int(THIS_LOCATION, ##__VA_ARGS__)
#define ANON_INT(...)  ::pp::fkl_anon_int(THIS_LOCATION, ##__VA_ARGS__)

//
// Declare a hex datatype.
//
extern DatatypePtr
fkl_hex(const ParseLocation &loc,
        const string &name, const BitWidth width, const string &units);
inline DatatypePtr
fkl_hex(const ParseLocation &loc, const string &name, const BitWidth width)
{
	return fkl_hex(loc, name, width, "");
}
inline DatatypePtr
fkl_hex(const ParseLocation &loc, const string &name, const string &units)
{
	return fkl_hex(loc, name, pp::BITS0, units);
}
inline DatatypePtr
fkl_hex(const ParseLocation &loc, const string &name)
{
	return fkl_hex(loc, name, pp::BITS0, "");
}
inline DatatypePtr
fkl_anon_hex(const ParseLocation &loc, const BitWidth width)
{
	return fkl_hex(loc, "", width, "");
}
inline DatatypePtr
fkl_anon_hex(const ParseLocation &loc, const string &units)
{
	return fkl_hex(loc, "", pp::BITS0, units);
}
inline DatatypePtr
fkl_anon_hex(const ParseLocation &loc)
{
	return fkl_hex(loc, "", pp::BITS0, "");
}
#define HEX(...)       ::pp::fkl_hex(THIS_LOCATION, ##__VA_ARGS__)
#define ANON_HEX(...)  ::pp::fkl_anon_hex(THIS_LOCATION, ##__VA_ARGS__)

//
// Declare a string datatype.
//
extern DatatypePtr
fkl_string(const ParseLocation &loc, const string &name);
inline DatatypePtr
fkl_anon_string(const ParseLocation &loc)
{
	return fkl_string(loc, "");
}
#define STRING(...)       ::pp::fkl_string(THIS_LOCATION, ##__VA_ARGS__)
#define ANON_STRING(...)  ::pp::fkl_anon_string(THIS_LOCATION, ##__VA_ARGS__)

// These are helpers for type safety in pp_enum and pp_bitmask.
struct fkl_kvpair
{
	fkl_kvpair(const string &k, const Value &v): key(k), value(v) {}
	string key;
	Value value;
};
#define KV(k,v)  ::pp::fkl_kvpair(k, v)
typedef util::KeyedVector<string, Value> FklKvPairList;
inline FklKvPairList
operator,(const fkl_kvpair &lhs, const fkl_kvpair &rhs)
{
	FklKvPairList tmp;
	tmp.insert(lhs.key, lhs.value);
	if (lhs.key == rhs.key) {
		WARN("duplicate KV key: '" + rhs.key);
	}
	tmp.insert(rhs.key, rhs.value);
	return tmp;
}
inline FklKvPairList
operator,(const FklKvPairList &lhs, const fkl_kvpair &rhs)
{
	if (lhs.has_key(rhs.key)) {
		WARN("duplicate KV key: '" + rhs.key);
	}
	FklKvPairList tmp(lhs);
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
extern DatatypePtr
fkl_bitmask(const ParseLocation &loc,
            const string &name, const FklKvPairList &kvlist);
inline DatatypePtr
fkl_bitmask(const ParseLocation &loc,
            const string &name, const fkl_kvpair &kvpair)
{
	FklKvPairList tmp;
	tmp.insert(kvpair.key, kvpair.value);
	return fkl_bitmask(loc, name, tmp);
}
#define BITMASK(name, ...) \
    ::pp::fkl_bitmask(THIS_LOCATION, name, (__VA_ARGS__))
#define ANON_BITMASK(...)   \
    ::pp::fkl_bitmask(THIS_LOCATION, "", (__VA_ARGS__))

//
// Declare an enum datatype.
//
// It can take an "unlimited" number of arguments, in the form:
// 	ENUM("name", KV("abc", 1), KV("def", 2))
//
// See the NOTE for BITMASK, above.
//
extern DatatypePtr
fkl_enum(const ParseLocation &loc,
         const string &name, const FklKvPairList &kvlist);

inline DatatypePtr
fkl_enum(const ParseLocation &loc,
         const string &name, const fkl_kvpair &kvpair)
{
	FklKvPairList tmp;
	tmp.insert(kvpair.key, kvpair.value);
	return fkl_enum(loc, name, tmp);
}
#define ENUM(name, ...)  ::pp::fkl_enum(THIS_LOCATION, name, (__VA_ARGS__))
#define ANON_ENUM(...)   ::pp::fkl_enum(THIS_LOCATION, "", (__VA_ARGS__))

// These are helpers for type safety in pp_multi
struct fkl_range
{
	fkl_range(const ConstDatatypePtr &datatype,
		  Value min, Value max)
	    : type(datatype), low(min), high(max) {}
	fkl_range(const string &datatype, Value min, Value max)
	    : low(min), high(max)
	{
		type = runtime::current_context()->resolve_datatype(datatype);
	}
	ConstDatatypePtr type;
	Value low;
	Value high;
};
#define RANGE(d, l, h)  ::pp::fkl_range(d, l, h)
typedef std::vector<fkl_range> FklRangeList;
inline FklRangeList
operator,(const fkl_range &lhs, const fkl_range &rhs)
{
	FklRangeList tmp;
	tmp.push_back(lhs);
	tmp.push_back(rhs);
	return tmp;
}
inline FklRangeList
operator,(const FklRangeList &lhs, const fkl_range &rhs)
{
	FklRangeList tmp(lhs);
	tmp.push_back(rhs);
	return tmp;
}

//
// Declare a multi datatype.
//
//  It can take an "unlimited" number of arguments, in the form:
//        MULTI("name", RANGE("datatype_name1", 0, 1),
//	                RANGE("datatype_name2", 2, 3),
//	                    ...);
extern DatatypePtr
fkl_multi(const ParseLocation &loc,
	      const string &name, const FklRangeList &rangelist);

inline DatatypePtr
fkl_multi(const ParseLocation &loc,
	      const string &name, const fkl_range &range)
{
	FklRangeList tmp;
	tmp.push_back(range);
	return fkl_multi(loc, name, tmp);
}
// Note: copied Tim's "macro magic" from the BITMASK macro definition above
#define MULTI(name, ...)  ::pp::fkl_multi(THIS_LOCATION, name, (__VA_ARGS__))
#define ANON_MULTI(...)   ::pp::fkl_multi(THIS_LOCATION, "", (__VA_ARGS__))

//
// Declare a boolean datatype.
//
extern DatatypePtr
fkl_bool(const ParseLocation &loc,
         const string &name, const string &true_str, const string &false_str);
#define BOOL(name, true_, false_) \
    ::pp::fkl_bool(THIS_LOCATION, name, true_, false_)
#define ANON_BOOL(true_, false_) \
    ::pp::fkl_bool(THIS_LOCATION, "", true_, false_)

//
// Declare a transform datatype wrapper.
// This has to be inline because it is a template.  It has to be a
// template because of boost::lambda.
//
template<typename Tdefunc, typename Tenfunc>
inline DatatypePtr
fkl_xform(const ParseLocation &loc,
          const string &name, const ConstDatatypePtr &real_type,
          const Tdefunc &decode_func, const Tenfunc &encode_func)
{
	DTRACE(TRACE_TYPES, "xform: " + name);

	DASSERT_MSG(!runtime::current_context()->is_readonly(),
	    "current_context is read-only");

	DASSERT_MSG(real_type, "found NULL real_type for xform " + name);

	TransformDatatypePtr xform_ptr = new_pp_transform_datatype(
	    real_type, decode_func, encode_func);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		runtime::current_context()->add_datatype(name, xform_ptr);
	}
	return xform_ptr;
}
template<typename Tdefunc, typename Tenfunc>
inline DatatypePtr
fkl_xform(const ParseLocation &loc,
          const string &name, const string &real_type,
          const Tdefunc &decode_func, const Tenfunc &encode_func)
{
	return fkl_xform(loc, name,
	    runtime::current_context()->resolve_datatype(real_type),
	    decode_func, encode_func);
}
#define XFORM(...)       ::pp::fkl_xform(THIS_LOCATION, ##__VA_ARGS__)
#define ANON_XFORM(...)  ::pp::fkl_xform(THIS_LOCATION, "", ##__VA_ARGS__)
#define LAMBDA // for clarity in callers

//
// Declare a fixed-point datatype.
//
extern DatatypePtr
fkl_fixed(const ParseLocation &loc,
          const string &name, int nbits, const string &units);
inline DatatypePtr
fkl_fixed(const ParseLocation &loc, const string &name, int nbits)
{
	return fkl_fixed(loc, name, nbits, "");
}
inline DatatypePtr
fkl_anon_fixed(const ParseLocation &loc, int nbits, const string &units)
{
	return fkl_fixed(loc, "", nbits, units);
}
inline DatatypePtr
fkl_anon_fixed(const ParseLocation &loc, int nbits)
{
	return fkl_fixed(loc, "", nbits, "");
}
#define FIXED(...)       ::pp::fkl_fixed(THIS_LOCATION, ##__VA_ARGS__)
#define ANON_FIXED(...)  ::pp::fkl_anon_fixed(THIS_LOCATION, ##__VA_ARGS__)

}  // namespace pp

#endif // PP_FAKE_LANGUAGE_H__
