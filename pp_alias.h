/* Copyright (c) Tim Hockin, 2008 */
#ifndef PP_PP_ALIAS_H__
#define PP_PP_ALIAS_H__

#include "pp.h"
#include "pp_dirent.h"
#include "pp_path.h"

/*
 * pp_alias - a symbolic link to a dirent.
 */
class pp_alias: public pp_dirent
{
    public:
	explicit pp_alias(const pp_path &path)
	    : pp_dirent(PP_DIRENT_ALIAS), m_path(path)
	{
	}

	const pp_path &
	link_path() const
	{
		return m_path;
	}

    private:
	pp_path m_path;
};
typedef boost::shared_ptr<pp_alias> pp_alias_ptr;
typedef boost::shared_ptr<const pp_alias> pp_alias_const_ptr;

#define new_pp_alias(...) pp_alias_ptr(new pp_alias(__VA_ARGS__))

// const
inline pp_alias_const_ptr
pp_alias_from_dirent(const pp_dirent_const_ptr &dirent)
{
	if (dirent->is_alias()) {
		return static_pointer_cast<const pp_alias>(dirent);
	}
	throw pp_dirent::conversion_error("non-alias dirent used as alias");
}
// non-const
inline pp_alias_ptr
pp_alias_from_dirent(const pp_dirent_ptr &dirent)
{
	const pp_dirent_const_ptr &const_dirent = dirent;
	return const_pointer_cast<pp_alias>(
	       pp_alias_from_dirent(const_dirent));
}

#endif // PP_PP_ALIAS_H__
