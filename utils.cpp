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
	pp_const_dirent_ptr de = scope->dirent(path_front);

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
	pp_const_dirent_ptr de = scope->dirent(path_front);

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
	pp_const_dirent_ptr de = scope->dirent(path_front);

	/* did we find the field? */
	if (path.empty()) {
		return de.get();
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
dirent_defined(const pp_scope *scope, pp_path path)
{
	const pp_dirent *d = NULL;
	try {
		d = get_dirent(scope, path);
	} catch (std::out_of_range e) {
	}

	return (d != NULL);
}

/*
 * regfield()
 *
 * Create a register and a field which consumes that register.
 */
void
regfield(const string &name, pp_scope *scope, const pp_binding *binding,
    pp_regaddr address, pp_bitwidth width, pp_const_datatype_ptr type)
{
	pp_register_ptr reg = new_pp_register(binding, address, width);
	pp_regbits_field_ptr field = new_pp_regbits_field(type);

	field->add_regbits(reg.get(), 0, PP_MASK(width), 0);
	scope->add_dirent("%" + name, reg);
	scope->add_dirent(name, field);
}

//FIXME: need comments
//FIXME: need tests
#include <assert.h>
#include <stdarg.h>

static pp_scope_ptr cur_scope;
static std::vector<pp_scope_ptr> scope_stack;

static string cur_scope_name;
static std::vector<string> scope_name_stack;

static pp_const_binding_ptr cur_binding;
static std::vector<pp_const_binding_ptr> binding_stack;

pp_scope_ptr
NEW_PLATFORM()
{
	assert(cur_scope.get() == NULL);
	assert(scope_stack.empty());

	OPEN_SCOPE("");

	/* FIXME: take these out when we have a real language */
	global_datatypes_init(cur_scope.get());
	pci_datatypes_init(cur_scope.get());

	return cur_scope;
}

void
OPEN_SCOPE(const string &name, pp_const_binding_ptr binding)
{
	pp_scope_ptr tmp_scope_ = new_pp_scope(binding);

	if (cur_scope) {
		tmp_scope_->set_parent(cur_scope.get());
		scope_stack.push_back(cur_scope);
		scope_name_stack.push_back(cur_scope_name);
	}
	cur_scope = tmp_scope_;
	cur_scope_name = name;

	if (cur_binding) {
		binding_stack.push_back(cur_binding);
	}
	cur_binding = binding;
}

void
CLOSE_SCOPE()
{
	pp_scope_ptr parent = scope_stack.back();
	parent->add_dirent(cur_scope_name, cur_scope);

	if (cur_scope->binding()) {
		cur_binding = binding_stack.back();
		binding_stack.pop_back();
	}

	cur_scope = scope_stack.back();
	scope_stack.pop_back();

	cur_scope_name = scope_name_stack.back();
	scope_name_stack.pop_back();

}

void
REGN(const string &name, pp_regaddr address, pp_bitwidth width)
{
	pp_register_ptr reg_ptr = new_pp_register(
			cur_binding.get(), address, width);
	cur_scope->add_dirent(name, reg_ptr);
}

void
SIMPLE_FIELD(const string &name, pp_const_datatype_ptr type,
		const string &regname, int hi_bit, int lo_bit)
{
	assert(hi_bit >= lo_bit);
	const pp_register *reg;

	reg = get_register(cur_scope.get(), pp_path(regname));
	pp_regbits_field_ptr field_ptr = new_pp_regbits_field(type);
	field_ptr->add_regbits(reg, lo_bit,
			PP_MASK((hi_bit - lo_bit) + 1), 0);
	cur_scope->add_dirent(name, field_ptr);
}
void
SIMPLE_FIELD(const string &name, const string &type_str,
		const string &regname, int hi_bit, int lo_bit)
{
	pp_const_datatype_ptr type = cur_scope->resolve_datatype(type_str);
	SIMPLE_FIELD(name, type, regname, hi_bit, lo_bit);
}

//assumption: bit rangess are added sequentially from lowest to highest.
void
COMPLEX_FIELD_(const string &name, pp_const_datatype_ptr type, bitrange_ *bit)
{
	pp_regbits_field_ptr field_ptr = new_pp_regbits_field(type);

	int ttl_bits = 0;
	while (bit->regname) {
		const pp_register *reg;
		int nbits = (bit->hi_bit - bit->lo_bit) + 1;

		reg = get_register(cur_scope.get(), pp_path(bit->regname));
		field_ptr->add_regbits(reg, bit->lo_bit, PP_MASK(nbits),
				ttl_bits);
		ttl_bits += nbits;

		bit++;
	}

	cur_scope->add_dirent(name, field_ptr);
}
void
COMPLEX_FIELD_(const string &name, const string &type, bitrange_ *bit)
{
	COMPLEX_FIELD_(name, cur_scope->resolve_datatype(type), bit);
}

pp_bitmask_ptr
BITMASK_(const string &name, kvpair_ *value)
{
	pp_bitmask_ptr bitmask_ptr = new_pp_bitmask();

	while (value->key) {
		bitmask_ptr->add_bit(value->key, value->value);
		value++;
	}

	if (name != "") {
		cur_scope->add_datatype(name, bitmask_ptr);
	}

	return bitmask_ptr;
}

pp_enum_ptr
ENUM_(const string &name, kvpair_ *value)
{
	pp_enum_ptr enum_ptr = new_pp_enum();

	while (value->key) {
		enum_ptr->add_value(value->key, value->value);
		value++;
	}

	if (name != "") {
		cur_scope->add_datatype(name, enum_ptr);
	}

	return enum_ptr;
}

pp_bool_ptr
BOOL(const string &name, const string &true_str, const string &false_str)
{
	pp_bool_ptr bool_ptr = new_pp_bool(true_str, false_str);

	if (name != "") {
		cur_scope->add_datatype(name, bool_ptr);
	}

	return bool_ptr;
}
