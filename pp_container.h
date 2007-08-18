/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_CONTAINER_H__
#define PP_PP_CONTAINER_H__

#include "pp.h"
#include "keyed_vector.h"
#include "pp_datatype.h"
#include "pp_dirent.h"

/*
 * pp_container - base class for all things that contain things.
 *
 * Constructors:
 * 	()
 *
 * Notes:
 */
class pp_container;
typedef boost::shared_ptr<pp_container> pp_container_ptr;
typedef boost::shared_ptr<const pp_container> pp_const_container_ptr;

class pp_container
{
    public:
	explicit pp_container() {}
	virtual ~pp_container() {}

	/*
	 * pp_container::parent()
	 *
	 * Get a pointer to the parent container of this object.  If this
	 * container is the top of the hierarchy, this method returns a
	 * pointer to this object.
	 */
	const pp_container *
	parent() const
	{
		if (is_root()) {
			return this;
		}
		return m_parent;
	}

	/*
	 * pp_container::set_parent()
	 *
	 * Set the parent container of this object.
	 */
	void
	set_parent(const pp_container *parent)
	{
		m_parent = parent;
	}

	/*
	 * pp_container::is_root()
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
	 * pp_container::add_datatype(name, datatype)
	 *
	 * Add a named datatype to this container.
	 */
	void
	add_datatype(const string &name, pp_const_datatype_ptr datatype)
	{
		m_datatypes.insert(name, datatype);
	}

	/*
	 * pp_container::n_datatypes()
	 *
	 * Return the number of datatypes in this container.
	 */
	size_t
	n_datatypes() const
	{
		return m_datatypes.size();
	}

	/*
	 * pp_container::datatype()
	 *
	 * Provide access to the datatypes vector.
	 */
	pp_const_datatype_ptr
	datatype(int index) const
	{
		return m_datatypes[index];
	}
	pp_const_datatype_ptr
	datatype(string index) const
	{
		return m_datatypes[index];
	}

	/*
	 * pp_container::datatype_name(index)
	 *
	 * Return the name of the indexed datatype.
	 */
	string
	datatype_name(int index) const
	{
		return m_datatypes.key_at(index);
	}

	/*
	 * pp_container::resolve_datatype(name)
	 *
	 * Look up a datatype by name.
	 */
	pp_const_datatype_ptr
	resolve_datatype(const string &name) const
	{
		try {
			return m_datatypes[name];
		} catch (std::out_of_range) {
		}

		if (!is_root()) {
			return m_parent->resolve_datatype(name);
		}

		return pp_const_datatype_ptr();
	}

	/*
	 * pp_container::n_dirents()
	 *
	 * Return the number of dirents in this container.
	 */
	size_t
	n_dirents() const
	{
		return m_dirents.size();
	}

	/*
	 * pp_container::dirent()
	 *
	 * Provide access to the dirents vector.
	 */
	pp_const_dirent_ptr
	dirent(int index) const
	{
		return m_dirents[index];
	}
	pp_const_dirent_ptr
	dirent(string index) const
	{
		return m_dirents[index];
	}

	/*
	 * pp_container::dirent_name(index)
	 *
	 * Return the name of the indexed dirent.
	 */
	string
	dirent_name(int index) const
	{
		return m_dirents.key_at(index);
	}

    protected:
	const pp_container *m_parent;
	keyed_vector<string, pp_const_dirent_ptr> m_dirents;
	keyed_vector<string, pp_const_datatype_ptr> m_datatypes;
};

#define new_pp_container(...) pp_container_ptr(new pp_container(__VA_ARGS__))

#endif // PP_PP_CONTAINER_H__
