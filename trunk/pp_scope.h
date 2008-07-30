// Copyright (c) Tim Hockin, 2007
#ifndef PP_PP_SCOPE_H__
#define PP_PP_SCOPE_H__

#include "pp.h"
#include <stdexcept>
#include <map>
#include "pp_path.h"
#include "pp_dirent.h"
#include "pp_binding.h"
#include "keyed_vector.h"
#include "pp_datatype.h"
#include "pp_register.h"
#include "pp_field.h"
#include "pp_array.h"
#include <boost/enable_shared_from_this.hpp>

//
// pp_scope - a lexical scope.
//
class pp_scope;
typedef boost::shared_ptr<pp_scope> pp_scope_ptr;
typedef boost::shared_ptr<const pp_scope> pp_scope_const_ptr;

class pp_scope: public pp_dirent,
                public boost::enable_shared_from_this<pp_scope>
{
    private:
	pp_scope_const_ptr m_parent;
	pp_binding_const_ptr m_binding;
	keyed_vector<string, pp_dirent_ptr> m_dirents;
	keyed_vector<string, pp_datatype_const_ptr> m_datatypes;
	std::map<string, int> m_bookmarks;

    public:
	explicit pp_scope(const pp_binding_ptr &binding = pp_binding_ptr())
	    : pp_dirent(PP_DIRENT_SCOPE), m_parent(), m_binding(binding)
	{
	}
	virtual ~pp_scope()
	{
	}

	//
	// Get a pointer to the parent scope of this object.  If this
	// scope is the top of the hierarchy, this method returns a
	// pointer to this object.
	//
	pp_scope_const_ptr
	parent() const;

	//
	// Set the parent scope of this object.
	//
	void
	set_parent(const pp_scope_const_ptr &parent);

	//
	// Return a boolean indicating whether this object is the top of the
	// containership hierarchy or not.  This is the same as checking
	// for:
	//     (object->parent() == object)
	//
	bool
	is_root() const;

	//
	// Get the binding of this scope.  If this scope is not bound,
	// climb the scope hierarchy until you find a binding.  If no
	// scope in the hierarchy is bound, return NULL.
	//
	const pp_binding_const_ptr &
	binding() const;
	bool
	is_bound() const;

	//
	// Add a named or unnamed datatype to this scope.
	//
	// Throws:
	// 	pp_parse_error		- invalid name
	//
	//
	void
	add_datatype(const string &name, const pp_datatype_ptr &datatype);

	//
	// Return the number of named datatypes in this scope.
	//
	size_t
	n_datatypes() const;

	//
	// Provide access to the datatypes vector.
	//
	const pp_datatype_const_ptr &
	datatype(int index) const;
	const pp_datatype_const_ptr &
	datatype(string index) const;

	//
	// Return the name of the indexed datatype.
	//
	string
	datatype_name(int index) const;

	//
	// Look up a datatype by name.
	//
	const pp_datatype_const_ptr &
	resolve_datatype(const string &name) const;

	//
	// Add a named dirent to this scope.
	//
	// Throws:
	// 	pp_path::not_found_error	- path not found
	// 	pp_path::invalid_error		- invalid path element
	// 	pp_dirent::conversion_error	- path element is not a scope
	//
	void
	add_dirent(const pp_path::element &name, const pp_dirent_ptr &dirent);

	//
	// Return the number of dirents in this scope.
	//
	size_t
	n_dirents() const;

	//
	// Provide access to the dirents vector.
	//
	// Returns:
	// 	 NULL if element not found, otherwise the desired pp_dirent.
	//
	pp_dirent_ptr
	dirent(int index);
	pp_dirent_ptr
	dirent(string index);
	pp_dirent_const_ptr
	dirent(int index) const;
	pp_dirent_const_ptr
	dirent(string index) const;

	//
	// Return the name of the indexed dirent.
	//
	// Throws:
	// 	std::out_of_range
	//
	const string &
	dirent_name(int index) const;

	//
	// Return a pointer to the specified dirent.
	// NOTE: This takes path as a copy.
	//
	// Returns:
	// 	NULL if path not found.
	// Throws:
	// 	pp_path::invalid_error		- invalid path element
	// 	pp_dirent::conversion_error	- path element is not a scope
	//
	//
	pp_dirent_const_ptr
	lookup_dirent(pp_path path) const;

	//
	// Tests whether the path resolves to a defined dirent.
	//
	bool
	dirent_defined(const pp_path &path) const;

	//
	// Return a pointer to the specified register.
	//
	// Returns:
	// 	NULL if path not found.
	// Throws:
	// 	pp_path::invalid_error		- invalid path element
	// 	pp_dirent::conversion_error	- path element is not a scope
	//
	pp_register_const_ptr
	lookup_register(const pp_path &path) const;

	//
	// Return a pointer to the specified field.
	//
	// Returns:
	// 	NULL if path not found.
	// Throws:
	// 	pp_path::invalid_error		- invalid path element
	// 	pp_dirent::conversion_error	- path element is not a scope
	//
	pp_field_const_ptr
	lookup_field(const pp_path &path) const;

	//
	// Return a pointer to the specified scope.
	//
	// Returns:
	// 	NULL if path not found.
	// Throws:
	// 	pp_path::invalid_error		- invalid path element
	// 	pp_dirent::conversion_error	- path element is not a scope
	//
	pp_scope_const_ptr
	lookup_scope(const pp_path &path) const;

	//
	// Return a pointer to the specified array.
	//
	// Returns:
	// 	NULL if path not found.
	// Throws:
	// 	pp_path::invalid_error		- invalid path element
	// 	pp_dirent::conversion_error	- path element is not a scope
	//
	pp_array_const_ptr
	lookup_array(const pp_path &path) const;

	//
	// Add a bookmark for this scope.
	//
	void
	add_bookmark(const string &name);

	//
	// See if this scope has a particular bookmark.
	//
	bool
	has_bookmark(const string &name) const;

    private:
	// Returned desired dirent specified by path, NULL if not found.
	pp_dirent_const_ptr
	lookup_dirent_internal(pp_path &path) const;
};

#define new_pp_scope(...) pp_scope_ptr(new pp_scope(__VA_ARGS__))

// const
inline pp_scope_const_ptr
pp_scope_from_dirent(const pp_dirent_const_ptr &dirent)
{
	if (dirent->is_scope()) {
		return static_pointer_cast<const pp_scope>(dirent);
	}
	throw pp_dirent::conversion_error("non-scope dirent used as scope");
}
// non-const
inline pp_scope_ptr
pp_scope_from_dirent(const pp_dirent_ptr &dirent)
{
	const pp_dirent_const_ptr &const_dirent = dirent;
	return const_pointer_cast<pp_scope>(
	       pp_scope_from_dirent(const_dirent));
}

#endif // PP_PP_SCOPE_H__
