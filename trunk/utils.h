#ifndef PP_UTILS_H__
#define PP_UTILS_H__

/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 */

#include "pp.h"
#include "pp_dirent.h"
#include "pp_scope.h"
#include "pp_fields.h"
#include "pp_datatype.h"
#include "pp_datatypes.h"
#include "pp_register.h"
#include "pp_binding.h"
#include "pp_path.h"

//FIXME: comments
extern const pp_field *
GET_FIELD(const pp_path &path);

extern const pp_register *
GET_REGISTER(const pp_path &path);

extern bool
DEFINED(const pp_path &path);

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

//FIXME: comment
extern pp_scope_ptr
NEW_PLATFORM();

/*
 * OPEN_SCOPE
 * Create a new scope with the input name.  This pair of open and close scope
 * shortcut functions works like a matching bracket stack.  After you open a scope
 * all subsequent operations (from these shortcut operations, eg ONE_BIT_FIELD)
 * will be placed on this scope.
 */
extern void
OPEN_SCOPE(const string &name, pp_const_binding_ptr binding = pp_binding_ptr());

/*
 * CLOSE_SCOPE
 * Closes the most recent scope. (Similar to bracket matching algorithms).
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
#define REG8(name, address)  REGN(name, address, BITS8)
#define REG16(name, address) REGN(name, address, BITS16)
#define REG32(name, address) REGN(name, address, BITS32)
#define REG64(name, address) REGN(name, address, BITS64)

/*
 * SIMPLE_FIELD
 * Create a simple field, give the name, type, register from which the
 * bits will be taken from, the hi and lo bits.  This is a simplified
 * macro function, and the field created will be added to the present
 * scope (as created with OPEN_SCOPE).
 */
//FIXME: drop this in favor of simply an overloaded "FIELD()" function?
extern void
SIMPLE_FIELD(const string &name, const pp_datatype *type,
		const string &regname, unsigned hi_bit, unsigned lo_bit);
extern void
SIMPLE_FIELD(const string &name, const string &type,
		const string &regname, unsigned hi_bit, unsigned lo_bit);
#define ONE_BIT_FIELD(name, type, regname, bit) \
		SIMPLE_FIELD(name, type, regname, bit, bit)

// This is a helper for type safety in COMPLEX_FIELD()
struct reg_bitrange
{
	reg_bitrange(): regname(""), hi_bit(0), lo_bit(0) {}
	reg_bitrange(const string &reg, unsigned hi, unsigned lo)
	    : regname(reg), hi_bit(hi), lo_bit(lo) {}
	string regname;
	unsigned hi_bit;
	unsigned lo_bit;
};
#define BITS(reg,hi,lo) reg_bitrange(reg, hi, lo)

/*
 * COMPLEX_FIELD
 * Create a complex field, give the name, type, registers from which the
 * bits will be taken from, the hi and lo bits.
 *
 * It can take an "unlimted" amount of arguments, in the form:
 * 	COMPLEX_FIELD("name", "type", BIT("abc", 1, 0), BIT("def", 7, 2))
 *
 * NOTE: If I could check this in under an assumed name, I would.  I am
 * embarrassed to put my name on this.  I just can't see a cleaner,
 * type-safe solution right now.  I'm sure one exists.  This will all go
 * away when we have a real language, and we can switch this to take a
 * vector or something.
 */
extern void
COMPLEX_FIELD(const string &name, const pp_datatype *type,
	const reg_bitrange &bits0, const reg_bitrange &bits1=reg_bitrange(),
	const reg_bitrange &bits2=reg_bitrange(),
	const reg_bitrange &bits3=reg_bitrange(),
	const reg_bitrange &bits4=reg_bitrange());
extern void
COMPLEX_FIELD(const string &name, const string &type,
	const reg_bitrange &bits0, const reg_bitrange &bits1=reg_bitrange(),
	const reg_bitrange &bits2=reg_bitrange(),
	const reg_bitrange &bits3=reg_bitrange(),
	const reg_bitrange &bits4=reg_bitrange());

//FIXME: comment
extern void
REGFIELDN(const string &name, const pp_value &address,
		const pp_datatype *type, pp_bitwidth width);
extern void
REGFIELDN(const string &name, const pp_value &address,
		const string &type, pp_bitwidth width);
#define REGFIELD8(name, address, type)  REGFIELDN(name, address, type, BITS8)
#define REGFIELD16(name, address, type) REGFIELDN(name, address, type, BITS16)
#define REGFIELD32(name, address, type) REGFIELDN(name, address, type, BITS32)
#define REGFIELD64(name, address, type) REGFIELDN(name, address, type, BITS64)

extern void
CONSTANT_FIELD(const string &name, const pp_datatype *type,
		const pp_value &value);
extern void
CONSTANT_FIELD(const string &name, const string &type, const pp_value &value);

/*
 * INT
 * A shortcut for creating a pp_int.
 */
extern pp_int *
INT(const string &name, const string &units="");
#define ANON_INT(units) INT("", units)

// This is a helper for type safety in pp_enum and pp_bitmask.
struct kv_pair
{
	kv_pair(): key(""), value(0) {}
	kv_pair(const string &k, const pp_value &v): key(k), value(v) {}
	string key;
	pp_value value;
};
#define KV(k,v) kv_pair(k, v)

/*
 * BITMASK
 * A shortcut function for creating a pp_bitmask.
 * It can take an "unlimted" amount of arguments, in the form:
 * 	BITMASK("name", KV("abc", 1), KV("def", 2))
 *
 * NOTE: If I could check this in under an assumed name, I would.  I am
 * embarrassed to put my name on this.  I just can't see a cleaner,
 * type-safe solution right now.  I'm sure one exists.  This will all go
 * away when we have a real language, and we can switch this to take a
 * vector or something.
 */
extern pp_bitmask *
BITMASK_(const string &name, const string &dflt,
	const kv_pair &kv0, const kv_pair &kv1=kv_pair(),
	const kv_pair &kv2=kv_pair(), const kv_pair &kv3=kv_pair(),
	const kv_pair &kv4=kv_pair(), const kv_pair &kv5=kv_pair(),
	const kv_pair &kv6=kv_pair(), const kv_pair &kv7=kv_pair(),
	const kv_pair &kv8=kv_pair(), const kv_pair &kv9=kv_pair(),
	const kv_pair &kv10=kv_pair(), const kv_pair &kv11=kv_pair(),
	const kv_pair &kv12=kv_pair(), const kv_pair &kv13=kv_pair(),
	const kv_pair &kv14=kv_pair(), const kv_pair &kv15=kv_pair(),
	const kv_pair &kv16=kv_pair(), const kv_pair &kv17=kv_pair(),
	const kv_pair &kv18=kv_pair(), const kv_pair &kv19=kv_pair(),
	const kv_pair &kv20=kv_pair(), const kv_pair &kv21=kv_pair(),
	const kv_pair &kv22=kv_pair(), const kv_pair &kv23=kv_pair(),
	const kv_pair &kv24=kv_pair(), const kv_pair &kv25=kv_pair(),
	const kv_pair &kv26=kv_pair(), const kv_pair &kv27=kv_pair(),
	const kv_pair &kv28=kv_pair(), const kv_pair &kv29=kv_pair(),
	const kv_pair &kv30=kv_pair(), const kv_pair &kv31=kv_pair());
#define BITMASK_DFLT(name, dflt, ...)	BITMASK_(name, dflt, __VA_ARGS__)
#define BITMASK(name, ...)		BITMASK_(name, "", __VA_ARGS__)
#define ANON_BITMASK(...)		BITMASK_("", "", __VA_ARGS__)

/*
 * ENUM
 * A shortcut function for creating a pp_enum.
 * It can take an "unlimted" amount of arguments, in the form:
 * 	ENUM("name", KV("abc", 1), KV("def", 2))
 *
 * NOTE: If I could check this in under an assumed name, I would.  I am
 * embarrassed to put my name on this.  I just can't see a cleaner,
 * type-safe solution right now.  I'm sure one exists.  This will all go
 * away when we have a real language, and we can switch this to take a
 * vector or something.
 */
extern pp_enum *
ENUM(const string &name,
	const kv_pair &kv0, const kv_pair &kv1=kv_pair(),
	const kv_pair &kv2=kv_pair(), const kv_pair &kv3=kv_pair(),
	const kv_pair &kv4=kv_pair(), const kv_pair &kv5=kv_pair(),
	const kv_pair &kv6=kv_pair(), const kv_pair &kv7=kv_pair(),
	const kv_pair &kv8=kv_pair(), const kv_pair &kv9=kv_pair(),
	const kv_pair &kv10=kv_pair(), const kv_pair &kv11=kv_pair(),
	const kv_pair &kv12=kv_pair(), const kv_pair &kv13=kv_pair(),
	const kv_pair &kv14=kv_pair(), const kv_pair &kv15=kv_pair(),
	const kv_pair &kv16=kv_pair(), const kv_pair &kv17=kv_pair(),
	const kv_pair &kv18=kv_pair(), const kv_pair &kv19=kv_pair());
#define ANON_ENUM(...) ENUM("", __VA_ARGS__)

/*
 * BOOL
 * Shortcut to create a boolean.
 */
extern pp_bool *
BOOL(const string &name, const string &true_str, const string &false_str);
#define ANON_BOOL(true_str, false_str) BOOL("", true_str, false_str)

#endif // PP_UTILS_H__
