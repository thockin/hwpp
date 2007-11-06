/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 *
 * Implementation of functions contained in utils.h
 */

#include "pp.h"
#include "utils.h"
#include "pp_field.h"
#include "pp_fields.h"
#include "pp_path.h"
#include "pp_dirent.h"
#include "pp_register.h"
#include "pp_scope.h"
#include "pp_datatypes.h"
#include "device_init.h"

#include <stdexcept>
using namespace std;

/*
 * get_field()
 *
 * A function to search the tree and return a pointer to
 * the field in question.
 *
 * NOTE: this takes a copy of the path, since it will be modified.
 */
const pp_field *
get_field(const pp_scope *scope, pp_path path)
{
	/* error case */
	if (path.empty()) {
		throw std::out_of_range("path not found");
	}

	/* grab first element of path */
	string path_front = path.pop_front();

	/*
	 * Look up the dirent of the next element.  This can throw
	 * std::out_of_range if the dirent does not exist.
	 */
	const pp_dirent *de;
	if (path_front == "..") {
		de = scope->parent();
	} else {
		de = scope->dirent(path_front);
	}

	/* did we find the field? */
	if (path.empty() && de->dirent_type() == PP_DIRENT_FIELD) {
		return pp_field_from_dirent(de);
	}

	/* recursive case */
	if (de->dirent_type() == PP_DIRENT_SCOPE) {
		return get_field(pp_scope_from_dirent(de), path);
	}

	/* default error case */
	throw std::out_of_range("path is not a field");
}

/*
 * get_register()
 *
 * A function to search the tree and return a pointer to
 * the field in question.
 *
 * NOTE: this takes a copy of the path, since it will be modified.
 */
const pp_register *
get_register(const pp_scope *scope, pp_path path)
{
	/* error case */
	if (path.empty()) {
		throw std::out_of_range("path not found");
	}

	/* grab first element of path */
	string path_front = path.pop_front();

	/*
	 * Look up the dirent of the next element.  This can throw
	 * std::out_of_range if the dirent does not exist.
	 */
	const pp_dirent *de;
	if (path_front == "..") {
		de = scope->parent();
	} else {
		de = scope->dirent(path_front);
	}

	/* did we find the field? */
	if (path.empty() && de->dirent_type() == PP_DIRENT_REGISTER) {
		return pp_register_from_dirent(de);
	}

	/* recursive case */
	if (de->dirent_type() == PP_DIRENT_SCOPE) {
		return get_register(pp_scope_from_dirent(de), path);
	}

	/* default error case */
	throw std::out_of_range("path is not a register");
}

/*
 * get_dirent()
 *
 * A function to search the tree and return a pointer to
 * the dirent in question.
 *
 * NOTE: this takes a copy of the path, since it will be modified.
 */
const pp_dirent *
get_dirent(const pp_scope *scope, pp_path path)
{
	/* error case */
	if (path.empty()) {
		throw std::out_of_range("path not found");
	}

	/* grab first element of path */
	string path_front = path.pop_front();

	/*
	 * Look up the dirent of the next element.  This can throw
	 * std::out_of_range if the dirent does not exist.
	 */
	const pp_dirent *de;
	if (path_front == "..") {
		de = scope->parent();
	} else {
		de = scope->dirent(path_front);
	}

	/* did we find the field? */
	if (path.empty()) {
		return de;
	}

	if (de->dirent_type() == PP_DIRENT_SCOPE) {
		return get_dirent(pp_scope_from_dirent(de), path);
	}

	/* default error case */
	throw std::out_of_range("path is not a known dirent_type");
}

/*
 * dirent_defined()
 *
 * Tests whether the pp_path resolves to a defined dirent.
 */
bool
dirent_defined(const pp_scope *scope, const pp_path &path)
{
	const pp_dirent *d = NULL;
	try {
		d = get_dirent(scope, path);
	} catch (std::out_of_range e) {
	}

	return (d != NULL);
}

//FIXME: need comments
//FIXME: need tests
static pp_scope_ptr cur_scope;
static std::vector<pp_scope_ptr> scope_stack;

static string cur_scope_name;
static std::vector<string> scope_name_stack;

static pp_const_binding_ptr cur_binding;
static std::vector<pp_const_binding_ptr> binding_stack;

/*
 * Resolve a path to a field, relative to the current scope.  If the specified
 * path does not exist or is not a field, throw std::out_of_range.
 */
const pp_field *
GET_FIELD(const pp_path &path)
{
	DASSERT(cur_scope.get() != NULL);
	return get_field(cur_scope.get(), path);
}

/*
 * Resolve a path to a register, relative to the current scope.  If the
 * specified path does not exist or is not a register, throw
 * std::out_of_range.
 */
const pp_register *
GET_REGISTER(const pp_path &path)
{
	if (path == "0")
		return magic_zeros;
	if (path == "1")
		return magic_ones;

	DASSERT(cur_scope.get() != NULL);
	return get_register(cur_scope.get(), path);
}

/*
 * Check whether a path resolves successfully.
 */
bool
DEFINED(const pp_path &path)
{
	DASSERT(cur_scope.get() != NULL);
	return dirent_defined(cur_scope.get(), path);
}

/*
 * Start a new platform scope.  A pltform scope is the top-level scope in the
 * hierarchy.
 */
pp_scope_ptr
NEW_PLATFORM()
{
	// platform is the top level, cur_scope must not exist
	DASSERT(cur_scope.get() == NULL);
	DASSERT(scope_stack.empty());

	OPEN_SCOPE("");

	// FIXME: take these out when we have a real language
	global_datatypes_init(cur_scope.get());
	pci_datatypes_init(cur_scope.get());

	DASSERT(cur_scope.get() != NULL);
	return cur_scope;
}

/*
 * Start a new scope.
 */
void
OPEN_SCOPE(const string &name, pp_const_binding_ptr binding)
{
	pp_scope_ptr tmp_scope_ = new_pp_scope(binding);

	// if we are not opening a top-level scope, save the current scope
	if (cur_scope) {
		tmp_scope_->set_parent(cur_scope.get());
		scope_stack.push_back(cur_scope);
		scope_name_stack.push_back(cur_scope_name);
	}
	// set cur_scope to the new scope
	cur_scope = tmp_scope_;
	cur_scope_name = name;

	// if we are starting a new binding, save the old binding
	if (cur_binding && binding) {
		binding_stack.push_back(cur_binding);
		cur_binding = binding;
	}
}

/*
 * Close the current scope.
 */
void
CLOSE_SCOPE()
{
	// there had better be a current scope and a parent scope
	DASSERT(cur_scope);
	DASSERT(scope_stack.back());

	// add the current scope to the parent
	pp_scope_ptr parent = scope_stack.back();
	parent->add_dirent(cur_scope_name, cur_scope);

	// if the current scope is bound, restore the previous binding
	if (cur_scope->binding()) {
		cur_binding = binding_stack.back();
		binding_stack.pop_back();
	}

	// restore the parent scope and scope_name
	cur_scope = scope_stack.back();
	scope_stack.pop_back();
	cur_scope_name = scope_name_stack.back();
	scope_name_stack.pop_back();
}

/*
 * Define a register.
 */
void
REGN(const string &name, pp_regaddr address, pp_bitwidth width)
{
	// enforce that registers start with '%'
	DASSERT(name[0] == '%');
	// enforce unique register names
	DASSERT(!DEFINED(name));

	pp_register_ptr reg_ptr = new_pp_register(
			cur_binding.get(), address, width);
	cur_scope->add_dirent(name, reg_ptr);
}

/*
 * Define a simple regbits_field from a single range of bits from a single
 * register.
 *
 * NOTE: this can not simply be a macro, because COMPLEX_FIELD takes a
 * "char *" rather than a "string".
 */
void
SIMPLE_FIELD(const string &name, const pp_datatype *type,
		const string &regname, int hi_bit, int lo_bit)
{
	COMPLEX_FIELD(name, type, {regname.c_str(), hi_bit, lo_bit});
}
void
SIMPLE_FIELD(const string &name, const string &type_str,
		const string &regname, int hi_bit, int lo_bit)
{
	const pp_datatype *type = cur_scope->resolve_datatype(type_str);
	SIMPLE_FIELD(name, type, regname, hi_bit, lo_bit);
}

/*
 * Define a regbits_field from an array of bitranges.
 *
 * ASSUMPTION: bit ranges are added sequentially from lowest to highest.
 */
void
COMPLEX_FIELD_(const string &name, const pp_datatype *type,
		const bitrange_ *bitrange)
{
	// sanity
	DASSERT(type);
	DASSERT(bitrange);
	// enforce unique field names
	DASSERT(!DEFINED(name));

	// create a temporary field and add each bitrange
	pp_regbits_field_ptr field_ptr = new_pp_regbits_field(type);
	int ttl_bits = 0;
	while (bitrange->regname) {
		// the register must be defined
		DASSERT(DEFINED(bitrange->regname));
		// the hi_bit # can not be less than the lo_bit #
		DASSERT(bitrange->hi_bit >= bitrange->lo_bit);

		const pp_register *reg = GET_REGISTER(bitrange->regname);
		int nbits = (bitrange->hi_bit - bitrange->lo_bit) + 1;
		DASSERT(nbits <= reg->width());

		// add the current bitrange at the next free bit number
		field_ptr->add_regbits(reg, bitrange->lo_bit, PP_MASK(nbits),
				ttl_bits);
		ttl_bits += nbits;

		// next bitrange
		bitrange++;
	}

	cur_scope->add_dirent(name, field_ptr);
}
void
COMPLEX_FIELD_(const string &name, const string &type,
		const bitrange_ *bitrange)
{
	COMPLEX_FIELD_(name, cur_scope->resolve_datatype(type), bitrange);
}

/*
 * Define a register and a field that consumes that register.
 */
void
REGFIELDN(const string &name, pp_regaddr address, const pp_datatype *type,
		pp_bitwidth width)
{
	string regname = "%" + name;
	REGN(regname, address, width);
	SIMPLE_FIELD(name, type, regname, width-1, 0);
}
void
REGFIELDN(const string &name, pp_regaddr address, const string &type,
		pp_bitwidth width)
{
	REGFIELDN(name, address, cur_scope->resolve_datatype(type), width);
}

/*
 * Define a pp_int datatype.
 */
pp_int *
INT(const string &name, const string &units)
{
	// sanity
	DASSERT(cur_scope);

	pp_int_ptr int_ptr = new_pp_int(units);
	if (name == "") {
		cur_scope->add_datatype(int_ptr);
	} else {
		cur_scope->add_datatype(name, int_ptr);
	}
	return int_ptr.get();
}

pp_bitmask *
BITMASK_(const string &name, kvpair_ *value)
{
	// sanity
	DASSERT(cur_scope);
	DASSERT(value);

	pp_bitmask_ptr bitmask_ptr = new_pp_bitmask();
	while (value->key) {
		bitmask_ptr->add_bit(value->key, value->value);
		value++;
	}

	if (name == "") {
		cur_scope->add_datatype(bitmask_ptr);
	} else {
		cur_scope->add_datatype(name, bitmask_ptr);
	}

	return bitmask_ptr.get();
}

pp_enum *
ENUM_(const string &name, kvpair_ *value)
{
	// sanity
	DASSERT(cur_scope);
	DASSERT(value);

	pp_enum_ptr enum_ptr = new_pp_enum();
	while (value->key) {
		enum_ptr->add_value(value->key, value->value);
		value++;
	}

	if (name == "") {
		cur_scope->add_datatype(enum_ptr);
	} else {
		cur_scope->add_datatype(name, enum_ptr);
	}

	return enum_ptr.get();
}

pp_bool *
BOOL(const string &name, const string &true_str, const string &false_str)
{
	// sanity
	DASSERT(cur_scope);

	pp_bool_ptr bool_ptr = new_pp_bool(true_str, false_str);
	if (name == "") {
		cur_scope->add_datatype(bool_ptr);
	} else {
		cur_scope->add_datatype(name, bool_ptr);
	}

	return bool_ptr.get();
}
