#ifndef PP_TESTS_TEST_HELPERS_H__
#define PP_TESTS_TEST_HELPERS_H__
/*
 * test_helpers.h
 *
 * Functions to help writing tests.
 */

#include "pp.h"
#include "pp_test.h"

/*
 * generate_random_platform()
 *
 * Randomly generate a platform tree.  The distribution of items can be
 * controlled via MAX_X values defined in the corresponding .cpp file.
 */
pp_scope_ptr generate_random_platform();

/*
 * display_tree()
 *
 * Given an input tree, display it neatly on the screen using tabs as
 * a means of indicating ownership/tree structure.
 */
void display_tree(const pp_scope *container);

#endif /* PP_TESTS_TEST_HELPERS_H__ */