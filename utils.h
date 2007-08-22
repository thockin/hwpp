#ifndef PP_UTILS_H__
#define PP_UTILS_H__

/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 */

#include "pp.h"
#include "pp_platform.h"
#include "pp_dirent.h"
#include "pp_device.h"
#include "pp_space.h"
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
get_field(const pp_container *container, pp_path path);

/*
 * get_register()
 *
 * Returns a pointer to the register specified by pp_path.
 * If the register does not exist, or pp_path does not specify a register,
 * this function will throw std::out_of_range.
 */
extern const pp_register *
get_register(const pp_container *container, pp_path path);

/*
 * get_dirent()
 *
 * Returns a pointer to the dirent specified by pp_path.
 * If the dirent does not exist, this function will throw std::out_of_range.
 */
extern const pp_dirent *
get_dirent(const pp_container *container, pp_path path);

/*
 * dirent_defined()
 *
 * Tests whether the pp_path resolves to a defined dirent.
 */
extern bool
dirent_defined(const pp_container *container, pp_path path);

/*
 * regfield()
 *
 * Create a register and a field which consumes that register.
 */
extern void
regfield(const string &name, pp_scope *scope,
		const pp_binding *binding, pp_regaddr address,
		pp_bitwidth width, pp_const_datatype_ptr type);

//FIXME: needs comments
extern void
START_SPACE(pp_space *space);

extern void
OPEN_SCOPE(const string &name);

extern void
CLOSE_SCOPE();

extern void
REGN(const string &name, pp_regaddr address, pp_bitwidth width);
#define REG8(name, address)  REGN(name, address, BITS8)
#define REG16(name, address) REGN(name, address, BITS16)
#define REG32(name, address) REGN(name, address, BITS32)
#define REG64(name, address) REGN(name, address, BITS64)

extern void
SIMPLE_FIELD(const string &name, pp_const_datatype_ptr type,
		const string &regname, int hi_bit, int lo_bit);
extern void
SIMPLE_FIELD(const string &name, const string &type,
		const string &regname, int hi_bit, int lo_bit);
#define ONE_BIT_FIELD(name, type, regname, bit) \
		SIMPLE_FIELD(name, type, regname, bit, bit)

extern void
COMPLEX_FIELD(const string &name, pp_const_datatype_ptr type, ...);
extern void
COMPLEX_FIELD(const string &name, const string &type, ...);

extern pp_bitmask_ptr
BITMASK(const string &name, ...);
#define ANON_BITMASK(...) BITMASK_("", __VA_ARGS__)

extern pp_enum_ptr
ENUM(const string &name, ...);
#define ANON_ENUM(...) ENUM("", __VA_ARGS__)

#endif // PP_UTILS_H__
