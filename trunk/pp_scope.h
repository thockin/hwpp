/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_SCOPE_H__
#define PP_PP_SCOPE_H__

#include "pp.h"
#include <stdexcept>
#include "pp_path.h"
#include "pp_dirent.h"
#include "pp_binding.h"
#include "keyed_vector.h"
#include "pp_datatype.h"
#include "pp_register.h"
#include "pp_field.h"

/*
 * pp_scope - a lexical scope.
 *
 * Constructors:
 *	(binding?)
 *
 * Notes:
 */
class pp_scope: public pp_dirent
{
    public:
	explicit pp_scope(pp_const_binding_ptr binding = pp_binding_ptr())
	    : pp_dirent(PP_DIRENT_SCOPE), m_binding(binding)
	{
	}
	virtual ~pp_scope()
	{
	}

	/*
	 * pp_scope::parent()
	 *
	 * Get a pointer to the parent scope of this object.  If this
	 * scope is the top of the hierarchy, this method returns a
	 * pointer to this object.
	 */
	const pp_scope *
	parent() const;

	/*
	 * pp_scope::set_parent()
	 *
	 * Set the parent scope of this object.
	 */
	void
	set_parent(const pp_scope *parent);

	/*
	 * pp_scope::is_root()
	 *
	 * Return a boolean indicating whether this object is the top of the
	 * containership hierarchy or not.  This is the same as checking
	 * for:
	 *     (object->parent() == object)
	 */
	bool
	is_root() const;

	/*
	 * pp_scope::binding()
	 *
	 * Get the binding of this scope.  If this scope is not bound,
	 * climb the scope hierarchy until you find a binding.  If no
	 * scope in the hierarchy is bound, return NULL.
	 */
	const pp_binding *
	binding() const;
	bool
	is_bound() const;

	/*
	 * pp_scope::add_datatype(name, datatype)
	 *
	 * Add a named or unnamed datatype to this scope.
	 */
	void
	add_datatype(const string &name, pp_const_datatype_ptr datatype);
	void
	add_datatype(pp_const_datatype_ptr datatype);

	/*
	 * pp_scope::n_datatypes()
	 *
	 * Return the number of named datatypes in this scope.
	 */
	size_t
	n_datatypes() const;

	/*
	 * pp_scope::datatype()
	 *
	 * Provide access to the datatypes vector.
	 */
	const pp_datatype *
	datatype(int index) const;
	const pp_datatype *
	datatype(string index) const;

	/*
	 * pp_scope::datatype_name(index)
	 *
	 * Return the name of the indexed datatype.
	 */
	string
	datatype_name(int index) const;

	/*
	 * pp_scope::resolve_datatype(name)
	 *
	 * Look up a datatype by name.
	 */
	const pp_datatype *
	resolve_datatype(const string &name) const;

	/*
	 * pp_scope::add_dirent(name, dirent)
	 *
	 * Add a named dirent to this scope.
	 */
	void
	add_dirent(const string &name, pp_dirent_ptr dirent);

	/*
	 * pp_scope::n_dirents()
	 *
	 * Return the number of dirents in this scope.
	 */
	size_t
	n_dirents() const;

	/*
	 * pp_scope::dirent()
	 *
	 * Provide access to the dirents vector.
	 */
	const pp_dirent *
	dirent(int index) const;
	const pp_dirent *
	dirent(string index) const;

	/*
	 * pp_scope::dirent_name(index)
	 *
	 * Return the name of the indexed dirent.
	 */
	string
	dirent_name(int index) const;

	/*
	 * pp_scope::lookup_dirent(path)
	 *
	 * Return a pointer to the specified dirent.  If the dirent at the
	 * end of the path is not a field, or if the some element of the
	 * path does not exist, throw std::out_of_range.
	 *
	 * NOTE: This takes path as a copy.
	 */
	const pp_dirent *
	lookup_dirent(pp_path path) const;

	/*
	 * pp_scope::dirent_defined(path)
	 *
	 * Tests whether the path resolves to a defined dirent.  Does not
	 * throw.
	 */
	bool
	dirent_defined(const pp_path &path) const;

	/*
	 * pp_scope::lookup_register(path)
	 *
	 * Return a pointer to the specified register.  If the dirent at the
	 * end of the path is not a register, or if the some element of the
	 * path does not exist, throw std::out_of_range.
	 */
	const pp_register *
	lookup_register(const pp_path &path) const;

	/*
	 * pp_scope::lookup_field(path)
	 *
	 * Return a pointer to the specified field.  If the dirent at the
	 * end of the path is not a field, or if the some element of the
	 * path does not exist, throw std::out_of_range.
	 */
	const pp_field *
	lookup_field(const pp_path &path) const;

	/*
	 * pp_scope::lookup_scope(path)
	 *
	 * Return a pointer to the specified scope.  If the dirent at the
	 * end of the path is not a scope, or if the some element of the
	 * path does not exist, throw std::out_of_range.
	 */
	const pp_scope *
	lookup_scope(const pp_path &path) const;

    private:
	const pp_scope *m_parent;
	pp_const_binding_ptr m_binding;
	keyed_vector<string, pp_const_dirent_ptr> m_dirents;
	keyed_vector<string, pp_const_datatype_ptr> m_datatypes;
	std::vector<pp_const_datatype_ptr> m_anon_datatypes;

	const pp_dirent *
	lookup_dirent_internal(pp_path &path) const;
};
typedef boost::shared_ptr<pp_scope> pp_scope_ptr;
typedef boost::shared_ptr<const pp_scope> pp_const_scope_ptr;

#define new_pp_scope(...) pp_scope_ptr(new pp_scope(__VA_ARGS__))

inline const pp_scope *
pp_scope_from_dirent(const pp_dirent *dirent)
{
	if (dirent->dirent_type() != PP_DIRENT_SCOPE) {
		throw std::runtime_error("non-scope dirent used as scope");
	}
	return static_cast<const pp_scope *>(dirent);
}

#endif // PP_PP_SCOPE_H__
