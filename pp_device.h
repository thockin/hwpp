/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_DEVICE_H__
#define PP_PP_DEVICE_H__

#include "pp.h"
#include "pp_container.h"
#include "keyed_vector.h"
#include "pp_datatype.h"
#include "pp_space.h"
#include "pp_field.h"

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

class pp_device: public pp_container {
    public:
	typedef enum {
		DIRENT_DEVICE,
		DIRENT_SPACE,
		DIRENT_FIELD,
	} dirent_type;

    private:
	class dirent {
	    public:
		explicit dirent(pp_device_ptr device)
		    : m_type(DIRENT_DEVICE), m_device(device) {}
		explicit dirent(pp_space_ptr space)
		    : m_type(DIRENT_SPACE), m_space(space) {}
		explicit dirent(pp_field_ptr field)
		    : m_type(DIRENT_FIELD), m_field(field) {}
		~dirent() {}

		dirent_type type() const {
			return m_type;
		}

		bool is_device() const {
			return m_type == DIRENT_DEVICE;
		}
		pp_device_ptr device() const {
			return m_device;
		}

		bool is_space() const {
			return m_type == DIRENT_SPACE;
		}
		pp_space_ptr space() const {
			return m_space;
		}

		bool is_field() const {
			return m_type == DIRENT_FIELD;
		}
		pp_field_ptr field() const {
			return m_field;
		}

	    private:
		dirent_type m_type;
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
	void add_constant(const string &name, const pp_value value) {
		constants.insert(name, value);
	}

	/*
	 * pp_device::add_datatype(name, datatype)
	 *
	 * Add a named datatype to this device.
	 */
	void add_datatype(const string &name,
	    const pp_datatype_ptr &datatype) {
		datatypes.insert(name, datatype);
	}

	/*
	 * pp_device::add_field(name, field)
	 *
	 * Add a named field to this device.
	 */
	void add_field(const string &name, const pp_field_ptr &field) {
		dirents.insert(name, dirent(field));
	}

	/*
	 * pp_device::add_device(name, device)
	 *
	 * Add a named device to this device.
	 */
	void add_device(const string &name, const pp_device_ptr &device) {
		pp_container_ptr tmp = shared_from_this();
		device->set_parent(tmp);
		dirents.insert(name, dirent(device));
	}

	/*
	 * pp_device::add_space(name, space)
	 *
	 * Add a named space to this device.
	 */
	void add_space(const string &name, const pp_space_ptr &space) {
		pp_container_ptr tmp = shared_from_this();
		space->set_parent(tmp);
		dirents.insert(name, dirent(space));
	}

	keyed_vector<string, pp_value> constants;
	keyed_vector<string, pp_datatype_ptr> datatypes;
	keyed_vector<string, dirent> dirents;
};

#endif // PP_PP_DEVICE_H__
