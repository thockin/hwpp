/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 *
 * Implementation of functions contained in utils.h
 */

#include "utils.h"
using namespace std;

/*
 * get_field()
 *
 * A function to search the tree and return a pointer to
 * the field in question.
 */
pp_const_field_ptr
get_field(pp_const_container_ptr container, pp_path path)
{
	pp_field_ptr null_ptr;

	/* error case */
	if (path.empty()) {
		//FIXME: should this just throw?
		return null_ptr;
	}

	/* grab first element of path */
	string path_front = path.pop_front();

	/* look up the dirent of the next element */
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
	//FIXME: should this just throw?
	return null_ptr;
}

/*
 * get_register()
 *
 * A function to search the tree and return a pointer to
 * the field in question.
 */
pp_const_register_ptr
get_register(pp_const_container_ptr container, pp_path path)
{
	pp_register_ptr null_ptr;

	/* error case */
	if (path.empty()) {
		//FIXME: should this just throw?
		return null_ptr;
	}

	/* grab first element of path */
	string path_front = path.pop_front();

	/* look up the dirent of the next element */
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
	//FIXME: should this just throw?
	return null_ptr;
}
