/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_SPACE_H__
#define PP_PP_SPACE_H__

#include "pp.h"
#include "pp_scope.h"
#include "pp_binding.h"

#include <stdexcept>

/*
 * pp_space - a bound register space.
 *
 * Constructors:
 *	()
 *
 * Notes:
 */
class pp_space: public pp_scope
{
    public:
	explicit pp_space(pp_const_binding_ptr binding)
	    : pp_scope(PP_DIRENT_SPACE), m_binding(binding) {}
	virtual ~pp_space() {}

	/*
	 * pp_space::binding()
	 *
	 * Get the binding of this space.
	 */
	const pp_binding *
	binding() const
	{
		return m_binding.get();
	}

    private:
	pp_const_binding_ptr m_binding;
};
typedef boost::shared_ptr<pp_space> pp_space_ptr;
typedef boost::shared_ptr<const pp_space> pp_const_space_ptr;

inline const pp_space *
pp_space_from_dirent(pp_const_dirent_ptr dirent)
{
	if (dirent->dirent_type() != PP_DIRENT_SPACE) {
		throw std::runtime_error("non-space dirent used as space");
	}
	return static_cast<const pp_space *>(dirent.get());
}

#define new_pp_space(...) pp_space_ptr(new pp_space(__VA_ARGS__))

#endif // PP_PP_SPACE_H__
