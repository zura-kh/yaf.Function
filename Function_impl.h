// yaf::Function - generic representation of a callable entity.
// Copyright (c) 2009-2012, Zurab Khetsuriani (zura.khetsuriani 'at gmail.com)
// Distributed under the Boost Software License, Version 1.0.
// (See BOOST_LICENSE_1_0.txt file or copy at http://www.boost.org/LICENSE_1_0.txt)

// NOTE: this is a private header file and should NOT be included directly


#define YAF_TEMPLATE_ARG_LIST		YAF_CONCAT(YAF_TEMPLATE_ARG_LIST_, YAF_ARG_COUNT)

#define YAF_SIGNATURE_SPEC			YAF_CONCAT(YAF_SIGNATURE_SPEC_, YAF_ARG_COUNT)
//#define YAF_SIGNATURE_SPEC_impl	YAF_CONCAT(YAF_SIGNATURE_SPEC_, YAF_ARG_COUNT)
//#define YAF_SIGNATURE_SPEC		YAF_SIGNATURE_SPEC_impl(YAF_DUMMY_ARG)

#define YAF_FUNCTION_ARG_TYPE_LIST	YAF_CONCAT(YAF_FUNCTION_ARG_TYPE_LIST_, YAF_ARG_COUNT)
#define YAF_FUNCTION_ARG_VAR_LIST	YAF_CONCAT(YAF_FUNCTION_ARG_VAR_LIST_, YAF_ARG_COUNT)
#define YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST		YAF_CONCAT(YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST_, YAF_ARG_COUNT)



namespace yaf {





template <class T,  YAF_TEMPLATE_ARG_LIST>
Function<YAF_SIGNATURE_SPEC> mem_fun(T* obj, ReturnType (T::*method)(YAF_FUNCTION_ARG_TYPE_LIST) )
{
	return Function<YAF_SIGNATURE_SPEC>(obj, method);
}


template <class T,  YAF_TEMPLATE_ARG_LIST>
Function<YAF_SIGNATURE_SPEC> mem_fun(const T* obj, ReturnType (T::*method)(YAF_FUNCTION_ARG_TYPE_LIST) const)
{
	return Function<YAF_SIGNATURE_SPEC>(obj, method);
}






	namespace impl {


		template <YAF_TEMPLATE_ARG_LIST>
		class InvokerBase<YAF_SIGNATURE_SPEC> {
		public:

			virtual InvokerBase* Clone() const = 0;

			virtual ReturnType Invoke(YAF_FUNCTION_ARG_TYPE_LIST) = 0;

			virtual ~InvokerBase() {}
		};



		// function reference
		template <YAF_TEMPLATE_ARG_LIST>
		class FunctionInvoker<YAF_SIGNATURE_SPEC>: public InvokerBase<YAF_SIGNATURE_SPEC> {
		public:

			typedef ReturnType Signature(YAF_FUNCTION_ARG_TYPE_LIST);

			FunctionInvoker(Signature& fun): m_fun(fun) {}

			FunctionInvoker(const FunctionInvoker& other): m_fun(other.m_fun) {}

			FunctionInvoker* Clone() const { return new FunctionInvoker(*this); }

			ReturnType Invoke(YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST) { return m_fun(YAF_FUNCTION_ARG_VAR_LIST); }

		private:
			Signature& m_fun;
		};




		// functor (Functor object is copied by value)
		template <class Functor, YAF_TEMPLATE_ARG_LIST>
		class FunctorInvoker<Functor, YAF_SIGNATURE_SPEC>: public InvokerBase<YAF_SIGNATURE_SPEC> {
		public:

			FunctorInvoker(const Functor& functor): m_functor(functor) {}

			FunctorInvoker(const FunctorInvoker& other): m_functor(other.m_functor) {}

			FunctorInvoker* Clone() const { return new FunctorInvoker(*this); }

			ReturnType Invoke(YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST) { return m_functor(YAF_FUNCTION_ARG_VAR_LIST); }

		private:
			Functor m_functor;
		};




		// member function
		template <class ObjectPtr, typename Method, YAF_TEMPLATE_ARG_LIST>
		class MemberFunctionInvoker<ObjectPtr, Method, YAF_SIGNATURE_SPEC>: public InvokerBase<YAF_SIGNATURE_SPEC> {
		public:

			MemberFunctionInvoker(const ObjectPtr& obj, Method method): m_obj(obj), m_method(method) {}

			MemberFunctionInvoker(const MemberFunctionInvoker& other): m_obj(other.m_obj), m_method(other.m_method) {}

			MemberFunctionInvoker* Clone() const { return new MemberFunctionInvoker(*this); }

			ReturnType Invoke(YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST) { return (m_obj->*m_method)(YAF_FUNCTION_ARG_VAR_LIST); }

		private:
			ObjectPtr m_obj;
			Method m_method;
		};




	} // namespace impl




	

template <YAF_TEMPLATE_ARG_LIST>
class Function<YAF_SIGNATURE_SPEC>

#if YAF_ARG_COUNT == 1
	: public std::unary_function<A1, ReturnType>
#elif YAF_ARG_COUNT == 2
	: public std::binary_function<A1, A2, ReturnType>
#endif

{
public:

	typedef ReturnType Signature(YAF_FUNCTION_ARG_TYPE_LIST);


	Function(): m_invoker(0) {}

	/// function reference
	Function(Signature& func): m_invoker(new impl::FunctionInvoker<Signature>(func)) {}

	/// functor (Functor object is copied by value)
	template <class Functor>
	Function(const Functor& functor): m_invoker(new impl::FunctorInvoker<Functor, Signature>(functor)) {}

	/// member function
	template <class ObjectPtr, typename Method>
	Function(const ObjectPtr& obj, Method method):
	m_invoker(new impl::MemberFunctionInvoker<ObjectPtr, Method, Signature>(obj, method)) {}


	/// copy constructor
	Function(const Function& other): m_invoker(other ? other.m_invoker->Clone() : 0) {}


	Function& operator=(const Function& rhs)
	{
		Function tmp(rhs);
		tmp.swap(*this);
		return *this;
	}


	void swap(Function& other)
	{
		impl::swap(m_invoker, other.m_invoker);
	}


	bool empty() const
	{
		return m_invoker.get() == 0;
	}



private:
	// safe bool idiom
	struct safe_bool_conversion_type { int dummy; };
	typedef int safe_bool_conversion_type::* safe_bool_conversion;

public:
	operator safe_bool_conversion() const
	{
		return !empty() ? &safe_bool_conversion_type::dummy : 0;
	}





	ReturnType operator()(YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST) const
	{
		if (empty())
			throw bad_function_call();

		return m_invoker->Invoke(YAF_FUNCTION_ARG_VAR_LIST);
	}



private:
	std::auto_ptr<impl::InvokerBase<Signature> > m_invoker;
};





} //namespace yaf






#undef YAF_TEMPLATE_ARG_LIST
//#undef YAF_SIGNATURE_SPEC_impl
#undef YAF_SIGNATURE_SPEC
#undef YAF_FUNCTION_ARG_TYPE_LIST
#undef YAF_FUNCTION_ARG_VAR_LIST
#undef YAF_FUNCTION_ARG_TYPE_WITH_VAR_LIST
