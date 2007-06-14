/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_DEVICE_H__
#define PP_PP_DEVICE_H__

#include "pp.h"
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

class pp_device: public pp_container
{
    public:
	/*
	 * pp_device::dirent
	 */
	class dirent
	{
	    public:
		explicit dirent(pp_device_ptr device)
		    : m_type(DIRENT_DEVICE), m_device(device) {}
		explicit dirent(pp_space_ptr space)
		    : m_type(DIRENT_SPACE), m_space(space) {}
		explicit dirent(pp_field_ptr field)
		    : m_type(DIRENT_FIELD), m_field(field) {}
		~dirent() {}

		bool
		is_device() const
		{
			return m_type == DIRENT_DEVICE;
		}
		pp_device_ptr
		as_device() const
		{
			if (!is_device()) {
				throw std::runtime_error(
				    "non-device dirent used as device");
			}
			return m_device;
		}

		bool
		is_space() const
		{
			return m_type == DIRENT_SPACE;
		}
		pp_space_ptr
		as_space() const
		{
			if (!is_space()) {
				throw std::runtime_error(
				    "non-space dirent used as space");
			}
			return m_space;
		}

		bool
		is_field() const
		{
			return m_type == DIRENT_FIELD;
		}
		pp_field_ptr
		as_field() const
		{
			if (!is_field()) {
				throw std::runtime_error(
				    "non-field dirent used as field");
			}
			return m_field;
		}

	    private:
		enum {
			DIRENT_DEVICE,
			DIRENT_SPACE,
			DIRENT_FIELD,
		} m_type;
		pp_device_ptr m_device;
		pp_space_ptr m_space;
		pp_field_ptr m_field;
	};

    public:
	explicit pp_device() {}
	virtual ~pp_device() {}

	//FIXME: access methods for the raw vectors to be read-only?

	/*
	 * pp_device::add_constant(name, value)
	 *
	 * Add a named constant to this device.
	 */
	//FIXME: do we use constants at all?
	void
	add_constant(const string &name, const pp_value value)
	{
		constants.insert(name, value);
	}

	/*
	 * pp_device::add_datatype(name, datatype)
	 *
	 * Add a named datatype to this device.
	 */
	void
	add_datatype(const string &name,
	    const pp_datatype_ptr &datatype)
	{
		datatypes.insert(name, datatype);
	}

	/*
	 * pp_device::add_field(name, field)
	 *
	 * Add a named field to this device.
	 */
	void
	add_field(const string &name, const pp_field_ptr &field)
	{
		dirents.insert(name, dirent(field));
	}

	/*
	 * pp_device::add_device(name, device)
	 *
	 * Add a named device to this device.
	 */
	void
	add_device(const string &name, const pp_device_ptr &device)
	{
		pp_container_ptr tmp = shared_from_this();
		device->set_parent(tmp);
		dirents.insert(name, dirent(device));
	}

	/*
	 * pp_device::add_space(name, space)
	 *
	 * Add a named space to this device.
	 */
	void
	add_space(const string &name, const pp_space_ptr &space)
	{
		pp_container_ptr tmp = shared_from_this();
		space->set_parent(tmp);
		dirents.insert(name, dirent(space));
	}

	keyed_vector<string, pp_value> constants;
	keyed_vector<string, pp_datatype_ptr> datatypes;
	keyed_vector<string, dirent> dirents;
};

#define new_pp_device(...) pp_device_ptr(new pp_device(__VA_ARGS__))

#endif // PP_PP_DEVICE_H__
