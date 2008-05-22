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
#include "pp_array.h"
#include "language.h"

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
pp_scope::add_datatype(const string &name, const pp_datatype_ptr &datatype)
{
	if (!lang_valid_datatype_name(name)) {
		throw pp_parse_error("invalid datatype name: " + name);
	}
	m_datatypes.insert(name, datatype);
}
void
pp_scope::add_datatype(const pp_datatype_ptr &datatype)
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

//FIXME: return NULL like dirents?
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
	DTRACE(TRACE_TYPES && TRACE_SCOPES,
		"trying to resolve type \"" + name + "\"");

	keyed_vector<string, pp_const_datatype_ptr>::const_iterator it;
	it = m_datatypes.find(name);
	if (it != m_datatypes.end()) {
		return (*it).get();
	}

	// if not found in currrent scope fall through to parent scope
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
pp_scope::add_dirent(const string &name, const pp_dirent_ptr &new_dirent)
{
	// convert name to a path element, which will validate and parse it
	pp_path::element elem(name);

	// is the name an array access?
	if (elem.is_array()) {
		// if so, we don't support direct indexed writes, just appends
		if (elem.array_mode() != elem.ARRAY_APPEND) {
			throw pp_path::invalid_error(
			    "array write is not an append: "
			    + elem.to_string());
		}
		// does the array already exist?
		pp_dirent *de = dirent(elem.name());
		if (de != NULL) {
			// if so, append the new dirent
			pp_array *ar = pp_array_from_dirent(de);
			ar->append(new_dirent);
		} else {
			// if not, add the array and append the new dirent
			pp_array_ptr ar =
			    new_pp_array(new_dirent->dirent_type());
			m_dirents.insert(elem.name(), ar);
			ar->append(new_dirent);
		}
	} else {
		// if not, just add the new dirent
		m_dirents.insert(elem.name(), new_dirent);
	}

	// if we're adding a scope, we need to link it into the tree
	if (new_dirent->is_scope()) {
		pp_scope *scope = pp_scope_from_dirent(new_dirent.get());
		scope->set_parent(this);
	}
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
pp_dirent *
pp_scope::dirent(int index)
{
	keyed_vector<string, pp_dirent_ptr>::iterator it;
	it = m_dirents.find(index);
	if (it == m_dirents.end()) {
		return NULL;
	}
	return (*it).get();
}
pp_dirent *
pp_scope::dirent(string index)
{
	keyed_vector<string, pp_dirent_ptr>::iterator it;
	it = m_dirents.find(index);
	if (it == m_dirents.end()) {
		return NULL;
	}
	return (*it).get();
}
const pp_dirent *
pp_scope::dirent(int index) const
{
	pp_scope *scope = const_cast<pp_scope *>(this);
	return scope->dirent(index);
}
const pp_dirent *
pp_scope::dirent(string index) const
{
	pp_scope *scope = const_cast<pp_scope *>(this);
	return scope->dirent(index);
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
 * Returns:
 * 	NULL if path not found.
 * Throws:
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
// Returned desired dirent specified by path, NULL if not found.
// NOTE: this takes path as a non-const reference
const pp_dirent *
pp_scope::lookup_dirent_internal(pp_path &path) const
{
	/* get the next element of the path */
	pp_path::element path_front = path.pop_front();

	// look up the dirent of the next element
	const pp_dirent *de;
	if (path_front == "..") {
		// go up one level in the tree
		de = parent();
	} else {
		de = dirent(path_front.name());
		if (de == NULL) {
			return de;
		}
	}

	// handle arrays
	if (path_front.is_array()) {
		// if path is array and dirent is not array: error
		if (!de->is_array()) {
			throw pp_dirent::conversion_error(
			    "path element is not an array: "
			    + path_front.to_string());
		}
		// if path is array_append, return not_found
		if (path_front.array_mode() == path_front.ARRAY_APPEND) {
			return NULL;
		}

		const pp_array *ar = pp_array_from_dirent(de);
		size_t index;

		// if path is an array_tail, but dirent is empty: error
		if (path_front.array_mode() == path_front.ARRAY_TAIL
		 && ar->size() == 0) {
			return NULL;
		}
		// if path is an array_tail, access the last index
		if (path_front.array_mode() == path_front.ARRAY_TAIL) {
			index = ar->size() - 1;
		} else {
			// must be a direct access
			index = path_front.array_index();
		}
		// index into the array
		if (index < 0 || index >= ar->size()) {
			return NULL;
		}
		de = ar->at(index);
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
	return lookup_dirent(path) != NULL;
}

/*
 * Return a pointer to the specified register.
 *
 * Returns:
 * 	NULL if path not found.
 * Throws:
 * 	pp_path::invalid_error		- invalid path element
 * 	pp_dirent::conversion_error	- path element is not a scope
 */
const pp_register *
pp_scope::lookup_register(const pp_path &path) const
{
	const pp_dirent *de = lookup_dirent(path);

	if (de == NULL) {
		return NULL;
	}

	if (!de->is_register()) {
		throw pp_dirent::conversion_error("path is not a register: "
		    + to_string(path));
	}

	return pp_register_from_dirent(de);
}

/*
 * Return a pointer to the specified field.
 *
 * Returns:
 * 	NULL if path not found.
 * Throws:
 * 	pp_path::invalid_error		- invalid path element
 * 	pp_dirent::conversion_error	- path element is not a scope
 */
const pp_field *
pp_scope::lookup_field(const pp_path &path) const
{
	const pp_dirent *de = lookup_dirent(path);

	if (de == NULL) {
		return NULL;
	}

	if (!de->is_field()) {
		throw pp_dirent::conversion_error("path is not a field: "
		    + to_string(path));
	}

	return pp_field_from_dirent(de);
}

/*
 * Return a pointer to the specified scope.
 *
 * Returns:
 * 	NULL if path not found.
 * Throws:
 * 	pp_path::invalid_error		- invalid path element
 * 	pp_dirent::conversion_error	- path element is not a scope
 */
const pp_scope *
pp_scope::lookup_scope(const pp_path &path) const
{
	const pp_dirent *de = lookup_dirent(path);

	if (de == NULL) {
		return NULL;
	}

	if (!de->is_scope()) {
		throw pp_dirent::conversion_error("path is not a scope: "
		    + to_string(path));
	}

	return pp_scope_from_dirent(de);
}

/*
 * Return a pointer to the specified array.
 *
 * Returns:
 * 	NULL if path not found.
 * Throws:
 * 	pp_path::invalid_error		- invalid path element
 * 	pp_dirent::conversion_error	- path element is not a scope
 */
const pp_array *
pp_scope::lookup_array(const pp_path &path) const
{
	const pp_dirent *de = lookup_dirent(path);

	if (de == NULL) {
		return NULL;
	}

	if (!de->is_array()) {
		throw pp_dirent::conversion_error("path is not an array: "
		    + to_string(path));
	}

	return pp_array_from_dirent(de);
}
