// Copyright (c) Tim Hockin, 2007
#ifndef HWPP_SCOPE_H__
#define HWPP_SCOPE_H__

#include "hwpp.h"
#include <stdexcept>
#include <map>
#include "path.h"
#include "dirent.h"
#include "binding.h"
#include "util/keyed_vector.h"
#include "datatype.h"
#include "register.h"
#include "field.h"
#include "array.h"
#include <boost/enable_shared_from_this.hpp>

namespace hwpp {

//
// Scope - a lexical scope.
//
class Scope;
typedef boost::shared_ptr<Scope> ScopePtr;
typedef boost::shared_ptr<const Scope> ConstScopePtr;
typedef boost::weak_ptr<const Scope> WeakConstScopePtr;

class Scope: public Dirent, public boost::enable_shared_from_this<Scope>
{
    private:
	WeakConstScopePtr m_parent;
	ConstBindingPtr m_binding;
	util::KeyedVector<string, DirentPtr> m_dirents;
	util::KeyedVector<string, ConstDatatypePtr> m_datatypes;
	std::map<string, int> m_bookmarks;

    public:
	explicit Scope(const BindingPtr &binding = BindingPtr())
	    : Dirent(DIRENT_TYPE_SCOPE), m_parent(), m_binding(binding)
	{
	}
	virtual ~Scope()
	{
	}

	//
	// Get a pointer to the parent scope of this object.  If this
	// scope is the top of the hierarchy, this method returns a
	// pointer to this object.
	//
	ConstScopePtr
	parent() const;

	//
	// Set the parent scope of this object.
	//
	void
	set_parent(const ConstScopePtr &parent);

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
	const ConstBindingPtr &
	binding() const;
	bool
	is_bound() const;

	//
	// Add a named datatype to this scope.
	//
	// Throws:
	// 	hwpp_parse_error		- invalid name
	//
	//
	void
	add_datatype(const string &name, const DatatypePtr &datatype);

	//
	// Return the number of named datatypes in this scope.
	//
	size_t
	n_datatypes() const;

	//
	// Provide access to the datatypes vector.
	//
	// Returns:
	// 	 NULL if not found, otherwise the desired Datatype.
	//
	ConstDatatypePtr
	datatype(int index) const;
	ConstDatatypePtr
	datatype(string index) const;

	//
	// Return the name of the indexed datatype.
	//
	string
	datatype_name(int index) const;

	//
	// Look up a datatype by name.
	//
	ConstDatatypePtr
	resolve_datatype(const string &name) const;

	//
	// Canonicalize a path string, expanding and finalizing things
	// like bookmarks and array reverse-indices.
	//
	// Returns:
	// 	a valid Path on success.
	// 	an uninitialized, empty Path on error.
	// Throws:
	// 	Path::InvalidError		- invalid path element
	// 	Dirent::ConversionError		- path element is not a scope
	//
	Path
	resolve_path(const Path &path) const;

	//
	// Add a named dirent to this scope.
	//
	// Throws:
	// 	Path::NotFoundError		- path not found
	// 	Path::InvalidError		- invalid path element
	// 	Dirent::ConversionError		- path element is not a scope
	//
	void
	add_dirent(const string &name, const DirentPtr &dirent)
	{
		add_dirent(Path::Element(name), dirent);
	}
	void
	add_dirent(const Path::Element &name, const DirentPtr &dirent);

	//
	// Return the number of dirents in this scope.
	//
	size_t
	n_dirents() const;

	//
	// Provide access to the dirents vector.
	//
	// Returns:
	// 	 NULL if element not found, otherwise the desired Dirent.
	//
	DirentPtr
	dirent(int index);
	DirentPtr
	dirent(string index);
	ConstDirentPtr
	dirent(int index) const;
	ConstDirentPtr
	dirent(string index) const;

	//
	// Return the name of the indexed dirent.
	//
	// Throws:
	// 	std::out_of_range
	//
	const string &
	dirent_name(int index) const;

	// This flag indicates that lookup_dirent() should resolve aliases
	// if the final dirent is an alias.
	static const unsigned RESOLVE_ALIAS = 0x01;

	//
	// Return a pointer to the specified dirent.
	//
	// Returns:
	// 	NULL if path not found.
	// Throws:
	// 	Path::InvalidError		- invalid path element
	// 	Dirent::ConversionError		- path element is not a scope
	//
	//
	ConstDirentPtr
	lookup_dirent(const Path &path, unsigned flags=0) const;

	//
	// Tests whether the path resolves to a defined dirent.
	//
	bool
	dirent_defined(const Path &path) const;

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
	// Walk a path.
	int
	walk_path(const Path &path, unsigned flags,
	          ConstDirentPtr *out_de, Path *out_path) const;
	int
	walk_path_internal(Path &path, unsigned flags,
	          ConstDirentPtr *out_de, Path *out_path) const;
};

#define new_hwpp_scope(...) ::hwpp::ScopePtr(new ::hwpp::Scope(__VA_ARGS__))

// const
inline ConstScopePtr
scope_from_dirent(const ConstDirentPtr &dirent)
{
	if (dirent->is_scope()) {
		return static_pointer_cast<const Scope>(dirent);
	}
	throw Dirent::ConversionError("non-scope dirent used as scope");
}
// non-const
inline ScopePtr
scope_from_dirent(const DirentPtr &dirent)
{
	const ConstDirentPtr &const_dirent = dirent;
	return const_pointer_cast<Scope>(scope_from_dirent(const_dirent));
}

}  // namespace hwpp

#endif // HWPP_SCOPE_H__
