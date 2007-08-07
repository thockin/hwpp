/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 *
 * utils.h
 * Two functions, one to get a named field from the tree, the
 * second to get a named register from the tree.
 */

#include "pp_platform.h"
#include "pp_dirent.h"
#include "pp_device.h"
#include "pp_space.h"
#include "pp_scope.h"
#include "pp_field.h"
#include "pp_datatype.h"
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
const pp_field *get_field(const pp_container *container, pp_path path);

/*
 * get_register()
 *
 * Returns a pointer to the register specified by pp_path.
 * If the register does not exist, or pp_path does not specify a register,
 * this function will throw std::out_of_range.
 */
const pp_register *get_register(const pp_container *container, pp_path path);

/*
 * get_dirent()
 *
 * Returns a pointer to the dirent specified by pp_path.
 * If the dirent does not exist, this function will throw std::out_of_range.
 */
const pp_dirent *get_dirent(const pp_container *container, pp_path path);

/*
 * dirent_defined()
 *
 * Tests whether the pp_path resolves to a defined dirent.
 */
bool dirent_defined(const pp_container *container, pp_path path);

/*
 * regfield()
 *
 * Create a register and a field which consumes that register.
 */
void regfield(const string &name, pp_scope *scope,
    const pp_binding *binding, pp_regaddr address, pp_bitwidth width,
    pp_const_datatype_ptr type);
