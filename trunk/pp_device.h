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
		device->set_parent(this);
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
		space->set_parent(this);
		m_dirents.insert(name, space);
	}
};

inline const pp_device *
pp_device_from_dirent(pp_const_dirent_ptr dirent)
{
	if (dirent->dirent_type() != PP_DIRENT_DEVICE) {
		throw std::runtime_error("non-device dirent used as device");
	}
	return static_cast<const pp_device *>(dirent.get());
}

#define new_pp_device(...) pp_device_ptr(new pp_device(__VA_ARGS__))

#endif // PP_PP_DEVICE_H__
