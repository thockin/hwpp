#include "random_platform.h"
#include "test_binding.h"
#include <iostream>

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

using namespace std;

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

static void generate_scope(pp_scope_ptr root)
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

static void generate_space(pp_space_ptr root)
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

static void generate_device(pp_device_ptr root)
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

/* Generate a Platform base */
pp_platform_ptr generate_random_platform()
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
 * Print Tabs
 * This function prints a given number of tabs.
 * It is used for displaying the randomly generated trees in a usefull and
 * understandable manner.
 */
static void print_tabs(int tab_count)
{
	for (int i = 0; i < tab_count; i++) {
		cout << "\t";
	}
}

/*
 * display_tree
 * A set of functions to "Display" the tree we just created. 
 */
static void display_tree(pp_register_ptr root, int layer)
{
}

static void display_tree(pp_field_ptr root, int layer)
{
}

static void display_tree(pp_scope_ptr root, int layer)
{
	for (size_t i = 0; i < root->dirents.size(); i++) {
		print_tabs(layer+1);
		if (root->dirents[i].is_scope()) {
			cout << "SCOPE-" << root->dirents.key_at(i) << endl;
			display_tree(root->dirents[i].as_scope(), layer+1);
		} else if (root->dirents[i].is_register()) {
			cout << "REGISTER-" << root->dirents.key_at(i) << endl;
			display_tree(root->dirents[i].as_register(), layer+1);
		} else if (root->dirents[i].is_field()) {
			cout << "FIELD-" << root->dirents.key_at(i) << endl;
			display_tree(root->dirents[i].as_field(), layer+1);
		}
	}
}

static void display_tree(pp_space_ptr root, int layer)
{
	for (size_t i = 0; i < root->dirents.size(); i++) {
		print_tabs(layer+1);
		if (root->dirents[i].is_scope()) {
			cout << "SCOPE-" << root->dirents.key_at(i) << endl;
			display_tree(root->dirents[i].as_scope(), layer+1);
		} else if (root->dirents[i].is_register()) {
			cout << "REGISTER-" << root->dirents.key_at(i) << endl;
			display_tree(root->dirents[i].as_register(), layer+1);
		} else if (root->dirents[i].is_field()) {
			cout << "FIELD-" << root->dirents.key_at(i) << endl;
			display_tree(root->dirents[i].as_field(), layer+1);
		}
	}
}

static void display_tree(pp_device_ptr root, int layer)
{
	for (size_t i = 0; i < root->dirents.size(); i++) {
		print_tabs(layer+1);
		if (root->dirents[i].is_space()) {
			cout << "SPACE-" << root->dirents.key_at(i) << endl;
			display_tree(root->dirents[i].as_space(), layer+1);
		} else if (root->dirents[i].is_device()) {
			cout << "DEVICE-" << root->dirents.key_at(i) << endl;
			display_tree(root->dirents[i].as_device(), layer+1);
		} else if (root->dirents[i].is_field()) {
			cout << "FIELD-" << root->dirents.key_at(i) << endl;
			display_tree(root->dirents[i].as_field(), layer+1);
		}
	}
}

void display_tree(pp_platform_ptr root, int layer)
{
	cout << "ROOT" << endl;
	for (size_t i = 0; i < root->dirents.size(); i++) {
		print_tabs(layer+1);
		if (root->dirents[i].is_space()) {
			cout << "SPACE-" << root->dirents.key_at(i) << endl;
			display_tree(root->dirents[i].as_space(), layer+1);
		} else if (root->dirents[i].is_device()) {
			cout << "DEVICE-" << root->dirents.key_at(i) << endl;
			display_tree(root->dirents[i].as_device(), layer+1);
		} else if (root->dirents[i].is_field()) {
			cout << "FIELD-" << root->dirents.key_at(i) << endl;
			display_tree(root->dirents[i].as_field(), layer+1);
		}
	}
}
