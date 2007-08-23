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
#include "pp_container.h"
#include "pp_path.h"
#include "pp_dirent.h"
#include "pp_register.h"
#include "pp_device.h"
#include "pp_space.h"
#include "pp_scope.h"
#include "pp_datatypes.h"
#include "pp_platform.h"

#include <stdexcept>
using namespace std;

/*
 * get_field()
 *
 * A function to search the tree and return a pointer to
 * the field in question.
 */
const pp_field *
get_field(const pp_container *container, pp_path path)
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
	pp_const_dirent_ptr de = container->dirent(path_front);

	/* did we find the field? */
	if (path.empty() && de->dirent_type() == PP_DIRENT_FIELD) {
		return pp_field_from_dirent(de);
	}

	/* recursive case */
	switch (de->dirent_type()) {
	case PP_DIRENT_SCOPE:
		return get_field(pp_scope_from_dirent(de), path);
	case PP_DIRENT_SPACE:
		return get_field(pp_space_from_dirent(de), path);
	case PP_DIRENT_DEVICE:
		return get_field(pp_device_from_dirent(de), path);
	default:
		break;
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
get_register(const pp_container *container, pp_path path)
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
	pp_const_dirent_ptr de = container->dirent(path_front);

	/* did we find the field? */
	if (path.empty() && de->dirent_type() == PP_DIRENT_REGISTER) {
		return pp_register_from_dirent(de);
	}

	/* recursive case */
	switch (de->dirent_type()) {
	case PP_DIRENT_SCOPE:
		return get_register(pp_scope_from_dirent(de), path);
	case PP_DIRENT_SPACE:
		return get_register(pp_space_from_dirent(de), path);
	case PP_DIRENT_DEVICE:
		return get_register(pp_device_from_dirent(de), path);
	default:
		break;
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
get_dirent(const pp_container *container, pp_path path)
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
	pp_const_dirent_ptr de = container->dirent(path_front);

	/* did we find the field? */
	if (path.empty()) {
		return de.get();
	}

	/* recursive case */
	switch (de->dirent_type()) {
	case PP_DIRENT_SCOPE:
		return get_dirent(pp_scope_from_dirent(de), path);
	case PP_DIRENT_SPACE:
		return get_dirent(pp_space_from_dirent(de), path);
	case PP_DIRENT_DEVICE:
		return get_dirent(pp_device_from_dirent(de), path);
	default:
		break;
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
dirent_defined(const pp_container *container, pp_path path)
{
	const pp_dirent *d = NULL;
	try {
		d = get_dirent(container, path);
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
	pp_direct_field_ptr field = new_pp_direct_field(type);

	field->add_regbits(reg.get(), 0, PP_MASK(width), 0);
	scope->add_register("%" + name, reg);
	scope->add_field(name, field);
}

//FIXME: need comments
//FIXME: need tests
#include <assert.h>
#include <stdarg.h>

static const pp_binding *cur_binding_;
static pp_scope *cur_scope_;
static string cur_scope_name_;
static std::vector<pp_scope*> scope_stack_;
static std::vector<string> scope_name_stack_;
static std::vector<pp_scope_ptr> scope_ptr_stack_;
static pp_scope_ptr cur_scope_ptr_;

void
START_SPACE(pp_space *space)
{
	assert(scope_stack_.empty());
	assert(space);
	cur_scope_ = space;
	cur_binding_ = space->binding();
}

void
OPEN_SCOPE(const string &name)
{
	pp_scope_ptr tmp_scope_ = new_pp_scope();
	tmp_scope_->set_parent(cur_scope_);

	scope_ptr_stack_.push_back(cur_scope_ptr_);
	scope_stack_.push_back(cur_scope_);
	scope_name_stack_.push_back(cur_scope_name_);

	cur_scope_ptr_ = tmp_scope_;
	cur_scope_ = cur_scope_ptr_.get();
	cur_scope_name_ = name;
}

void
CLOSE_SCOPE()
{
	pp_scope *parent = scope_stack_.back();
	parent->add_scope(cur_scope_name_, cur_scope_ptr_);

	cur_scope_ptr_ = scope_ptr_stack_.back();
	scope_ptr_stack_.pop_back();

	cur_scope_ = scope_stack_.back();
	scope_stack_.pop_back();

	cur_scope_name_ = scope_name_stack_.back();
	scope_name_stack_.pop_back();
}

void
REGN(const string &name, pp_regaddr address, pp_bitwidth width)
{
	pp_register_ptr reg_ptr = new_pp_register(
			cur_binding_, address, width);
	cur_scope_->add_register(name, reg_ptr);
}

void
SIMPLE_FIELD(const string &name, pp_const_datatype_ptr type,
		const string &regname, int hi_bit, int lo_bit)
{
	assert(hi_bit >= lo_bit);
	const pp_register *reg = get_register(cur_scope_, pp_path(regname));
	pp_direct_field_ptr field_ptr = new_pp_direct_field(type);
	field_ptr->add_regbits(reg, lo_bit,
			PP_MASK((hi_bit - lo_bit) + 1), 0);
	cur_scope_->add_field(name, field_ptr);
}
void
SIMPLE_FIELD(const string &name, const string &type,
		const string &regname, int hi_bit, int lo_bit)
{
	pp_const_datatype_ptr type_ptr = cur_scope_->resolve_datatype(type);
	SIMPLE_FIELD(name, type_ptr, regname, hi_bit, lo_bit);
}

static void
VA_COMPLEX_FIELD(const string &name, pp_const_datatype_ptr type, va_list args)
{
	pp_direct_field_ptr field_ptr = new_pp_direct_field(type);

	int nbits = 0;
	while (1) {
		const char *regname;
		int hi_bit;
		int lo_bit;

		regname = va_arg(args, const char *);
		if (regname == NULL) {
			break;
		}
		hi_bit = va_arg(args, int);
		lo_bit = va_arg(args, int);

		//FIXME: should be resolve_register()
		const pp_register *reg;
		reg = get_register(cur_scope_, pp_path(regname));
		field_ptr->add_regbits(reg, lo_bit,
			PP_MASK((hi_bit - lo_bit) + 1), nbits);
		nbits += (hi_bit - lo_bit) + 1;
	}

	cur_scope_->add_field(name, field_ptr);
}
//FIXME: there has to be a better, type-safe way
void
COMPLEX_FIELD(const string &name, pp_const_datatype_ptr type, ...)
{
	va_list args;
	va_start(args, type);
	VA_COMPLEX_FIELD(name, type, args);
	va_end(args);
}
void
COMPLEX_FIELD(const string &name, const string &type, ...)
{
	va_list args;
	va_start(args, type);
	VA_COMPLEX_FIELD(name, cur_scope_->resolve_datatype(type), args);
	va_end(args);
}

//FIXME: there has to be a better, type-safe way
pp_bitmask_ptr
BITMASK(const string &name, ...)
{
	va_list args;
	pp_bitmask_ptr bitmask_ptr = new_pp_bitmask();

	va_start(args, name);
	while (1) {
		const char *key;
		pp_value value;

		key = va_arg(args, const char *);
		if (key == NULL) {
			break;
		}
		value = va_arg(args, pp_value);

		bitmask_ptr->add_bit(key, value);
	}
	va_end(args);

	if (name != "") {
		cur_scope_->add_datatype(name, bitmask_ptr);
	}

	return bitmask_ptr;
}

//FIXME: there has to be a better, type-safe way
pp_enum_ptr
ENUM(const string &name, ...)
{
	va_list args;
	pp_enum_ptr enum_ptr = new_pp_enum();

	va_start(args, name);
	while (1) {
		const char *key;
		pp_value value;

		key = va_arg(args, const char *);
		if (key == NULL) {
			break;
		}
		value = va_arg(args, pp_value);

		enum_ptr->add_value(key, value);
	}
	va_end(args);

	if (name != "") {
		cur_scope_->add_datatype(name, enum_ptr);
	}

	return enum_ptr;
}

pp_bool_ptr
BOOL(const string &name, const string &true_str, const string &false_str)
{
	pp_bool_ptr bool_ptr = new_pp_bool(true_str, false_str);

	if (name != "") {
		cur_scope_->add_datatype(name, bool_ptr);
	}

	return bool_ptr;
}
