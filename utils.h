/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 *
 * utils.h
 * Two functions, one to get a named field from the tree, the
 * second to get a named register from the tree.
 */

#include "pp_platform.h"
#include "pp_device.h"
#include "pp_space.h"
#include "pp_scope.h"
#include "pp_field.h"
#include "pp_datatype.h"
#include "pp_register.h"
#include "pp_path.h"
#include <iostream>

/*
 * get_field()
 *
 * Returns a pointer to the field specified by pp_path.
 * If the field does not exist, or pp_path does not specify a field,
 * NULL is returned.
 */
pp_field_ptr get_field(const pp_container_ptr container, pp_path path);

/*
 * get_register()
 *
 * Returns a pointer to the register specified by pp_path.
 * If the register does not exist, or pp_path does not specify a register,
 * NULL is returned.
 */
pp_register_ptr get_register(const pp_container_ptr container, pp_path path);
