/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_DEVICE_H__
#define PP_PP_DEVICE_H__

#include "pp.h"
#include "pp_dirent.h"
#include "pp_container.h"
#include "keyed_vector.h"
#include "pp_datatype.h"
#include "pp_space.h"
#include "pp_field.h"

#include <stdexcept>

/*
 * pp_device - a logical device container.
 *
 * Constructors:
 *	()
 *
 * Notes:
 */
class pp_device;
typedef boost::shared_ptr<pp_device> pp_device_ptr;
typedef boost::shared_ptr<const pp_device> pp_const_device_ptr;

class pp_device: public pp_dirent, public pp_container
{
    protected:
	explicit pp_device(pp_dirent_type detype): pp_dirent(detype) {}
    public:
	explicit pp_device(): pp_dirent(PP_DIRENT_DEVICE) {}
	virtual ~pp_device() {}

	/*
	 * pp_device::add_field(name, field)
	 *
	 * Add a named field to this device.
	 */
	void
	add_field(const string &name, pp_field_ptr field)
	{
		m_dirents.insert(name, field);
	}

	/*
	 * pp_device::add_device(name, device)
	 *
	 * Add a named device to this device.
	 */
	void
	add_device(const string &name, pp_device_ptr device)
	{
		pp_container_ptr tmp = shared_from_this();
		device->set_parent(tmp);
		m_dirents.insert(name, device);
	}

	/*
	 * pp_device::add_space(name, space)
	 *
	 * Add a named space to this device.
	 */
	void
	add_space(const string &name, pp_space_ptr space)
	{
		pp_container_ptr tmp = shared_from_this();
		space->set_parent(tmp);
		m_dirents.insert(name, space);
	}
};

inline pp_const_device_ptr
pp_device_from_dirent(pp_const_dirent_ptr dirent)
{
	if (dirent->dirent_type() != PP_DIRENT_DEVICE) {
		throw std::runtime_error("non-device dirent used as device");
	}
	return boost::static_pointer_cast<const pp_device>(dirent);
}

#define new_pp_device(...) pp_device_ptr(new pp_device(__VA_ARGS__))

#endif // PP_PP_DEVICE_H__
