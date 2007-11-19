/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_SCOPE_H__
#define PP_PP_SCOPE_H__

#include "pp.h"
#include "pp_dirent.h"
#include "pp_binding.h"
#include "keyed_vector.h"
#include "pp_datatype.h"
#include "pp_register.h"
#include "pp_field.h"

#include <stdexcept>

/*
 * pp_scope - a lexical scope.
 *
 * Constructors:
 *	()
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
	parent() const
	{
		if (is_root()) {
			return this;
		}
		return m_parent;
	}

	/*
	 * pp_scope::set_parent()
	 *
	 * Set the parent scope of this object.
	 */
	void
	set_parent(const pp_scope *parent)
	{
		m_parent = parent;
	}

	/*
	 * pp_scope::is_root()
	 *
	 * Return a boolean indicating whether this object is the top of the
	 * containership hierarchy or not.  This is the same as checking
	 * for:
	 *     (object->parent() == object)
	 */
	bool
	is_root() const
	{
		return (m_parent == NULL);
	}

	/*
	 * pp_scope::binding()
	 *
	 * Get the binding of this scope.
	 */
	const pp_binding *
	binding() const
	{
		return m_binding.get();
	}

	/*
	 * pp_scope::add_datatype(name, datatype)
	 *
	 * Add a named or unnamed datatype to this scope.
	 */
	void
	add_datatype(const string &name, pp_const_datatype_ptr datatype)
	{
		m_datatypes.insert(name, datatype);
	}
	void
	add_datatype(pp_const_datatype_ptr datatype)
	{
		m_anon_datatypes.push_back(datatype);
	}

	/*
	 * pp_scope::n_datatypes()
	 *
	 * Return the number of named datatypes in this scope.
	 */
	size_t
	n_datatypes() const
	{
		return m_datatypes.size();
	}

	/*
	 * pp_scope::datatype()
	 *
	 * Provide access to the datatypes vector.
	 */
	const pp_datatype *
	datatype(int index) const
	{
		return m_datatypes[index].get();
	}
	const pp_datatype *
	datatype(string index) const
	{
		return m_datatypes[index].get();
	}

	/*
	 * pp_scope::datatype_name(index)
	 *
	 * Return the name of the indexed datatype.
	 */
	string
	datatype_name(int index) const
	{
		return m_datatypes.key_at(index);
	}

	/*
	 * pp_scope::resolve_datatype(name)
	 *
	 * Look up a datatype by name.
	 */
	const pp_datatype *
	resolve_datatype(const string &name) const
	{
		try {
			return m_datatypes[name].get();
		} catch (std::out_of_range &e) {
		}

		if (!is_root()) {
			return m_parent->resolve_datatype(name);
		}

		return NULL;
	}

	/*
	 * pp_scope::add_dirent(name, dirent)
	 *
	 * Add a named dirent to this scope.
	 */
	void
	add_dirent(const string &name, pp_dirent_ptr dirent)
	{
		if (dirent->is_scope()) {
			pp_scope *scope = static_cast<pp_scope*>(dirent.get());
			scope->set_parent(this);
		}
		m_dirents.insert(name, dirent);
	}

	/*
	 * pp_scope::n_dirents()
	 *
	 * Return the number of dirents in this scope.
	 */
	size_t
	n_dirents() const
	{
		return m_dirents.size();
	}

	/*
	 * pp_scope::dirent()
	 *
	 * Provide access to the dirents vector.
	 */
	const pp_dirent *
	dirent(int index) const
	{
		return m_dirents[index].get();
	}
	const pp_dirent *
	dirent(string index) const
	{
		return m_dirents[index].get();
	}

	/*
	 * pp_scope::dirent_name(index)
	 *
	 * Return the name of the indexed dirent.
	 */
	string
	dirent_name(int index) const
	{
		return m_dirents.key_at(index);
	}

    private:
	const pp_scope *m_parent;
	pp_const_binding_ptr m_binding;
	keyed_vector<string, pp_const_dirent_ptr> m_dirents;
	keyed_vector<string, pp_const_datatype_ptr> m_datatypes;
	std::vector<pp_const_datatype_ptr> m_anon_datatypes;
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
