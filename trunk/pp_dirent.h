/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_DIRENT_H__
#define PP_PP_DIRENT_H__

#include "pp.h"

/*
 * pp_dirent_type - the specific type of a dirent.
 */
typedef enum {
	PP_DIRENT_SCOPE,
	PP_DIRENT_FIELD,
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
	 * pp_dirent::type()
	 *
	 * Get the pp_dirent_type of this object.
	 */
	pp_dirent_type type() const {
		return m_type;
	}

    private:
	pp_dirent_type m_type;
};
typedef boost::shared_ptr<pp_dirent> pp_dirent_ptr;

#endif // PP_PP_DIRENT_H__
