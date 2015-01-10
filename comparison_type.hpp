#pragma once

/*
	TODO:
	* support for ll/ull comparison
	* min/max for template metaprogramming
	* non-const reference support and mixes (how?)
	* ll/ld support with protection
	* EXTREMA
	* update greater arity overloads
	* make it work with enum parameters

	DECISIONS:
	* debate on {1.2}
	* debate on support for compilers without long long
	* protection from char unsignedness uncertainty

	DONE:
	* debate on (long long, unsigned long long) comparison: no problem
	* debate on support for (long long, long double) comparison: use long double, but only if the user wants to
	* proper return type for integral comparisons.
	* debate on support for 16-bit compilers: it doesn't hurt, remains
	* implicit conversions between related classes (upcasting)
	* implicit conversions between unrelated types

#when comparing types: should we cast both types to the same type? when comparing classes this may be undesirable, when comparing arithmetic types this may be desirable.
	when comparing arithmetics - use explicit conversion
	when comparing other comibinations:
		when both ways convesion is possible - explicit conversion if the resultant types are arithmetic
		otherwise implicit

!when comparing ull and a class implicitly convertible to ll, it won't work perfectly
!conversions to class from arithmetic work even when the constructor is marked explicit
! if two types are comparable, we shouldn't perform casts; if not - we should try to do magic
# comparison of primitive pointers and pointers to related classes - for some dubious reason it works. is_convertible is just very good, i guess

when comparing related classes: implicit upcast, no slicing, return type is base&
when comparing two unrelated types with implicit conversion between them:
	if conversions both way are possible: (optional behavior, the user should be able to disable it for safety)
		choose the arithmetic type if the other is convertible to it, return type is the arithmetic
		otherwise fail compilation
	otherwise return type is the conversion result, non-reference to prevent
		possiblity of returning a reference to a temporary object.
		optionally it can still return a reference if the compiler is smart about returning a reference to a temp object (i.e. works ok).

*/

/* AVAILABLE TWEAKS:

	BOOST_EXTREMA_CLASS_CONVERTS_TO_ARITHMETIC_WHEN_AMBIGUOUS
	BOOST_EXTREMA_RETURN_TEMPORARY_OBJECTS_BY_REFERENCE
	BOOST_EXTREMA_INTEGRATE_WITH_STD
	BOOST_EXTREMA_MIN_NAME
	BOOST_EXTREMA_MAX_NAME
	BOOST_EXTREMA_NAMESPACE
	BOOST_EXTREMA_NO_OVERLOADS_OF_GREATER_ARITY
	USE_LONG_DOUBLE_FOR_COMPARISON
	PROMOTE_FP_TYPES_FOR_COMPARISON

*/

/* comparison type selection steps

	if sA == sB {1}
		if exactly one of A and B is unsigned then {1.1}
			promote both to the next signed that is larger than A and B
		else pick then senior one (e.g. int and long - long, signed char and char - signed char) {1.2}
	else if sA < sB {2} // main case, the sA > sB is handled by reversing the arguments
		if sB is signed
			promote A to the same type
		else // sB is unsigned
			promote A and B to the next signed type larger than both

	the case (signed long long, unsigned long long) should be handled in a special overload

*/

#include <boost/mpl/assert.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/bitxor.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/is_float.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>
#include <boost/type_traits/remove_reference.hpp>

namespace boost
{
	template <typename A, typename B>
	struct unsupported_type_pair;

namespace detail {

	template <typename T> struct next_signed {};

#define BOOST_EXTREMA_DEFINE_NEXT_SIGNED(t, next) \
	template <> struct next_signed<t> { typedef next type; };

	BOOST_EXTREMA_DEFINE_NEXT_SIGNED(char, short)
	BOOST_EXTREMA_DEFINE_NEXT_SIGNED(signed char, short)
	BOOST_EXTREMA_DEFINE_NEXT_SIGNED(unsigned char, short)
	BOOST_EXTREMA_DEFINE_NEXT_SIGNED(short, long)
	BOOST_EXTREMA_DEFINE_NEXT_SIGNED(unsigned short, long)
	BOOST_EXTREMA_DEFINE_NEXT_SIGNED(long, long long)
	BOOST_EXTREMA_DEFINE_NEXT_SIGNED(unsigned long, long long)
#undef BOOST_EXTREMA_DEFINE_NEXT_SIGNED

	template <> struct next_signed<int>
	{
		typedef mpl::if_c<
			sizeof(int) == sizeof(long),
			long long,
			long
		>::type type;
	};
	template <> struct next_signed<unsigned int>
		: next_signed<int>
	{};

	template <typename A>
	struct comparison_type_helper
	{
		typedef typename mpl::eval_if<is_unsigned<A>
						, next_signed<A>
						, mpl::identity<A>
					>::type type;
	};

	template <typename A, typename B>
	struct integral_comparison_type
	{
		typedef
			typename mpl::eval_if_c<sizeof(A) == sizeof(B),
				mpl::eval_if<mpl::bitxor_<is_unsigned<A>, is_unsigned<B>>
					, next_signed<A>
					, mpl::identity<A>  //TODO
				>,
				mpl::eval_if_c<(sizeof(A) > sizeof(B))
					, comparison_type_helper<A>
					, comparison_type_helper<B>
				>
			>::type
		type;
	};

	template <bool ok, typename F, typename I>
	struct mixed_arithmetic_comparison_type_impl
	{
		typedef F type;
	};

	template <typename F, typename I>
	struct mixed_arithmetic_comparison_type_impl<false, F, I>
	{
		typedef unsupported_type_pair<F, I> type;
	};

	template <typename F, typename I>
	struct mixed_arithmetic_comparison_type
		: mixed_arithmetic_comparison_type_impl<(sizeof(F) > sizeof(I)), F, I>
	{};

#define BOOST_EXTREMA_DEFINE_ARITHMETIC_MIXED_COMPARISON_TYPE(F, I, T) \
	template <> \
	struct mixed_arithmetic_comparison_type<F, I> \
	{ \
		typedef T type; \
	};

	BOOST_EXTREMA_DEFINE_ARITHMETIC_MIXED_COMPARISON_TYPE(float, int, double)
	BOOST_EXTREMA_DEFINE_ARITHMETIC_MIXED_COMPARISON_TYPE(float, unsigned int, double)
	BOOST_EXTREMA_DEFINE_ARITHMETIC_MIXED_COMPARISON_TYPE(float, long, double)
	BOOST_EXTREMA_DEFINE_ARITHMETIC_MIXED_COMPARISON_TYPE(float, unsigned long, double)
#undef BOOST_EXTREMA_DEFINE_ARITHMETIC_MIXED_COMPARISON_TYPE

#define BOOST_EXTREMA_DEFINE_UNSUPPORTED_EXTREMA_MIXING(F, I) \
	template <> \
	struct mixed_arithmetic_comparison_type<F, I> \
	{ \
		typedef unsupported_type_pair<F, I> type; \
	};

	BOOST_EXTREMA_DEFINE_UNSUPPORTED_EXTREMA_MIXING(long double, long long)
	BOOST_EXTREMA_DEFINE_UNSUPPORTED_EXTREMA_MIXING(long double, unsigned long long)
#undef BOOST_EXTREMA_DEFINE_UNSUPPORTED_EXTREMA_MIXING

	template <typename A, typename B>
	struct arithmetic_comparison_type
	{
		typedef typename 
			mpl::eval_if<mpl::and_<is_float<A>, is_float<B>>,
				mpl::if_c<(sizeof(A) > sizeof(B)), A, B>
				, mpl::eval_if<is_float<A>
					, detail::mixed_arithmetic_comparison_type<A, B>
					, mpl::eval_if<is_float<B>
						, detail::mixed_arithmetic_comparison_type<B, A>
						, detail::integral_comparison_type<A, B>
					>
				>
			>::type
		type;
	};

	template <typename A, typename B>
	struct min_integral_return_type
	{
		// if the larger type is signed, take it, otherwise take the smaller type.
		typedef mpl::if_c<(sizeof(A) < sizeof(B)), B, A> larger_type;
		typedef mpl::if_c<(sizeof(A) < sizeof(B)), A, B> smaller_type;
		typedef typename mpl::eval_if<is_unsigned<typename larger_type::type>, smaller_type, larger_type>::type type;
	};

	template <typename A, typename B>
	struct max_integral_return_type
	{
		// if the types are the same size, take the unsigned one, otherwise take the larger one.
		typedef mpl::if_c<(sizeof(A) < sizeof(B)), B, A> larger_type;
		typedef typename
			mpl::eval_if_c<
				sizeof(A) == sizeof(B)
				, mpl::if_<is_unsigned<A>, A, B>
				, larger_type
			>::type
		type;
	};

	template <typename A, typename B>
	struct check_unique_comparison_path_existence
		: mpl::not_<mpl::and_<is_convertible<A, B>, is_convertible<B, A>>>
	{};

	template <class A, class B>
	struct unrelated_classes_comparison_type
	{
		typedef void type;
	};

	template <class C, typename A>
	struct class_and_arithmetic_comparison_type
	{
	#ifndef BOOST_EXTREMA_CLASS_CONVERTS_TO_ARITHMETIC_WHEN_AMBIGUOUS
		BOOST_MPL_ASSERT((check_unique_comparison_path_existence<C, A>));
	#endif
		typedef typename
			mpl::if_<check_unique_comparison_path_existence<C, A>
				, void
				, A
			>::type
		type;
	};

	template <class A, class B>
	struct unrelated_types_comparison_type
	{
		typedef typename
			mpl::eval_if<
				is_arithmetic<A>
				, class_and_arithmetic_comparison_type<B, A>
				, mpl::eval_if<is_arithmetic<B>
					, class_and_arithmetic_comparison_type<A, B>
					, unrelated_classes_comparison_type<A, B>
				>
			>::type
		type;
	};

	template <class A, class B>
	struct class_or_mixed_comparison_type
	{
		typedef mpl::identity<void> _void;
		typedef typename
			mpl::eval_if<
				is_base_and_derived<A, B>
				, _void
				, typename mpl::if_<is_base_and_derived<B, A>
					, _void
					, unrelated_types_comparison_type<A, B>
				>::type
			>::type
		type;
	};

	template <class A, class B>
	struct unrelated_classes_return_type
	{
		BOOST_MPL_ASSERT((check_unique_comparison_path_existence<A&, B&>));
		typedef typename mpl::if_<is_convertible<A, B>
	#ifdef BOOST_EXTREMA_RETURN_TEMPORARY_OBJECTS_BY_REFERENCE
			, const B&, const A&
	#else
			, B, A
	#endif
		>::type type;
	};

	template <class C, typename A>
	struct class_and_arithmetic_return_type
	{
#ifndef BOOST_EXTREMA_CLASS_CONVERTS_TO_ARITHMETIC_WHEN_AMBIGUOUS
		BOOST_MPL_ASSERT((check_unique_comparison_path_existence<C, A>));
#endif
		typedef typename mpl::if_<is_convertible<C, A>, A, const C&>::type type;
	};

	template <typename A, typename B>
	struct unrelated_types_return_type
	{
		typedef typename
			mpl::eval_if<
				is_arithmetic<A>
				, class_and_arithmetic_return_type<B, A>
				, mpl::eval_if<is_arithmetic<B>
					, class_and_arithmetic_return_type<A, B>
					, unrelated_classes_return_type<A, B>
				>
			>::type
		type;
	};

	template <typename _A, typename _B>
	struct class_or_mixed_return_type
	{
		typedef typename remove_reference<_A>::type A;
		typedef typename remove_reference<_B>::type B;
		typedef typename
			mpl::eval_if<
				mpl::or_<is_same<A, B>, is_base_and_derived<A, B>>
				, mpl::identity<const A&>
				, mpl::eval_if<is_base_and_derived<B, A>
					, mpl::identity<const B&>
					, unrelated_types_return_type<A, B>
				>
			>::type
		type;
	};
}

template <typename A, typename B>
struct comparison_type
{
	typedef typename
		mpl::eval_if<
			mpl::not_<mpl::or_<is_convertible<A, B>, is_convertible<B, A>>>
			, unsupported_type_pair<A, B>
			, mpl::eval_if<
				mpl::and_<is_arithmetic<A>, is_arithmetic<B>>
				, detail::arithmetic_comparison_type<A, B>
				, detail::class_or_mixed_comparison_type<A, B>
			>
		>::type
	type;
};

template <typename A, typename B>
struct min_return_type
{
	typedef typename
		mpl::eval_if<
			is_same<typename comparison_type<A, B>::type, void>
			, unsupported_type_pair<A, B>
			, mpl::eval_if<
				mpl::and_<is_integral<A>, is_integral<B>>
				, detail::min_integral_return_type<A, B>
				, detail::class_or_mixed_return_type<A, B>
			>
		>::type
	type;
};

template <typename A, typename B>
struct max_return_type
{
	typedef typename
		mpl::eval_if<
			is_same<typename comparison_type<A, B>::type, void>
			, unsupported_type_pair<A, B>
			, mpl::eval_if<
				mpl::and_<is_integral<A>, is_integral<B>>
				, detail::max_integral_return_type<A, B>
				, detail::class_or_mixed_return_type<A, B>
			>
		>::type
	type;
};

}
