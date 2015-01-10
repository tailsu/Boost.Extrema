#pragma once

#include <boost/mpl/if.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/config.hpp>

namespace boost {
namespace detail {

template <class A, class B, class R>
struct any_is_same
	: mpl::or_<is_same<A, R>, is_same<B, R>>
{};

}

template <typename A, typename B>
struct arithmetic_type_promotion
{
	BOOST_MPL_ASSERT((mpl::and_<is_arithmetic<A>, is_arithmetic<B>>));

	typedef typename mpl::if_<
		detail::any_is_same<A, B, long double>,
		long double,
		typename mpl::if_<

		detail::any_is_same<A, B, double>,
		double,
		typename mpl::if_<

		detail::any_is_same<A, B, float>,
		float,
		typename mpl::if_<

#ifdef BOOST_HAS_LONG_LONG
		detail::any_is_same<A, B, unsigned long long>,
		unsigned long long,
		typename mpl::if_<

		detail::any_is_same<A, B, long long>,
		long long,
		typename mpl::if_<
#endif

		detail::any_is_same<A, B, unsigned long>,
		unsigned long,
		typename mpl::if_<

		detail::any_is_same<A, B, long>,
		long,
		typename mpl::if_<

		detail::any_is_same<A, B, unsigned int>,
		unsigned int,
		int

	>::type
	>::type
#ifdef BOOST_HAS_LONG_LONG
	>::type
	>::type
#endif
	>::type
	>::type
	>::type
	>::type

	type;
};

}
