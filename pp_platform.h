// Copyright 2007 Google Inc. All Rights Reserved.
// Author: lesleyn@google.com (Lesley Northam)

#ifndef PP_PLATFORM_HPP__
#define PP_PLATFORM_HPP__

#include "pp.h"
#include "pp_device.h"

/*
 * pp_platform - a platform with devices and spaces, etc.
 *
 * Constructors:
 *      ()
 * Notes:
 */
class pp_platform: public pp_device
{
    public:
	explicit pp_platform() {};
	virtual ~pp_platform() {};
};
typedef boost::shared_ptr<pp_platform> pp_platform_ptr;

#define new_pp_platform(...) pp_platform_ptr(new pp_platform(__VA_ARGS__))

#endif // PP_PLATFORM_HPP__

