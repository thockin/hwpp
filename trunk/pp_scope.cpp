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
 * pp_scope::parent()
 *
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
 * pp_scope::set_parent()
 *
 * Set the parent scope of this object.
 */
void
pp_scope::set_parent(const pp_scope *parent)
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
pp_scope::is_root() const
{
	return (m_parent == NULL);
}

/*
 * pp_scope::binding()
 *
 * Get the binding of this scope.
 */
const pp_binding *
pp_scope::binding() const
{
	return m_binding.get();
}

/*
 * pp_scope::add_datatype(name, datatype)
 *
 * Add a named or unnamed datatype to this scope.
 */
void
pp_scope::add_datatype(const string &name, pp_const_datatype_ptr datatype)
{
	m_datatypes.insert(name, datatype);
}
void
pp_scope::add_datatype(pp_const_datatype_ptr datatype)
{
	m_anon_datatypes.push_back(datatype);
}

/*
 * pp_scope::n_datatypes()
 *
 * Return the number of named datatypes in this scope.
 */
size_t
pp_scope::n_datatypes() const
{
	return m_datatypes.size();
}

/*
 * pp_scope::datatype()
 *
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
 * pp_scope::datatype_name(index)
 *
 * Return the name of the indexed datatype.
 */
string
pp_scope::datatype_name(int index) const
{
	return m_datatypes.key_at(index);
}

/*
 * pp_scope::resolve_datatype(name)
 *
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
 * pp_scope::add_dirent(name, dirent)
 *
 * Add a named dirent to this scope.
 */
void
pp_scope::add_dirent(const string &name, pp_dirent_ptr dirent)
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
pp_scope::n_dirents() const
{
	return m_dirents.size();
}

/*
 * pp_scope::dirent()
 *
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
 * pp_scope::dirent_name(index)
 *
 * Return the name of the indexed dirent.
 */
string
pp_scope::dirent_name(int index) const
{
	return m_dirents.key_at(index);
}
