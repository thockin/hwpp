/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_RWPROCS_H__
#define PP_RWPROCS_H__

#include "pp.h"

namespace pp {

// a helper for things which delegate read and write operations
struct RwProcs
{
	virtual ~RwProcs() {}
	virtual Value read() const = 0;
	virtual void write(const Value &value) const = 0;
};
typedef boost::shared_ptr<const RwProcs> RwProcsPtr;

}  // namespace pp

#endif // PP_RWPROCS_H__
