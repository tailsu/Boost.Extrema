// UltimateMinmax.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/preprocessor/arithmetic/dec.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/paren.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>

#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/utility/enable_if.hpp>

#include "arithmetic_type_promotion.hpp"

#define MAX_MINMAX_ARITY 3

namespace std {

#define BASE_FUNCTION(func, op) \
	template <typename A, typename B> \
	typename boost::enable_if<boost::mpl::and_<boost::is_arithmetic<A>, boost::is_arithmetic<B>, boost::mpl::not_<boost::is_same<A, B>>> \
		, typename boost::arithmetic_type_promotion<A, B>::type>::type \
	func(A a, B b) \
	{ \
		typedef typename boost::arithmetic_type_promotion<A, B>::type R; \
		return R(a) op R(b) ? R(a) : R(b); \
	}

BASE_FUNCTION(min, <)
BASE_FUNCTION(max, >)

#define FUNCTION_CALL_PREFIX(z, n, data) data(BOOST_PP_CAT(t, n),
#define FUNCTION_CALL_SUFFIX(z, n, data) BOOST_PP_RPAREN()
#define ENABLE_IF_CONDITION_PREFIX(z, n, data) boost::mpl::and_<boost::is_arithmetic<BOOST_PP_CAT(T, n)>,
#define ENABLE_IF_CONDITION_SUFFIX(z, n, data) >
#define TYPE_PROMOTION_PREFIX(z, n, data) typename boost::arithmetic_type_promotion<BOOST_PP_CAT(T, n),
#define TYPE_PROMOTION_SUFFIX(z, n, data) >::type

#define PREFIX_SUFFIX(n, data, prefix, middle, suffix) \
	BOOST_PP_REPEAT(n, prefix, data) \
	middle \
	BOOST_PP_REPEAT(n, suffix, data)

#define FUNCTION_TEMPLATE(z, n, function_name) \
template <BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), typename T)> \
typename boost::enable_if<PREFIX_SUFFIX(n, ~, ENABLE_IF_CONDITION_PREFIX, boost::is_integral<BOOST_PP_CAT(T, n)>, ENABLE_IF_CONDITION_SUFFIX) \
	, PREFIX_SUFFIX(n, ~, TYPE_PROMOTION_PREFIX, BOOST_PP_CAT(T, n), TYPE_PROMOTION_SUFFIX) \
>::type \
function_name(BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_INC(n), T, t)) \
{ \
	return PREFIX_SUFFIX(n, function_name, FUNCTION_CALL_PREFIX, BOOST_PP_CAT(t, n), FUNCTION_CALL_SUFFIX) \
	; \
}

#define FUNCTION_TEMPLATE_DEC_N(z, n, data) FUNCTION_TEMPLATE(z, BOOST_PP_DEC(n), data)

BOOST_PP_REPEAT_FROM_TO(3, BOOST_PP_INC(MAX_MINMAX_ARITY), FUNCTION_TEMPLATE_DEC_N, min)
BOOST_PP_REPEAT_FROM_TO(3, BOOST_PP_INC(MAX_MINMAX_ARITY), FUNCTION_TEMPLATE_DEC_N, max)

}

#include <typeinfo>

template <class T>
void type_discovery(T t)
{
	cout << typeid(T).name() << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int a = max(char(1), short(2));
	int x = max(5, 6);
	double b = max(int(2), 3.0, 1);
	int c = max(2, 1, 3, 4);
	int d = max(2, 3, 4, 1, 5);

	cout << a << b << c << d << endl;

	return 0;
}
