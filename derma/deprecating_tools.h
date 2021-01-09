/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

/*
	A lot of Doom code used raw enum elements as raw array indices, so we need to be able to treat them as integral values in this context until
	we can refactor those arrays away (since enums are now explicit types and static casting in place is ugly and right biased)
	
	solution = template function with macro instantiation
	
	NOTE: MANY functions also use old school bit manipulations, so changing from a specific sized integral type to an enum type requires
	refactoring all that 90's razzmatazz out before updating the parameter types
*/

template<typename T, typename U>
constexpr auto _integral_value(U v) noexcept
{
	return static_cast<T>(v);
}

template<typename T>
constexpr auto _underlying_value(T v) noexcept
{
	return static_cast<::std::underlying_type_t<T>>(v);
}
