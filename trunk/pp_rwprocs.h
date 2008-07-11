/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_RWPROCS_H__
#define PP_PP_RWPROCS_H__

#include "pp.h"

// a helper for things which delegate read and write operations
struct pp_rwprocs
{
	virtual ~pp_rwprocs() {}
	virtual pp_value read() const = 0;
	virtual void write(const pp_value &value) const = 0;
};
typedef boost::shared_ptr<const pp_rwprocs> pp_rwprocs_ptr;

#endif // PP_PP_RWPROCS_H__
