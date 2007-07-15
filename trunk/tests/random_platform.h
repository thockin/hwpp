#ifndef PP_TESTS_RANDOM_PLATFORM_H__
#define PP_TESTS_RANDOM_PLATFORM_H__
/*
 * random_platform.h
 *
 * Functions to automatically generate a platform tree filled
 * with registers, fields, devices, spaces and scopes.
 *
 * The trees are randomly generated, but the distribution of items
 * can be controlled via MAX_X values defined in generate_platform.cpp file.
 *
 * Print_tabs and display_tree are used for neatly displaying our randomly
 * generated platform tree.
 */

#include "pp_datatypes.h"
#include "pp_path.h"
#include "pp_platform.h"
#include "pp_device.h"
#include "pp_space.h"
#include "pp_scope.h"
#include "pp_field.h"
#include "pp_fields.h"
#include "pp_register.h"
#include <iostream>

/*
 * generate_random_platform()
 *
 * Randomly generate a platform tree.
 */
pp_platform_ptr generate_random_platform();

/*
 * display_tree
 * Given an input tree, display it neatly on the screen using tabs as
 * a means of indicating ownership/tree structure.
 */
void display_tree(pp_platform_ptr, int);

#endif PP_TESTS_RANDOM_PLATFORM_H__
