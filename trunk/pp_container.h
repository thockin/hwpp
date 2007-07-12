/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_CONTAINER_H__
#define PP_PP_CONTAINER_H__

#include "pp.h"
#include "keyed_vector.h"
#include "pp_datatype.h"
#include <boost/enable_shared_from_this.hpp>

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

class pp_container: public boost::enable_shared_from_this<pp_container>
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
	pp_const_container_ptr
	parent() const
	{
		if (is_root()) {
			return shared_from_this();
		}
		return m_parent;
	}

	/*
	 * pp_container::set_parent()
	 *
	 * Set the parent container of this object.
	 */
	void
	set_parent(const pp_container_ptr &parent)
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
		return (m_parent.get() == NULL);
	}

	/*
	 * pp_container::datatypes()
	 *
	 * Provide raw access to the datatypes vector.
	 */
	const keyed_vector<string, pp_const_datatype_ptr> &
	datatypes()
	{
		return m_datatypes;
	}

	/*
	 * pp_container::add_datatype(name, datatype)
	 *
	 * Add a named datatype to this container.
	 */
	void
	add_datatype(const string &name, const pp_const_datatype_ptr &datatype)
	{
		m_datatypes.insert(name, datatype);
	}

	/*
	 * pp_container::resolve_datatype(name)
	 *
	 * Look up a datatype by name.
	 */
	pp_const_datatype_ptr
	resolve_datatype(const string &name)
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

    protected:
	pp_container_ptr m_parent;
	keyed_vector<string, pp_const_datatype_ptr> m_datatypes;
};

#define new_pp_container(...) pp_container_ptr(new pp_container(__VA_ARGS__))

#endif // PP_PP_CONTAINER_H__
