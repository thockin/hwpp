#include "pp.h"
#include "pp_register.h"
#include "pp_fields.h"
#include "pp_datatypes.h"
#include "pp_scope.h"
#include "pp_space.h"
#include "pp_device.h"
#include "pp_dirent.h"
#include "pp_platform.h"
#include "test_helpers.h"
#include "test_binding.h"
#include <iostream>

using namespace std;


/*
 * Generate a random platform.
 */

/* Give each PP tree item an identifying integer */
#define DEVICE 0
#define SPACE 1
#define SCOPE 2
#define FIELD 3
#define REGISTER 4

/* The maximum number of entries in a tree */
#define MAX_ITEMS 50

/* The number of item choices we have (DEVICE, SPACE, SCOPE, FIELD, REGISTER)*/
#define CHOICES 5

/* The maximum number of each item we can place in the tree.  This is used to
 * ensure that a reasonable distribution of items is generated and also as a
 * "backup" stopping mechanism.
 */
#define MAX_DEVICES 5
#define MAX_SPACES 8
#define MAX_SCOPES 10
#define MAX_REGISTERS 12
#define MAX_FIELDS 15

/* Item Counters */
static int device_count = 0;
static int space_count = 0;
static int field_count = 0;
static int scope_count = 0;
static int register_count = 0;

/* Global Datatypes */
static pp_datatype_ptr integer = new_pp_int();

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
			root->add_scope("SC"+to_string(scope_count),scope);
			scope_count++;
			generate_scope(scope);
		} else if (choice == FIELD && field_count < MAX_FIELDS) {
			/* Add a Field */
			pp_direct_field_ptr field = new_pp_direct_field(integer);
			root->add_field("F"+to_string(field_count),field);
			field_count++;
		} else if (choice == REGISTER && register_count < MAX_REGISTERS) {
			/* Add a Register */
			pp_register_ptr reg = new_pp_register(binding, 1, BITS16);
			root->add_register("R"+to_string(register_count),reg);
			register_count++;
		}
	}
}

static void
generate_space(pp_space_ptr root)
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
			root->add_scope("SC"+to_string(scope_count),scope);
			scope_count++;
			generate_scope(scope);
		} else if (choice == FIELD && field_count < MAX_FIELDS) {
			/* Add a Field */
			pp_direct_field_ptr field = new_pp_direct_field(integer);
			root->add_field("F"+to_string(field_count),field);
			field_count++;
		} else if (choice == REGISTER && register_count < MAX_REGISTERS) {
			/* Add a Register */
			pp_register_ptr reg = new_pp_register(binding, 1, BITS16);
			root->add_register("R"+to_string(register_count),reg);
			register_count++;
		}
	}
}

static void
generate_device(pp_device_ptr root)
{
	/* We want to add devices, spaces and fields to the input device */
	int choice = 0;
	srand(time(NULL));

	/* Select a number of items to add to the device */
	int number_of_items = rand() % MAX_ITEMS;

	for (int i=0; i<number_of_items; i++) {
		choice = rand() % CHOICES;
		if (choice == DEVICE && device_count < MAX_DEVICES) {
			/* Add a Device */
			pp_device_ptr dev = new_pp_device();
			root->add_device("D"+to_string(device_count),dev);
			device_count++;
			generate_device(dev);
		} else if (choice == SPACE && space_count < MAX_SPACES) {
			/* Add a Space */
			pp_space_ptr space = new_pp_space(binding);
			root->add_space("S"+to_string(space_count),space);
			space_count++;
			generate_space(space);
		} else if (choice == FIELD && field_count < MAX_FIELDS) {
			/* Add a Field */
			pp_direct_field_ptr field = new_pp_direct_field(integer);
			root->add_field("F"+to_string(field_count),field);
			field_count++;
		}
	}
}

/* generate a random platform */
pp_platform_ptr
generate_random_platform()
{
	pp_platform_ptr root = new_pp_platform();
	int choice = 0;
	srand(time(NULL));

	/* Select a number of items to add to the Platform */
	int number_of_items = rand() % MAX_ITEMS;

	for (int i=0; i<number_of_items; i++) {
		choice = rand() % CHOICES;
		if (choice == DEVICE && device_count < MAX_DEVICES) {
			/* Add a Device */
			pp_device_ptr dev = new_pp_device();
			root->add_device("D"+to_string(device_count), dev);
			device_count++;
			generate_device(dev);
		} else if (choice == SPACE && space_count < MAX_SPACES) {
			/* Add a Space */
			pp_space_ptr space = new_pp_space(binding);
			root->add_space("S"+to_string(space_count), space);
			space_count++;
			generate_space(space);
		} else if (choice == FIELD && field_count < MAX_FIELDS) {
			/* Add a Field */
			pp_direct_field_ptr field = new_pp_direct_field(integer);
			root->add_field("F"+to_string(field_count), field);
		}
	}
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
display_field(pp_const_field_ptr field, int depth)
{
}

static void
display_reg(pp_const_register_ptr reg, int depth)
{
}

static void
display_tree(pp_const_container_ptr ctr, int depth)
{
	depth++;

	for (size_t i = 0; i < ctr->datatypes().size(); i++) {
		indent(depth);
		cout << "datatype: "
		     << ctr->datatypes().key_at(i)
		     << endl;
	}

	for (size_t i = 0; i < ctr->dirents().size(); i++) {
		pp_const_dirent_ptr dirent = ctr->dirents()[i];

		indent(depth);

		if (dirent->is_scope()) {
			cout << "scope: "
			     << ctr->dirents().key_at(i)
			     << endl;
			display_tree(pp_scope_from_dirent(dirent), depth);
		} else if (dirent->is_space()) {
			cout << "space: "
			     << ctr->dirents().key_at(i)
			     << endl;
			display_tree(pp_space_from_dirent(dirent), depth);
		} else if (dirent->is_device()) {
			cout << "device: "
			     << ctr->dirents().key_at(i)
			     << endl;
			display_tree(pp_device_from_dirent(dirent), depth);
		} else if (dirent->is_field()) {
			cout << "field: "
			     << ctr->dirents().key_at(i)
			     << endl;
			display_field(pp_field_from_dirent(dirent), depth);
		} else if (dirent->is_register()) {
			cout << "register: "
			     << ctr->dirents().key_at(i)
			     << endl;
			display_reg(pp_register_from_dirent(dirent), depth);
		}
	}
}

/* this is the externally visible interface */
void
display_tree(pp_const_container_ptr ctr)
{
	cout << "root" << endl;
	display_tree(ctr, 0);
}

