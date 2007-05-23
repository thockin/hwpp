/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_CONTAINER_H__
#define PP_PP_CONTAINER_H__

#include "pp.h"
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

class pp_container: public boost::enable_shared_from_this<pp_container> {
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
	const pp_container_ptr parent() const {
		pp_container *p = const_cast<pp_container *>(this);
		return p->parent();
	}
	pp_container_ptr parent() {
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
	void set_parent(const pp_container_ptr &parent) {
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
	bool is_root() const {
		return (m_parent.get() == NULL);
	}

    protected:
	pp_container_ptr m_parent;
};

#endif // PP_PP_CONTAINER_H__
