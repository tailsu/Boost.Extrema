#pragma once

#ifndef BOOST_EXTREMA_MAX_ARITY
#	define BOOST_EXTREMA_MAX_ARITY 10
#endif

#ifdef BOOST_EXTREMA_INTEGRATE_WITH_STD
#	if defined(BOOST_EXTREMA_MIN_NAME) || defined(BOOST_EXTREMA_MAX_NAME) || defined(BOOST_EXTREMA_NAMESPACE)
#		error When integrating with the std namespace, don't define a custom namespace or custom names for the min and max functions.
#	endif
#	define BOOST_EXTREMA_MAX_NAME max
#	define BOOST_EXTREMA_MIN_NAME min
#	define BOOST_EXTREMA_NAMESPACE std
#else
#	ifndef BOOST_EXTREMA_MIN_NAME
#		define BOOST_EXTREMA_MIN_NAME minimum
#	endif
#	ifndef BOOST_EXTREMA_MAX_NAME
#		define BOOST_EXTREMA_MAX_NAME maximum
#	endif
#	ifndef BOOST_EXTREMA_NAMESPACE
#		define BOOST_EXTREMA_NAMESPACE boost
#	endif
#endif

#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/identity.hpp>
#include <boost/preprocessor/punctuation/paren.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/seq/elem.hpp>

#include <boost/mpl/and.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/is_void.hpp>

#include "comparison_type.hpp"

#ifdef BOOST_EXTREMA_INTEGRATE_WITH_STD
#	include <boost/mpl/not.hpp>
#	include <boost/type_traits/is_same.hpp>
#	include <boost/utility/enable_if.hpp>
#endif

namespace BOOST_EXTREMA_NAMESPACE {

#ifdef BOOST_EXTREMA_INTEGRATE_WITH_STD
#	define BOOST_EXTREMA_RETURN_TYPE_OF_BINARY_OVERLOAD(function_name) \
	typename boost::lazy_enable_if<boost::mpl::not_<boost::is_same<A, B>> \
		, boost::BOOST_PP_CAT(function_name, _return_type)<A, B>>::type
#else
#	define BOOST_EXTREMA_RETURN_TYPE_OF_BINARY_OVERLOAD(function_name) \
		typename boost::BOOST_PP_CAT(function_name, _return_type)<A, B>::type
#endif


#define BOOST_EXTREMA_BASE_FUNCTION_IMPL(function_name, op, semantic, cast_type, caster) \
	template <typename A, typename B> \
	BOOST_EXTREMA_RETURN_TYPE_OF_BINARY_OVERLOAD(semantic) \
	BOOST_PP_CAT(function_name, _impl)(const A& a, const B& b, boost::cast_type /*dont_cast_comparends*/) \
	{ \
		typedef typename boost::comparison_type<A, B>::type C; \
		typedef BOOST_EXTREMA_RETURN_TYPE_OF_BINARY_OVERLOAD(semantic) R; \
		if (caster(a) op caster(b)) \
			return (R)a; \
		else return (R)b; \
	}

#define BOOST_EXTREMA_CASTER(x) C(x)

#define BOOST_EXTREMA_BASE_FUNCTION(function_name, op, semantic) \
	BOOST_EXTREMA_BASE_FUNCTION_IMPL(function_name, op, semantic, false_type, C) \
	BOOST_EXTREMA_BASE_FUNCTION_IMPL(function_name, op, semantic, true_type, ) \
	template <typename A, typename B> \
	BOOST_EXTREMA_RETURN_TYPE_OF_BINARY_OVERLOAD(semantic) \
	function_name(const A& a, const B& b) \
	{ \
		typedef typename boost::comparison_type<A, B>::type C; \
		return BOOST_PP_CAT(function_name, _impl)(a, b, boost::is_void<C>()); \
	}

BOOST_EXTREMA_BASE_FUNCTION(BOOST_EXTREMA_MIN_NAME, <, min)
BOOST_EXTREMA_BASE_FUNCTION(BOOST_EXTREMA_MAX_NAME, >, max)

#ifndef BOOST_EXTREMA_NO_OVERLOADS_OF_GREATER_ARITY

#define BOOST_EXTREMA_FUNCTION_CALL_PREFIX(z, n, data) data(BOOST_PP_CAT(t, n),
#define BOOST_EXTREMA_FUNCTION_CALL_SUFFIX(z, n, data) BOOST_PP_RPAREN()
#define BOOST_EXTREMA_COMPOUND_RESULT_PREFIX(z, n, data) typename boost::BOOST_PP_CAT(data, _return_type)<BOOST_PP_CAT(T, n),
#define BOOST_EXTREMA_COMPOUND_RESULT_SUFFIX(z, n, data) >::type

#define BOOST_EXTREMA_PREFIX_AND_SUFFIX(n, data, prefix, middle, suffix) \
	BOOST_PP_REPEAT(n, prefix, data) \
	middle \
	BOOST_PP_REPEAT(n, suffix, data)

#define BOOST_EXTREMA_FUNCTION_TEMPLATE(z, n, data) \
template <BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), typename T)> \
BOOST_EXTREMA_PREFIX_AND_SUFFIX(n, BOOST_PP_SEQ_ELEM(1, data), BOOST_EXTREMA_COMPOUND_RESULT_PREFIX, BOOST_PP_CAT(T, n), BOOST_EXTREMA_COMPOUND_RESULT_SUFFIX) \
BOOST_PP_SEQ_ELEM(0, data)(BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_INC(n), T, t)) \
{ \
	return BOOST_EXTREMA_PREFIX_AND_SUFFIX(n, BOOST_PP_SEQ_ELEM(0, data), BOOST_EXTREMA_FUNCTION_CALL_PREFIX, BOOST_PP_CAT(t, n), BOOST_EXTREMA_FUNCTION_CALL_SUFFIX); \
}

#define BOOST_EXTREMA_FUNCTION_TEMPLATE_DEC_N(z, n, data) BOOST_EXTREMA_FUNCTION_TEMPLATE(z, BOOST_PP_DEC(n), data)

BOOST_PP_REPEAT_FROM_TO(3, BOOST_PP_INC(BOOST_EXTREMA_MAX_ARITY), BOOST_EXTREMA_FUNCTION_TEMPLATE_DEC_N, (BOOST_EXTREMA_MIN_NAME)(min))
BOOST_PP_REPEAT_FROM_TO(3, BOOST_PP_INC(BOOST_EXTREMA_MAX_ARITY), BOOST_EXTREMA_FUNCTION_TEMPLATE_DEC_N, (BOOST_EXTREMA_MAX_NAME)(max))

#undef BOOST_EXTREMA_RETURN_TYPE_OF_BINARY_OVERLOAD
#undef BOOST_EXTREMA_BASE_FUNCTION_IMPL
#undef BOOST_EXTREMA_BASE_FUNCTION
#undef BOOST_EXTREMA_FUNCTION_CALL_PREFIX
#undef BOOST_EXTREMA_FUNCTION_CALL_SUFFIX
#undef BOOST_EXTREMA_TYPE_PROMOTION_PREFIX
#undef BOOST_EXTREMA_TYPE_PROMOTION_SUFFIX
#undef BOOST_EXTREMA_PREFIX_AND_SUFFIX
#undef BOOST_EXTREMA_FUNCTION_TEMPLATE
#undef BOOST_EXTREMA_FUNCTION_TEMPLATE_DEC_N

#endif

}
