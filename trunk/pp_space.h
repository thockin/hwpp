/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_SPACE_H__
#define PP_PP_SPACE_H__

#include "pp.h"
#include "pp_scope.h"
#include "pp_binding.h"

/*
 * pp_space - a bound register space.
 *
 * Constructors:
 *	()
 *
 * Notes:
 */
class pp_space: public pp_scope {
    public:
	explicit pp_space(pp_binding_ptr binding): m_binding(binding) {}
	virtual ~pp_space() {}

	/*
	 * pp_space::binding()
	 *
	 * Get the binding of this space.
	 */
	const pp_binding_ptr binding() const { return m_binding; }

    private:
	pp_binding_ptr m_binding;
};
typedef boost::shared_ptr<pp_space> pp_space_ptr;

#endif // PP_PP_SPACE_H__
