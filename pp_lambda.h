/* Copyright (c) Tim Hockin, 2008 */
#ifndef PP_PP_LAMBDA_H__
#define PP_PP_LAMBDA_H__

#include <boost/lambda/lambda.hpp>

using boost::lambda::_1;
using boost::lambda::_2;
using boost::lambda::_3;

//
// Define some specializations for boost::lamba.  It should 'Just Work'.
//
namespace boost {
namespace lambda {

template<class Taction>
struct plain_return_type_2<arithmetic_action<Taction>, bignum, bignum> {
	typedef bignum type;
};
template<typename Taction, typename Tother>
struct plain_return_type_2<arithmetic_action<Taction>, bignum, Tother> {
	typedef bignum type;
};
template<typename Taction, typename Tother>
struct plain_return_type_2<arithmetic_action<Taction>, Tother, bignum> {
	typedef bignum type;
};

} // namespace lambda
} // namespace boost

#endif // PP_PP_LAMBDA_H__
