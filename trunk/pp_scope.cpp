/* Copyright (c) Tim Hockin, 2007 */
#include "pp.h"
#include "pp_scope.h"
#include <stdexcept>
#include "pp_path.h"
#include "pp_dirent.h"
#include "pp_binding.h"
#include "keyed_vector.h"
#include "pp_datatype.h"
#include "pp_register.h"
#include "pp_field.h"

/*
 * Get a pointer to the parent scope of this object.  If this
 * scope is the top of the hierarchy, this method returns a
 * pointer to this object.
 */
const pp_scope *
pp_scope::parent() const
{
	if (is_root()) {
		return this;
	}
	return m_parent;
}

/*
 * Set the parent scope of this object.
 */
void
pp_scope::set_parent(const pp_scope *parent)
{
	m_parent = parent;
}

/*
 * Return a boolean indicating whether this object is the top of the
 * containership hierarchy or not.  This is the same as checking
 * for:
 *     (object->parent() == object)
 */
bool
pp_scope::is_root() const
{
	return (m_parent == NULL);
}

/*
 * Get the binding of this scope.  If this scope is not bound,
 * climb the scope hierarchy until you find a binding.  If no
 * scope in the hierarchy is bound, return NULL.
 */
const pp_binding *
pp_scope::binding() const
{
	const pp_scope *s = this;
	while (!s->is_bound() && !s->is_root()) {
		s = s->parent();
	}
	return s->m_binding.get();
}
bool
pp_scope::is_bound() const
{
	return m_binding ? true : false;
}

/*
 * Add a named or unnamed datatype to this scope.
 */
void
pp_scope::add_datatype(const string &name, pp_datatype_ptr datatype)
{
	m_datatypes.insert(name, datatype);
}
void
pp_scope::add_datatype(pp_datatype_ptr datatype)
{
	m_anon_datatypes.push_back(datatype);
}

/*
 * Return the number of named datatypes in this scope.
 */
size_t
pp_scope::n_datatypes() const
{
	return m_datatypes.size();
}

/*
 * Provide access to the datatypes vector.
 */
const pp_datatype *
pp_scope::datatype(int index) const
{
	return m_datatypes[index].get();
}
const pp_datatype *
pp_scope::datatype(string index) const
{
	return m_datatypes[index].get();
}

/*
 * Return the name of the indexed datatype.
 */
string
pp_scope::datatype_name(int index) const
{
	return m_datatypes.key_at(index);
}

/*
 * Look up a datatype by name.
 */
const pp_datatype *
pp_scope::resolve_datatype(const string &name) const
{
	try {
		DTRACE(TRACE_TYPES && TRACE_SCOPES,
			"trying to resolve type \"" + name + "\"");
		return m_datatypes[name].get();
	} catch (std::out_of_range &e) {
	}

	if (!is_root()) {
		DTRACE(TRACE_TYPES && TRACE_SCOPES,
			"type \"" + name
			+ "\" not found, climbing scope");
		return m_parent->resolve_datatype(name);
	}

	DTRACE(TRACE_TYPES && TRACE_SCOPES,
		"type \"" + name + "\" not found");
	return NULL;
}

/*
 * Add a named dirent to this scope.
 *
 * Throws:
 * 	pp_path::not_found_error	- path not found
 * 	pp_path::invalid_error		- invalid path element
 * 	pp_dirent::conversion_error	- path element is not a scope
 */
void
pp_scope::add_dirent(const string &name, pp_dirent_ptr dirent)
{
	// convert name to a path element, which will validate and parse it
	pp_path::element elem(name);

	// if we're adding a scope, we need to link it into the tree
	if (dirent->is_scope()) {
		pp_scope *scope = static_cast<pp_scope*>(dirent.get());
		scope->set_parent(this);
	}

	// add it to the list of named dirents
	m_dirents.insert(name, dirent);
}

/*
 * Return the number of dirents in this scope.
 */
size_t
pp_scope::n_dirents() const
{
	return m_dirents.size();
}

/*
 * Provide access to the dirents vector.
 */
const pp_dirent *
pp_scope::dirent(int index) const
{
	return m_dirents[index].get();
}
const pp_dirent *
pp_scope::dirent(string index) const
{
	return m_dirents[index].get();
}

/*
 * Return the name of the indexed dirent.
 */
string
pp_scope::dirent_name(int index) const
{
	return m_dirents.key_at(index);
}

/*
 * Return a pointer to the specified dirent.
 * NOTE: This takes path as a copy.
 *
 * Throws:
 * 	pp_path::not_found_error	- path not found
 * 	pp_path::invalid_error		- invalid path element
 * 	pp_dirent::conversion_error	- path element is not a scope
 */
const pp_dirent *
pp_scope::lookup_dirent(pp_path path) const
{
	if (path.size() != 0 && path.front() == "^") {
		const pp_scope *s = this;
		while (!s->is_bound() && !s->is_root()) {
			s = s->parent();
		}
		path.pop_front();
		return s->lookup_dirent_internal(path);
	}

	if (path.size() == 0) {
		return this;
	}

	return lookup_dirent_internal(path);
}
/* NOTE: this takes path as a non-const reference */
const pp_dirent *
pp_scope::lookup_dirent_internal(pp_path &path) const
{
	/* get the next element of the path */
	pp_path::element path_front = path.pop_front();

	/*
	 * Look up the dirent of the next element.  This will throw
	 * std::out_of_range if the dirent does not exist.
	 */
	const pp_dirent *de;
	if (path_front == "..") {
		de = parent();
	} else {
		try {
			de = dirent(path_front.to_string());
		} catch (std::out_of_range &e) {
			throw pp_path::not_found_error(
			    "path element not found: " + path_front.to_string());
		}
	}

	/* did we find the dirent? */
	if (path.size() == 0) {
		return de;
	}

	/* keep looking */
	if (de->is_scope()) {
		const pp_scope *s = pp_scope_from_dirent(de);
		return s->lookup_dirent_internal(path);
	}

	/* error */
	throw pp_dirent::conversion_error("path element is not a scope: "
	    + to_string(path) + " (" + to_string(de->dirent_type()) + ")");
}

/*
 * Test whether the path resolves to a defined dirent.
 */
bool
pp_scope::dirent_defined(const pp_path &path) const
{
	const pp_dirent *de = NULL;
	try {
		de = lookup_dirent(path);
	} catch (pp_path::not_found_error &e) {
	}

	return (de != NULL);
}

/*
 * Return a pointer to the specified register.
 *
 * Throws:
 * 	pp_path::not_found_error	- path not found
 * 	pp_path::invalid_error		- invalid path element
 * 	pp_dirent::conversion_error	- path element is not a scope
 */
const pp_register *
pp_scope::lookup_register(const pp_path &path) const
{
	const pp_dirent *de = lookup_dirent(path);
	if (de->is_register()) {
		return pp_register_from_dirent(de);
	}
	throw pp_dirent::conversion_error("path is not a register: "
	    + to_string(path));
}

/*
 * Return a pointer to the specified field.
 *
 * Throws:
 * 	pp_path::not_found_error	- path not found
 * 	pp_path::invalid_error		- invalid path element
 * 	pp_dirent::conversion_error	- path element is not a scope
 */
const pp_field *
pp_scope::lookup_field(const pp_path &path) const
{
	const pp_dirent *de = lookup_dirent(path);
	if (de->is_field()) {
		return pp_field_from_dirent(de);
	}
	throw pp_dirent::conversion_error("path is not a field: "
	    + to_string(path));
}

/*
 * Return a pointer to the specified scope.
 *
 * Throws:
 * 	pp_path::not_found_error	- path not found
 * 	pp_path::invalid_error		- invalid path element
 * 	pp_dirent::conversion_error	- path element is not a scope
 */
const pp_scope *
pp_scope::lookup_scope(const pp_path &path) const
{
	const pp_dirent *de = lookup_dirent(path);
	if (de->is_scope()) {
		return pp_scope_from_dirent(de);
	}
	throw pp_dirent::conversion_error("path is not a scope: "
	    + to_string(path));
}
