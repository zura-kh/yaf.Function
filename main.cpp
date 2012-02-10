// Author: Zurab Khetsuriani (zura.khetsuriani 'at gmail.com)

// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>

#include <iostream>
#include <cstring>  // strlen
#include <cstdlib>  // EXIT_SUCCESS
#include <assert.h>

#include "Function.h"




int func1()
{
	std::cout << "func1 is called\n";
	return 1;
}


int func2()
{
	std::cout << "func2 is called\n";
	return 2;
}


int sum(int a, int b)
{
	return a + b;
}


struct FuncObj {
	int operator()() const
	{
		std::cout << "FuncObj::operator() is called" << std::endl;
		return 10;
	}
};



struct SomeClass {
	int get_int(double d, char c)  const
	{
		std::cout << "SomeClass::get_int is called with arguments: " << d << ", " << c << std::endl;
		return 50;
	}

	void method_with_ref_arg(int& i) const
	{
		i = 17;
	}
};






int main()
{
	// free function
	yaf::Function<int (int, int)> fsum(sum);
	assert(fsum(10,5) == sum(10, 5));

	yaf::Function<std::size_t (const char*)> fstrlen(std::strlen);
	assert(fstrlen("abc") == 3);

	// functor
	FuncObj fobj;
	const yaf::Function<int ()> f1(fobj);
	assert(f1); // not empty
	f1();
	

	// with 7 parameters
	yaf::Function<bool (int, int, int, int, int, int, int)> f7;
	f7 = [](int, int, int, int, int, int, int)->bool { return true; }; // C++11 lambdas are functors
	assert(f7(1,2,3,4,5,6,7));
	

	// member function
	SomeClass obj;
	// init via contructor
	yaf::Function<int (double, char)> f2(&obj, &SomeClass::get_int);
	assert(f2(1.23, 'b') == obj.get_int(1.23, 'b'));

	// or via yaf::mem_fun
	yaf::Function<int (double, char)> f3 = yaf::mem_fun(&obj, &SomeClass::get_int);
	assert(f3(1.23, 'b') == obj.get_int(1.23, 'b'));

	
	yaf::Function<int (int)> fempty;

	try {
		fempty(10);

	} catch (const yaf::bad_function_call& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	
	assert(static_cast<bool>(fempty) == !fempty.empty());
	

	yaf::Function<int ()> ffunc1 = func1, ffunc2 = func2;

	yaf::swap(ffunc1, ffunc2);
	// or ffunc1.swap(ffunc2);

	assert(ffunc1() == func2() && ffunc2() == func1());

	ffunc2 = ffunc1; // assignement
	assert(ffunc2() == ffunc1());


	yaf::Function<void (int&)> fwithref = yaf::mem_fun(&obj, &SomeClass::method_with_ref_arg);
	int i = 0;
	fwithref(i);
	assert(i == 17);

	std::cout << "* Success *" << std::endl;

	return EXIT_SUCCESS;
}

