#include "pp.h"
#include "pp_register.h"
#include "pp_fields.h"
#include "pp_datatypes.h"
#include "pp_scope.h"
#include "pp_dirent.h"
#include "test_helpers.h"
#include "test_binding.h"
#include <iostream>

using namespace std;

/*
 * Display pp_* objects.
 */

/*
 * indent()
 *
 * This function prints a given number of indentations.
 */
static void
indent(int tab_count)
{
	for (int i = 0; i < tab_count; i++) {
		cout << "  ";
	}
}

static void
display_field(const pp_field_const_ptr &field, int depth)
{
	(void)field;
	(void)depth;
}

static void
display_reg(const pp_register_const_ptr &reg, int depth)
{
	(void)reg;
	(void)depth;
}

static void
display_tree(const pp_scope_const_ptr &scope, int depth)
{
	depth++;

	for (size_t i = 0; i < scope->n_datatypes(); i++) {
		indent(depth);
		cout << "datatype: "
		     << scope->datatype_name(i)
		     << endl;
	}

	for (size_t i = 0; i < scope->n_dirents(); i++) {
		const pp_dirent_const_ptr &dirent = scope->dirent(i);

		indent(depth);

		if (dirent->is_scope()) {
			cout << "scope: "
			     << scope->dirent_name(i)
			     << endl;
			display_tree(pp_scope_from_dirent(dirent), depth);
		} else if (dirent->is_field()) {
			cout << "field: "
			     << scope->dirent_name(i)
			     << endl;
			display_field(pp_field_from_dirent(dirent), depth);
		} else if (dirent->is_register()) {
			cout << "register: "
			     << scope->dirent_name(i)
			     << endl;
			display_reg(pp_register_from_dirent(dirent), depth);
		}
	}
}

/* this is the externally visible interface */
void
display_tree(pp_scope_const_ptr &scope)
{
	cout << "root" << endl;
	display_tree(scope, 0);
}

