#pragma once
#include <type_traits>

namespace CityDraft::Utils
{

	// General case: assume callable
	template<typename F>
	using callable_return_t = std::invoke_result_t<F>;

	// Specialize for member function pointer
	template<typename R, typename C, typename... Args>
	R member_function_return_type(R(C::*)(Args...));

	template<typename R, typename C, typename... Args>
	R member_function_return_type(R(C::*)(Args...) const);

	// Trait to deduce return type
	template<typename T>
	struct deduce_return;

	template<typename F>
	struct deduce_return
	{
		using type = std::invoke_result_t<F>;
	};

	template<typename R, typename C, typename... Args>
	struct deduce_return<R(C::*)(Args...)>
	{
		using type = R;
	};

	template<typename R, typename C, typename... Args>
	struct deduce_return<R(C::*)(Args...) const>
	{
		using type = R;
	};

	template<typename T>
	using deduce_return_t = typename deduce_return<T>::type;

}