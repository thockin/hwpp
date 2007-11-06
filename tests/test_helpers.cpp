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
 * Generate a random platform.
 */

/* Give each PP tree item an identifying integer */
#define SCOPE 0
#define FIELD 1
#define REGISTER 2

/* The number of item choices we have (DEVICE, SPACE, SCOPE, FIELD, REGISTER)*/
#define CHOICES 3

/* The maximum number of entries in a tree */
#define MAX_ITEMS 50

/* The maximum number of each item we can place in the tree.  This is used to
 * ensure that a reasonable distribution of items is generated and also as a
 * "backup" stopping mechanism.
 */
#define MAX_SCOPES 30
#define MAX_REGISTERS 150
#define MAX_FIELDS 150

/* Item Counters */
static int field_count = 0;
static int scope_count = 0;
static int register_count = 0;

/* Global Datatypes */
static pp_datatype *integer = new pp_int();

/* Global Bindings */
static pp_binding_ptr binding = new_test_binding();

/*
 * GENERATE_X functions
 * The purpose of this set of functions is to create a
 * randomly generated tree, starting with a platform.
 *
 * These randomly generated trees can be used for isolated testing
 * purposes (for example, testing utils.h).
 * */

static void
generate_scope(pp_scope_ptr root)
{
	/* To a space we can add a register, scope or field */
	int choice = 0;
	srand(time(NULL));

	/* Select a number of items to add to the device */
	int number_of_items = rand() % MAX_ITEMS;

	for (int i=0; i<number_of_items; i++) {
		choice = rand() % CHOICES;
		if (choice == SCOPE && scope_count < MAX_SCOPES) {
			/* Add a Scope */
			pp_scope_ptr scope = new_pp_scope();
			root->add_dirent("SC"+to_string(scope_count),scope);
			scope_count++;
			generate_scope(scope);
		} else if (choice == FIELD && field_count < MAX_FIELDS) {
			/* Add a Field */
			pp_regbits_field_ptr field = new_pp_regbits_field(integer);
			root->add_dirent("F"+to_string(field_count),field);
			field_count++;
		} else if (choice == REGISTER && register_count < MAX_REGISTERS) {
			/* Add a Register */
			pp_register_ptr reg = new_pp_register(binding.get(), 1, BITS16);
			root->add_dirent("R"+to_string(register_count),reg);
			register_count++;
		}
	}
}

/* generate a random platform */
pp_scope_ptr
generate_random_platform()
{
	pp_scope_ptr root = new_pp_scope();
	generate_scope(root);
	return root;
}


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
display_field(const pp_field *field, int depth)
{
}

static void
display_reg(const pp_register *reg, int depth)
{
}

static void
display_tree(const pp_scope *scope, int depth)
{
	depth++;

	for (size_t i = 0; i < scope->n_datatypes(); i++) {
		indent(depth);
		cout << "datatype: "
		     << scope->datatype_name(i)
		     << endl;
	}

	for (size_t i = 0; i < scope->n_dirents(); i++) {
		const pp_dirent *dirent = scope->dirent(i);

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
display_tree(const pp_scope *scope)
{
	cout << "root" << endl;
	display_tree(scope, 0);
}

