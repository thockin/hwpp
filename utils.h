#ifndef PP_UTILS_H__
#define PP_UTILS_H__

/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 */

#include "pp.h"
#include "pp_dirent.h"
#include "pp_scope.h"
#include "pp_field.h"
#include "pp_datatype.h"
#include "pp_datatypes.h"
#include "pp_register.h"
#include "pp_binding.h"
#include "pp_path.h"

/*
 * get_field()
 *
 * Returns a pointer to the field specified by pp_path.
 * If the field does not exist, or pp_path does not specify a field, this
 * function will throw std::out_of_range.
 */
extern const pp_field *
get_field(const pp_scope *scope, pp_path path);

/*
 * get_register()
 *
 * Returns a pointer to the register specified by pp_path.
 * If the register does not exist, or pp_path does not specify a register,
 * this function will throw std::out_of_range.
 */
extern const pp_register *
get_register(const pp_scope *scope, pp_path path);

/*
 * get_dirent()
 *
 * Returns a pointer to the dirent specified by pp_path.
 * If the dirent does not exist, this function will throw std::out_of_range.
 */
extern const pp_dirent *
get_dirent(const pp_scope *scope, pp_path path);

/*
 * dirent_defined()
 *
 * Tests whether the pp_path resolves to a defined dirent.
 */
extern bool
dirent_defined(const pp_scope *scope, pp_path path);

/*
 * regfield()
 *
 * Create a register and a field which consumes that register.
 */
extern void
regfield(const string &name, pp_scope *scope,
		const pp_binding *binding, pp_regaddr address,
		pp_bitwidth width, pp_const_datatype_ptr type);


//FIXME:
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
REGN(const string &name, pp_regaddr address, pp_bitwidth width);
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
extern void
SIMPLE_FIELD(const string &name, pp_const_datatype_ptr type,
		const string &regname, int hi_bit, int lo_bit);
extern void
SIMPLE_FIELD(const string &name, const string &type,
		const string &regname, int hi_bit, int lo_bit);
#define ONE_BIT_FIELD(name, type, regname, bit) \
		SIMPLE_FIELD(name, type, regname, bit, bit)


/*
 * COMPLEX_FIELD
 * Create a complex field, give the name, type, registers from which the
 * bits will be taken from, the hi and lo bits.  Note that this function is
 * for handling fields with bits comming from multiple registers and can take
 * and unlimited amount of arguments.  This is a simplified
 * macro function, and the field created will be added to the present
 * scope (as created with OPEN_SCOPE).
 */

extern void
COMPLEX_FIELD(const string &name, pp_const_datatype_ptr type, ...);
extern void
COMPLEX_FIELD(const string &name, const string &type, ...);

/* this is a helper for type-safety */
struct kvpair_ {
	const char *key;
	pp_value value;
};

/*
 * BITMASK
 * A shortcut function for creating a bitmask
 */
extern pp_bitmask_ptr
BITMASK_KV(const string &name, kvpair_ *values);
#define BITMASK(name, ...) BITMASK_KV(name, (kvpair_[]){__VA_ARGS__, {NULL}})
#define ANON_BITMASK(...) BITMASK("", __VA_ARGS__)

/*
 * ENUM
 * A shortcut function for creating an enumeration.
 * It can take an unlimted amount of arguments, in the form:
 * enum_name, { "abc", 1 }, { "def", 2 }, ...
 */
extern pp_enum_ptr
ENUM_KV(const string &name, kvpair_ *values);
#define ENUM(name, ...) ENUM_KV(name, (kvpair_[]){__VA_ARGS__, {NULL}})
#define ANON_ENUM(...) ENUM("", __VA_ARGS__)

/*
 * BOOL
 * Shortcut to create a boolean.
 */
extern pp_bool_ptr
BOOL(const string &name, const string &true_str, const string &false_str);
#define ANON_BOOL(true_str, false_str) BOOL("", true_str, false_str)

#endif // PP_UTILS_H__
