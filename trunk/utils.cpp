/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 *
 * Implementation of functions contained in utils.h
 */

#include "utils.h"
#include "pp_field.h"
#include "pp_fields.h"
#include "pp_container.h"
#include "pp_path.h"
#include "pp_dirent.h"
#include "pp_register.h"
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
	pp_const_dirent_ptr de = container->dirents()[path_front];

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
	pp_const_dirent_ptr de = container->dirents()[path_front];

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
	pp_const_dirent_ptr de = container->dirents()[path_front];

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
