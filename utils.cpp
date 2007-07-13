/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 *
 * Implementation of functions contained in utils.h
 * These functions are used for getting a specified field
 * or register.
 */

#include "utils.h"
using namespace std;

/*
 * get_field
 * A function to search the tree and return a pointer to
 * the field in question.
 */
pp_field_ptr get_field(const pp_scope_ptr item, pp_path path)
{
	pp_field_ptr null_ptr;

	/* Error Case */
	if (path.empty()) {
		return null_ptr;
	}

	/* Grab first element of path */
	string path_front = path.front();
	path.pop_front();
	/* Non Error, Recursive Cases */
	if ((item->dirents.find(path_front))->is_scope()) {
		return get_field((item->dirents.find(path_front))->as_scope(), path);
	} else if ((item->dirents.find(path_front))->is_field()) {
		return (item->dirents.find(path_front))->as_field();
	}
	return null_ptr;
}

pp_field_ptr get_field(const pp_space_ptr item, pp_path path)
{
	pp_scope_ptr scope = item;
	return get_field(scope, path);
}

pp_field_ptr get_field( const pp_device_ptr item, pp_path path )
{
	pp_field_ptr null_ptr;

	/* Error Case */
	if (path.empty()) {
		return null_ptr;
	}

	/* Grab first item of path */
	string path_front = path.front();
	path.pop_front ();

	/* Non Error Cases */
	if (item->dirents.find(path_front)->is_device()) {
		return get_field((item->dirents.find(path_front))->as_device(), path);
	} else if ((item->dirents.find(path_front))->is_space()) {
		return get_field((item->dirents.find(path_front))->as_space(), path);
	} else if ((item->dirents.find(path_front))->is_field()) {
		return (item->dirents.find(path_front))->as_field();
	}
	return null_ptr;
}

pp_field_ptr get_field( const pp_platform_ptr item, pp_path path )
{
	pp_device_ptr device = item;
	return get_field(device, path);
}

/*
 * get_register
 * A function to search the tree and return a pointer to
 * the field in question.
 */
pp_register_ptr get_register(const pp_scope_ptr item, pp_path path)
{
	pp_register_ptr null_ptr;

	/* Error Case */
	if (path.empty()) {
		return null_ptr;
	}

	/* Grab first element of path */
	string path_front = path.front();
	path.pop_front();

	/* Non Error, Recursive Cases */
	if ((item->dirents.find(path_front))->is_scope()) {
		return get_register((item->dirents.find(path_front))->as_scope(), path);
	} else if ((item->dirents.find(path_front))->is_register()) {
		return (item->dirents.find(path_front))->as_register();
	}
	return null_ptr;
}

pp_register_ptr get_register(const pp_space_ptr item, pp_path path)
{
	pp_scope_ptr scope = item;
	return get_register(scope, path);
}

pp_register_ptr get_register(const pp_device_ptr item, pp_path path)
{
	pp_register_ptr null_ptr;

	/* Error Case */
	if (path.empty()) {
		return null_ptr;
	}

	/* Grab first item of path */
	string path_front = path.front();
	path.pop_front ();

	/* Non Error Cases */
	if (item->dirents.find(path_front)->is_device()) {
		return get_register((item->dirents.find(path_front))->as_device(), path);
	} else if ((item->dirents.find(path_front))->is_space()) {
		return get_register((item->dirents.find(path_front))->as_space(), path);
	}
	return null_ptr;
}

pp_register_ptr get_register(const pp_platform_ptr item, pp_path path)
{
	pp_device_ptr device = item;
	return get_register(device, path);
}
