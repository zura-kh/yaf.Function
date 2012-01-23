// yaf::Function - generic representation of a callable entity.
// Copyright (c) 2009-2012, Zurab Khetsuriani (zura.khetsuriani 'at gmail.com)
// Distributed under the Boost Software License, Version 1.0.
// (See BOOST_LICENSE_1_0.txt file or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef YAF_FUNCTION_H_
#define YAF_FUNCTION_H_

#include <memory>
#include <stdexcept>
#include <functional>



/**
Usage:

#include "Function.h"

yaf::Function<Signature> func;

Where Signature is any function signature with up to 7 parameters.
Given above, func can be assigned any callable entity with a corresponding signature, which are:
1. Free functions
2. Member functions (via constructor or yaf::mem_fun helper function)
3. Functors (objects with operator()), copied by value
 
When Signature has one or two parameters, yaf::Function<Signature> class is publicly derived
from std::unary_function or std::binary_function respectively.
 
Examples:

// free function
int get_sum(int a, int b)
{
	return a + b;
}


// functor type
struct MaxGetter {
	int operator()(int a, int b) const
	{
		return std::max(a, b);
	}
};


// some class
class A {
	int x;
public:
	// some non-const method
	int get_mul(int a, int b)
	{
		x = a * b;
		return x;
	}
};



yaf::Function<int (int, int)> func;

func = get_sum;
int sum = func(1, 2);

MaxGetter max_getter_functor;
func = max_getter_functor;
int max = func(10, 15);

A a;
func = yaf::mem_fun(&a, &A::get_mul);
int mul = func(3, 4);

if (func) // if func is not empty
	func(1, 2);


yaf::Function<int (int, int)> func2;

// swap with func2 (has the same signature)
func.swap(func2);
yaf::swap(func, func2);

// assign
func = func2;

*/






namespace yaf {




class bad_function_call: public std::runtime_error {
public:
	bad_function_call(): std::runtime_error("call to empty yaf::Function") {}
};









	namespace impl {

		// primary templates

		// base
		template <class Signature>
		class InvokerBase;

		// free function
		template <class Signature>
		class FunctionInvoker;

		// functor
		template <class Functor, class Signature>
		class FunctorInvoker;
		
		// member function
		template <class ObjectPtr, class Method, typename Signature>
		class MemberFunctionInvoker;


		// auto_ptr swap
		template <class T>
		void swap(std::auto_ptr<T>& lhs, std::auto_ptr<T>& rhs)
		{
			T* ptr = lhs.release();
			lhs.reset(rhs.release());
			rhs.reset(ptr);
		}


	} // namespace impl





// primary template
template <typename Sig>
class Function;


// Function swap
template<class Sig>
void swap(Function<Sig>& lhs, Function<Sig>& rhs)
{
	lhs.swap(rhs);
}




} //namespace yaf







// code generation

// typename ReturnType, typename A1, ...
#define YAF_TEMPLATE_ARG_LIST_0 typename ReturnType
#define YAF_TEMPLATE_ARG_LIST_1 YAF_TEMPLATE_ARG_LIST_0, typename A1
#define YAF_TEMPLATE_ARG_LIST_2 YAF_TEMPLATE_ARG_LIST_1, typename A2
#define YAF_TEMPLATE_ARG_LIST_3 YAF_TEMPLATE_ARG_LIST_2, typename A3
#define YAF_TEMPLATE_ARG_LIST_4 YAF_TEMPLATE_ARG_LIST_3, typename A4
#define YAF_TEMPLATE_ARG_LIST_5 YAF_TEMPLATE_ARG_LIST_4, typename A5
#define YAF_TEMPLATE_ARG_LIST_6 YAF_TEMPLATE_ARG_LIST_5, typename A6
#define YAF_TEMPLATE_ARG_LIST_7 YAF_TEMPLATE_ARG_LIST_6, typename A7

#define YAF_COMMA ,

// ReturnType ()
// ReturnType (A1)
// ReturnType (A1, A2)
// ...
#define YAF_SIGNATURE_SPEC_0(arg) ReturnType (arg)
#define YAF_SIGNATURE_SPEC_1(arg) YAF_SIGNATURE_SPEC_0(A1 arg)
#define YAF_SIGNATURE_SPEC_2(arg) YAF_SIGNATURE_SPEC_1(YAF_COMMA A2 arg)
#define YAF_SIGNATURE_SPEC_3(arg) YAF_SIGNATURE_SPEC_2(YAF_COMMA A3 arg)
#define YAF_SIGNATURE_SPEC_4(arg) YAF_SIGNATURE_SPEC_3(YAF_COMMA A4 arg)
#define YAF_SIGNATURE_SPEC_5(arg) YAF_SIGNATURE_SPEC_4(YAF_COMMA A5 arg)
#define YAF_SIGNATURE_SPEC_6(arg) YAF_SIGNATURE_SPEC_5(YAF_COMMA A6 arg)
#define YAF_SIGNATURE_SPEC_7(arg) YAF_SIGNATURE_SPEC_6(YAF_COMMA A7 arg)


// A1, A2, ...
#define YAF_FUNCTION_ARG_TYPE_LIST_0 
#define YAF_FUNCTION_ARG_TYPE_LIST_1 A1
#define YAF_FUNCTION_ARG_TYPE_LIST_2 YAF_FUNCTION_ARG_TYPE_LIST_1, A2
#define YAF_FUNCTION_ARG_TYPE_LIST_3 YAF_FUNCTION_ARG_TYPE_LIST_2, A3
#define YAF_FUNCTION_ARG_TYPE_LIST_4 YAF_FUNCTION_ARG_TYPE_LIST_3, A4
#define YAF_FUNCTION_ARG_TYPE_LIST_5 YAF_FUNCTION_ARG_TYPE_LIST_4, A5
#define YAF_FUNCTION_ARG_TYPE_LIST_6 YAF_FUNCTION_ARG_TYPE_LIST_5, A6
#define YAF_FUNCTION_ARG_TYPE_LIST_7 YAF_FUNCTION_ARG_TYPE_LIST_6, A7

// a1, a2, ...
#define YAF_FUNCTION_ARG_VAR_LIST_0 
#define YAF_FUNCTION_ARG_VAR_LIST_1 a1
#define YAF_FUNCTION_ARG_VAR_LIST_2 YAF_FUNCTION_ARG_VAR_LIST_1, a2
#define YAF_FUNCTION_ARG_VAR_LIST_3 YAF_FUNCTION_ARG_VAR_LIST_2, a3
#define YAF_FUNCTION_ARG_VAR_LIST_4 YAF_FUNCTION_ARG_VAR_LIST_3, a4
#define YAF_FUNCTION_ARG_VAR_LIST_5 YAF_FUNCTION_ARG_VAR_LIST_4, a5
#define YAF_FUNCTION_ARG_VAR_LIST_6 YAF_FUNCTION_ARG_VAR_LIST_5, a6
#define YAF_FUNCTION_ARG_VAR_LIST_7 YAF_FUNCTION_ARG_VAR_LIST_6, a7

// A1 a1, A2 a2, ...
#define YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_0 
#define YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_1 A1 a1
#define YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_2 YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_1, A2 a2
#define YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_3 YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_2, A3 a3
#define YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_4 YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_3, A4 a4
#define YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_5 YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_4, A5 a5
#define YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_6 YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_5, A6 a6
#define YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_7 YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_6, A7 a7






#define YAF_DUMMY_ARG

#define YAF_CONCAT(a, b) YAF_DO_CONCAT(a, b)
#define YAF_DO_CONCAT(a, b) a##b


#define YAF_ARG_COUNT 0
#include "Function_impl.h"
#undef YAF_ARG_COUNT

#define YAF_ARG_COUNT 1
#include "Function_impl.h"
#undef YAF_ARG_COUNT


#define YAF_ARG_COUNT 2
#include "Function_impl.h"
#undef YAF_ARG_COUNT


#define YAF_ARG_COUNT 3
#include "Function_impl.h"
#undef YAF_ARG_COUNT


#define YAF_ARG_COUNT 4
#include "Function_impl.h"
#undef YAF_ARG_COUNT


#define YAF_ARG_COUNT 5
#include "Function_impl.h"
#undef YAF_ARG_COUNT

#define YAF_ARG_COUNT 6
#include "Function_impl.h"
#undef YAF_ARG_COUNT

#define YAF_ARG_COUNT 7
#include "Function_impl.h"
#undef YAF_ARG_COUNT






#undef YAF_TEMPLATE_ARG_LIST_0
#undef YAF_TEMPLATE_ARG_LIST_1
#undef YAF_TEMPLATE_ARG_LIST_2
#undef YAF_TEMPLATE_ARG_LIST_3
#undef YAF_TEMPLATE_ARG_LIST_4
#undef YAF_TEMPLATE_ARG_LIST_5
#undef YAF_TEMPLATE_ARG_LIST_6
#undef YAF_TEMPLATE_ARG_LIST_7

#undef YAF_COMMA

#undef YAF_SIGNATURE_SPEC_0
#undef YAF_SIGNATURE_SPEC_1
#undef YAF_SIGNATURE_SPEC_2
#undef YAF_SIGNATURE_SPEC_3
#undef YAF_SIGNATURE_SPEC_4
#undef YAF_SIGNATURE_SPEC_5
#undef YAF_SIGNATURE_SPEC_6
#undef YAF_SIGNATURE_SPEC_7

#undef YAF_FUNCTION_ARG_TYPE_LIST_0 
#undef YAF_FUNCTION_ARG_TYPE_LIST_1
#undef YAF_FUNCTION_ARG_TYPE_LIST_2
#undef YAF_FUNCTION_ARG_TYPE_LIST_3
#undef YAF_FUNCTION_ARG_TYPE_LIST_4
#undef YAF_FUNCTION_ARG_TYPE_LIST_5
#undef YAF_FUNCTION_ARG_TYPE_LIST_6
#undef YAF_FUNCTION_ARG_TYPE_LIST_7

#undef YAF_FUNCTION_ARG_VAR_LIST_0 
#undef YAF_FUNCTION_ARG_VAR_LIST_1 
#undef YAF_FUNCTION_ARG_VAR_LIST_2 
#undef YAF_FUNCTION_ARG_VAR_LIST_3 
#undef YAF_FUNCTION_ARG_VAR_LIST_4 
#undef YAF_FUNCTION_ARG_VAR_LIST_5 
#undef YAF_FUNCTION_ARG_VAR_LIST_6
#undef YAF_FUNCTION_ARG_VAR_LIST_7

#undef YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_0 
#undef YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_1 
#undef YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_2 
#undef YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_3 
#undef YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_4 
#undef YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_5 
#undef YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_6
#undef YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_7

#undef YAF_DUMMY_ARG

#undef YAF_CONCAT
#undef YAF_DO_CONCAT



#endif // YAF_FUNCTION_H_
