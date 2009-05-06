// Copyright (c) Tim Hockin, 2007
#include "pp.h"
#include "printfxx.h"
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
#include "pp_alias.h"
#include "language.h"

//
// Get a pointer to the parent scope of this object.  If this
// scope is the top of the hierarchy, this method returns a
// pointer to this object.
//
pp_scope_const_ptr
pp_scope::parent() const
{
	if (is_root()) {
		return shared_from_this();
	}
	return m_parent.lock();
}

//
// Set the parent scope of this object.
//
void
pp_scope::set_parent(const pp_scope_const_ptr &parent)
{
	m_parent = parent;
}

//
// Return a boolean indicating whether this object is the top of the
// containership hierarchy or not.  This is the same as checking
// for:
//     (object->parent() == object)
//
bool
pp_scope::is_root() const
{
	return (m_parent.lock().get() == NULL);
}

//
// Get the binding of this scope.  If this scope is not bound,
// climb the scope hierarchy until you find a binding.  If no
// scope in the hierarchy is bound, return NULL.
//
const pp_binding_const_ptr &
pp_scope::binding() const
{
	const pp_scope *s = this;
	while (!s->is_bound() && !s->is_root()) {
		s = s->parent().get();
	}
	return s->m_binding;
}
bool
pp_scope::is_bound() const
{
	return m_binding ? true : false;
}

//
// Add a named datatype to this scope.
//
void
pp_scope::add_datatype(const string &name, const pp_datatype_ptr &datatype)
{
	m_datatypes.insert(name, datatype);
}

//
// Return the number of named datatypes in this scope.
//
size_t
pp_scope::n_datatypes() const
{
	return m_datatypes.size();
}

//
// Provide access to the datatypes vector.
//
pp_datatype_const_ptr
pp_scope::datatype(int index) const
{
	pp_datatype_const_ptr dt;
	util::KeyedVector<string, pp_datatype_const_ptr>::const_iterator it;
	it = m_datatypes.find(index);
	if (it != m_datatypes.end()) {
		dt = *it;
	}
	return dt;
}
pp_datatype_const_ptr
pp_scope::datatype(string index) const
{
	pp_datatype_const_ptr dt;
	util::KeyedVector<string, pp_datatype_const_ptr>::const_iterator it;
	it = m_datatypes.find(index);
	if (it != m_datatypes.end()) {
		dt = *it;
	}
	return dt;
}

//
// Return the name of the indexed datatype.
//
string
pp_scope::datatype_name(int index) const
{
	return m_datatypes.key_at(index);
}

//
// Look up a datatype by name.
//
pp_datatype_const_ptr
pp_scope::resolve_datatype(const string &name) const
{
	DTRACE(TRACE_TYPES && TRACE_SCOPES,
		"trying to resolve type \"" + name + "\"");

	// look for it in this scope
	pp_datatype_const_ptr dt = datatype(name);
	if (dt) {
		return dt;
	}

	// if not found in currrent scope fall through to parent scope
	if (!is_root()) {
		DTRACE(TRACE_TYPES && TRACE_SCOPES,
			"type \"" + name
			+ "\" not found, climbing scope");
		return parent()->resolve_datatype(name);
	}

	DTRACE(TRACE_TYPES && TRACE_SCOPES,
		"type \"" + name + "\" not found");
	return pp_datatype_const_ptr();
}

//
// Canonicalize a path string, expanding and finalizing things
// like bookmarks and array reverse-indices.
//
pp_path
pp_scope::resolve_path(const pp_path &path) const
{
	pp_path ret;
	if (walk_path(path, RESOLVE_ALIAS, NULL, &ret) < 0) {
		return pp_path();
	}
	return ret;
}

//
// Add a named dirent to this scope.
//
// Throws:
// 	pp_path::not_found_error	- path not found
// 	pp_path::invalid_error		- invalid path element
// 	pp_dirent::conversion_error	- path element is not a scope
//
void
pp_scope::add_dirent(const pp_path::element &elem,
                     const pp_dirent_ptr &new_dirent)
{
	// is the element an array access?
	if (elem.is_array()) {
		// if so, we don't support direct indexed writes, just appends
		if (elem.array_mode() != elem.ARRAY_APPEND) {
			throw pp_path::invalid_error(
			    "array write is not an append: "
			    + elem.to_string());
		}
		// does the array already exist?
		const pp_dirent_ptr &de = dirent(elem.name());
		if (de != NULL) {
			// if so, append the new dirent
			const pp_array_ptr &ar = pp_array_from_dirent(de);
			ar->append(new_dirent);
		} else {
			// if not, add the array and append the new dirent
			pp_array_ptr ar
			    = new_pp_array(new_dirent->dirent_type());
			m_dirents.insert(elem.name(), ar);
			ar->append(new_dirent);
		}
	} else {
		// if not, just add the new dirent
		m_dirents.insert(elem.name(), new_dirent);
	}
}

//
// Return the number of dirents in this scope.
//
size_t
pp_scope::n_dirents() const
{
	return m_dirents.size();
}

//
// Provide access to the dirents vector.
//
pp_dirent_ptr
pp_scope::dirent(int index)
{
	pp_dirent_ptr de;
	util::KeyedVector<string, pp_dirent_ptr>::iterator it;
	it = m_dirents.find(index);
	if (it != m_dirents.end()) {
		de = *it;
	}
	return de;
}
pp_dirent_ptr
pp_scope::dirent(string index)
{
	pp_dirent_ptr de;
	util::KeyedVector<string, pp_dirent_ptr>::iterator it;
	it = m_dirents.find(index);
	if (it != m_dirents.end()) {
		de = *it;
	}
	return de;
}
pp_dirent_const_ptr
pp_scope::dirent(int index) const
{
	pp_dirent_const_ptr de;
	util::KeyedVector<string, pp_dirent_ptr>::const_iterator it
	    = m_dirents.find(index);
	if (it != m_dirents.end()) {
		de = *it;
	}
	return de;
}
pp_dirent_const_ptr
pp_scope::dirent(string index) const
{
	pp_dirent_const_ptr de;
	util::KeyedVector<string, pp_dirent_ptr>::const_iterator it
	    = m_dirents.find(index);
	if (it != m_dirents.end()) {
		de = *it;
	}
	return de;
}

//
// Return the name of the indexed dirent.
//
const string &
pp_scope::dirent_name(int index) const
{
	return m_dirents.key_at(index);
}

//
// Return a pointer to the specified dirent.
//
// Returns:
// 	NULL if path not found.
// Throws:
// 	pp_path::invalid_error		- invalid path element
// 	pp_dirent::conversion_error	- path element is not a scope
//
pp_dirent_const_ptr
pp_scope::lookup_dirent(const pp_path &path, unsigned flags) const
{
	pp_dirent_const_ptr de;
	if (walk_path(path, flags, &de, NULL) < 0) {
		de = pp_dirent_const_ptr();
	}
	return de;
}

// This is a helper for lookup_dirent() and resolve_path().  It walks a
// path through a scope, producing a final dirent and a canonicalized
// path.
int
pp_scope::walk_path(const pp_path &path, unsigned flags,
                    pp_dirent_const_ptr *out_de,
                    pp_path *out_path) const
{
	pp_path my_path(path);
	const pp_scope *scope = this;

	if (my_path.is_absolute()) {
		while (!scope->is_root()) {
			scope = scope->parent().get();
			if (out_path) {
				out_path->push_back(pp_path::element(".."));
			}
		}
	}

	if (my_path.size() == 0) {
		if (out_de) {
			*out_de = scope->shared_from_this();
		}
		// out_path is correct already
		return 0;
	}

	return scope->walk_path_internal(my_path, flags, out_de, out_path);
}
// NOTE: this takes path as a non-const reference
int
pp_scope::walk_path_internal(pp_path &path, unsigned flags,
                             pp_dirent_const_ptr *out_de,
                             pp_path *out_path) const
{
	// get the next element of the path
	pp_path::element path_front = path.pop_front();

	// look up the dirent of the next element
	pp_dirent_const_ptr de;
	if (path_front == "..") {
		// go up one level in the tree
		de = parent();
		if (out_path) {
			if (out_path->size() > 0 && out_path->back() != "..") {
				out_path->pop_back();
			} else {
				out_path->push_back(path_front);
			}
		}
	} else if (path_front.is_bookmark()) {
		// climb back up the stack, looking for the bookmark
		const pp_scope *s = this;
		while (!s->has_bookmark(path_front.name()) && !s->is_root()) {
			s = s->parent().get();
			if (out_path) {
				out_path->push_back(pp_path::element(".."));
			}
		}
		// did we not find the bookmark?
		if (!s->has_bookmark(path_front.name())) {
			return -1;
		}
		de = s->shared_from_this();
	} else if (path_front.is_array()) {
		// it must be a dirent in the current scope
		de = dirent(path_front.name());
		if (!de) {
			return -1;
		}

		// if path is array and dirent is not array: error
		if (!de->is_array()) {
			throw pp_dirent::conversion_error(
			    "path element is not an array: "
			    + path_front.to_string());
		}
		// if path is array_append, return not_found
		if (path_front.array_mode() == path_front.ARRAY_APPEND) {
			return -1;
		}

		pp_array_const_ptr ar = pp_array_from_dirent(de);
		int index = path_front.array_index();

		// handle a negative index
		if (index < 0) {
			index = ar->size() + index;
		}

		// sanity check the index
		if (index < 0 || size_t(index) >= ar->size()) {
			return -1;
		}

		// index into the array
		de = ar->at(index);
		if (out_path) {
			string name = sprintfxx("%s[%d]",
			                        path_front.name(), index);
			out_path->push_back(pp_path::element(name));
		}
	} else {
		// it must be a dirent in the current scope
		de = dirent(path_front.name());
		if (!de) {
			return -1;
		}
		if (out_path) {
			out_path->push_back(path_front);
		}
	}

	// if it is an alias, dereference it
	if (de->is_alias() && (path.size() > 0 || flags & RESOLVE_ALIAS)) {
		pp_alias_const_ptr a = pp_alias_from_dirent(de);
		de = lookup_dirent(a->link_path());
		if (!de) {
			return -1;
		}
	}

	// did we find the dirent?
	if (path.size() == 0) {
		if (out_de) {
			*out_de = de;
		}
		return 0;
	}

	// keep looking
	if (de->is_scope()) {
		pp_scope_const_ptr s = pp_scope_from_dirent(de);
		return s->walk_path_internal(path, flags, out_de, out_path);
	}

	// error
	throw pp_dirent::conversion_error(sprintfxx(
	    "path element is not a scope: %s (%s)", path, de->dirent_type()));
}

//
// Test whether the path resolves to a defined dirent.
//
bool
pp_scope::dirent_defined(const pp_path &path) const
{
	return lookup_dirent(path) ? true : false;
}

void
pp_scope::add_bookmark(const string &name)
{
	m_bookmarks.insert(std::make_pair(name, 1));
}

bool
pp_scope::has_bookmark(const string &name) const
{
	return (m_bookmarks.find(name) != m_bookmarks.end());
}
