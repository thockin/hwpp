/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_TEST_H__
#define PP_TEST_H__

#include <iostream>

#define PP_TEST_ERROR(msg) \
	cerr << "ERROR: [" __FILE__ << ":" << __LINE__ << "] " << msg << endl

#endif // PP_TEST_H__
