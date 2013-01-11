/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_RWPROCS_H__
#define HWPP_RWPROCS_H__

#include "hwpp.h"

namespace hwpp {

// a helper for things which delegate read and write operations
struct RwProcs
{
	virtual ~RwProcs() {}
	virtual Value read() const = 0;
	virtual void write(const Value &value) const = 0;
};
typedef boost::shared_ptr<const RwProcs> RwProcsPtr;

}  // namespace hwpp

#endif // HWPP_RWPROCS_H__
