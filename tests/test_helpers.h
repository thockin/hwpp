#ifndef PP_TESTS_TEST_HELPERS_H__
#define PP_TESTS_TEST_HELPERS_H__
/*
 * test_helpers.h
 *
 * Functions to help writing tests.
 */

#include "pp/pp.h"
#include "pp/scope.h"
#include "pp_test.h"

/*
 * display_tree()
 *
 * Given an input tree, display it neatly on the screen using tabs as
 * a means of indicating ownership/tree structure.
 */
void display_tree(pp::ConstScopePtr &container);

#endif /* PP_TESTS_TEST_HELPERS_H__ */
