#include "pp/pp.h"
#include "pp/register.h"
#include "pp/field_types.h"
#include "pp/datatype_types.h"
#include "pp/scope.h"
#include "pp/dirent.h"
#include "test_helpers.h"
#include "test_binding.h"
#include <iostream>

using namespace std;

/*
 * Display pp objects.
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
display_field(const pp::ConstFieldPtr &field, int depth)
{
	(void)field;
	(void)depth;
}

static void
display_reg(const pp::ConstRegisterPtr &reg, int depth)
{
	(void)reg;
	(void)depth;
}

static void
display_tree(const pp::ConstScopePtr &scope, int depth)
{
	depth++;

	for (size_t i = 0; i < scope->n_datatypes(); i++) {
		indent(depth);
		cout << "datatype: "
		     << scope->datatype_name(i)
		     << endl;
	}

	for (size_t i = 0; i < scope->n_dirents(); i++) {
		const pp::ConstDirentPtr &dirent = scope->dirent(i);

		indent(depth);

		if (dirent->is_scope()) {
			cout << "scope: "
			     << scope->dirent_name(i)
			     << endl;
			display_tree(pp::scope_from_dirent(dirent), depth);
		} else if (dirent->is_field()) {
			cout << "field: "
			     << scope->dirent_name(i)
			     << endl;
			display_field(pp::field_from_dirent(dirent), depth);
		} else if (dirent->is_register()) {
			cout << "register: "
			     << scope->dirent_name(i)
			     << endl;
			display_reg(pp::register_from_dirent(dirent), depth);
		}
	}
}

/* this is the externally visible interface */
void
display_tree(pp::ConstScopePtr &scope)
{
	cout << "root" << endl;
	display_tree(scope, 0);
}

