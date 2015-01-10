// UltimateEXTREMA.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define BOOST_EXTREMA_MAX_ARITY 3

//#define BOOST_EXTREMA_INTEGRATE_WITH_STD
//#define BOOST_EXTREMA_CLASS_CONVERTS_TO_ARITHMETIC_WHEN_AMBIGUOUS
#define BOOST_EXTREMA_RETURN_TEMPORARY_OBJECTS_BY_REFERENCE
//#define BOOST_EXTREMA_NO_OVERLOADS_OF_GREATER_ARITY

#include "extrema.hpp"
#include <typeinfo>
#include <boost/mpl/assert.hpp>

template <class T>
void type_discovery()
{
	cout << typeid(T).name() << endl;
}

struct Base
{
	bool operator < (const Base&) const { return true; }
};

struct Derived : public Base
{};

struct ToInt
{
	ToInt(int) {}
	//operator int () const { return 5; }

	bool operator < (const ToInt&) const { return false; }
};
using namespace boost;

int _tmain(int argc, _TCHAR* argv[])
{

	//typedef comparison_type<float, long>::type a;
	//type_discovery<a>();

	//enum T { a, b, c};
	//minimum(5, a);
	
	//minimum(1, 2);
	//maximum(char(1), short(2));
	//minimum(size_t(5), 6U);

	//minimum(Base(), Derived());
	//cout << is_same<Base, Base>::value << endl;

	//type_discovery<min_return_type<Base, Base>::type>();
	//type_discovery<min_return_type<Base, Derived>::type>();
	//type_discovery<min_return_type<ToInt, int>::type>();
	//type_discovery<min_return_type<int, ToInt>::type>();
//	minimum(ToInt(3), 3);

	Base base; Derived der;
//	minimum(base, der);
//	minimum((char*)0, (int*)0);
	//type_discovery<min_return_type<char*, void*>::type>();
	//cout << is_convertible<int*, char*>::value << endl;

	//maximum(int(2), 2, 1.0);
	//max(2, 1, 3, 4);
	//minimum((Base&)base, (Derived&)der);
	//max(long long(3), unsigned long long(4));

	//cout << a << b << c << d << x << endl;


	// min integral comparison types
	BOOST_MPL_ASSERT((is_same<min_return_type<char, int>::type, int>));
	BOOST_MPL_ASSERT((is_same<min_return_type<char, unsigned int>::type, char>));

	return 0;
}

void UnitTests()
{
	BOOST_MPL_ASSERT((is_same<comparison_type<int, int>::type, int>));
	BOOST_MPL_ASSERT((is_same<comparison_type<unsigned int, int>::type, long long>));
	BOOST_MPL_ASSERT((is_same<comparison_type<unsigned short, int>::type, int>));
	BOOST_MPL_ASSERT((is_same<comparison_type<int, unsigned short>::type, int>));
	BOOST_MPL_ASSERT((is_same<comparison_type<double, int>::type, double>));
	BOOST_MPL_ASSERT((is_same<comparison_type<int, double>::type, double>));
	BOOST_MPL_ASSERT((is_same<comparison_type<int, float>::type, double>));
	BOOST_MPL_ASSERT((is_same<comparison_type<long, float>::type, double>));
}