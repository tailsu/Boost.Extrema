Boost.Extrema
=============

How it works
------------
When creating a min/max function two things about it need to be considered

- How will the comparison be performed?
- What is the return type of the function?

###Performing the comparison
The standard implementation of a min/max function uses the relational operators to perform the comparison. This poses several problems.

The first is related to comparing arithmetic types which differ in their domain. For example, if you compare a `long` and an `unsigned long`, the compiler may properly promote them to `long long` and perform the comparison, or it may issue a signed/unsigned mismatch warning and compile whatever it pleases, e.g. comparing them as if they are both `unsigned long`s and leaving you to hope for the best or manually cast the comparends. The same problem arises if you try to compare a `long long` and a `float` - the compiler will just "promote" the integer type to the floating point type and then perform the comparison, which may yield incorrect results.

The second is related to the way implicit conversions between unrelated types work. If a class can be implicitly constructed from an `int` and it's also implicitly convertible, but not comparable to `int`, then an ambiguity arises when trying to compare it to an `int`. In certain situations the ambiguity can be resolved either way, but it's up to the developer to make this choice.
####`comparison_type`

To solve both problems, the library provides the meta-function `comparison_type` that, given two types, returns the best type (i.e. that which contains the domain of both types) with which to perform the comparison, e.g. if the two types are char and unsigned short, the function will return `int`.  On the other hand, a conversion may not be necessary at all - given two non-arithmetic unrelated types that don't have an implicit conversion between them, it will return void, in which case no conversions should be done to the comparends.

It handles equally well 32-bit and 16-bit `int`s.

###Choosing a return type
A developer's intuition may incline them to think that the best return type of a min/max function is the type under which the comparison was performed. A clever observation points to the opposite: consider the minimum of an unsigned char and an `unsigned int` – the best comparison type is `unsigned int`, but the best return value is in fact unsigned char – the minimum of the two arguments is guaranteed to fit into an unsigned char! Now, consider the maximum of a `long long` and an `unsigned long long` – the comparison itself may be burdensome to perform, but the result is guaranteed to fit into an `unsigned long long`.

####`min_return_type` and `max_return_type`
The library provides two meta-functions that, given two types, determine the best (smallest) type which the can hold the min/max value of two variables of the given types.

Comparisons between floating point and integral values
------------------------------------------------------

The library supports in a natural way comparisons between floating point and integral values. For example, the comparison between a `float` and an `int` is performed with both arguments cast to `double`. According to the IEEE-754 standard, the double-precision floating point type provides 54 significant bits (53 bits plus an implicit bit for normalized values) for the mantissa which is enough to hold both a `float` and an `int` without loss of precision. 

The standard, however, does not mandate the size of an extended-precision floating point type. In C++ this type is called `long double` and in x86 architectures it is 80-bits long. This type is big enough to be used to perform a comparison between `double` and `long long`, but it's not perfectly clear if it will work on other platforms. Due to this reason such comparisons are disabled by default and can be enabled by defining the `USE_LONG_DOUBLE_FOR_COMPARISON` macro.

Another problem is the precision mode of the FPU. If it's set to 23-bit, then all FPU operations are performed as if with `float`s, even if the operands are `double`s or `long double`s. In this particular mode, even if the `(float, int)->double` comparison type seems like a legit decision, it may yield incorrect results.  Because of this problem, by default the library fails to compile min/max instantiations for mixed integral and floating point types. To enable these comparison (at your own risk) define the `PROMOTE_FP_TYPES_FOR_COMPARISON`.

In most cases it is safe to define both these macros.


Compiler errors
---------------
When feeding min/max with arguments that can't be compared, the compiler error message is designed to be as helpful as possible when looking for the source of the error.

When compiled with VC++9:
```
	struct a {};
	struct b {};
	minimum(a(), b());
```

the error message is

```
boost::unsupported_type_pair<A,B>' : base class undefined
1>        with
1>        [
1>            A=a,
1>            B=b
1>        ]
```
which ought to be helpful enough as to hint the developer where they've made a mistake.
