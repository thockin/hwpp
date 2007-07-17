/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_DIRENT_H__
#define PP_PP_DIRENT_H__

#include "pp.h"

/*
 * pp_dirent_type - the specific type of a dirent.
 */
typedef enum {
	PP_DIRENT_REGISTER,
	PP_DIRENT_FIELD,
	PP_DIRENT_SCOPE,
	PP_DIRENT_SPACE,
	PP_DIRENT_DEVICE,
} pp_dirent_type;

/*
 * pp_dirent - a directory entry in a container
 *
 * Constructors:
 * 	(const pp_dirent_type type)
 *
 * Notes:
 */
class pp_dirent {
    public:
	explicit pp_dirent(pp_dirent_type type): m_type(type) {}
	virtual ~pp_dirent() {}

	/*
	 * pp_dirent::dirent_type()
	 *
	 * Get the pp_dirent_type of this object.
	 */
	pp_dirent_type
	dirent_type() const
	{
		return m_type;
	}

	/*
	 * pp_dirent::is_*()
	 *
	 * Check the type of this object.
	 */
	bool
	is_register() const
	{
		return m_type == PP_DIRENT_REGISTER;
	}
	bool
	is_field() const
	{
		return m_type == PP_DIRENT_FIELD;
	}
	bool
	is_scope() const
	{
		return m_type == PP_DIRENT_SCOPE;
	}
	bool
	is_space() const
	{
		return m_type == PP_DIRENT_SPACE;
	}
	bool
	is_device() const
	{
		return m_type == PP_DIRENT_DEVICE;
	}

    private:
	pp_dirent_type m_type;
};
typedef boost::shared_ptr<pp_dirent> pp_dirent_ptr;
typedef boost::shared_ptr<const pp_dirent> pp_const_dirent_ptr;

#define new_pp_dirent(...) pp_dirent_ptr(new pp_dirent(__VA_ARGS__))

#endif // PP_PP_DIRENT_H__
