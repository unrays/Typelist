/*********************************************************************
 * EXOTIC.lyst - {Short Description of the Library}
 *
 * Copyright (c) 2026 Félix-Olivier Dumas
 * All rights reserved.
 *
 * Licensed under the Boost Software License, Version 1.0.
 * You may obtain a copy of the License at:
 *     https://www.boost.org/LICENSE_1_0.txt
 *
 * Description:
 *     {Detailed description of the library's purpose and functionality}
 *
 * This file is part of the EXOTIC collection, a set of high-performance
 * C++ libraries designed for modern software development.
 *
 * For more information, visit: https://github.com/unrays/Lyst
 *
 * Author: Félix-Olivier Dumas
 * Version: 1.0.0
 * Last Updated: January 15, 2026
 *********************************************************************/

#ifndef EXOTIC_LYST_H
#define EXOTIC_LYST_H

#define EXOTIC_LYST_DEBUG

#include <cstddef>

namespace EXOTIC {

// ============================================================
// EXOTIC::Utility - type traits and TMP helpers
// ============================================================

namespace Utility {

namespace details {
	template<typename>
	struct size_impl;

	template<template<typename...> class L, typename... Ts>
	struct size_impl<L<Ts...>> {
		static inline constexpr std::size_t value = sizeof...(Ts);
	};

	template<std::size_t...>
	struct make_index_sequence_impl;

	template<std::size_t N, std::size_t... Is>
	struct make_index_sequence_impl<N, Is...>
		: make_index_sequence_impl<N - 1, N - 1, Is...> {
	};

	template<std::size_t... Is>
	struct make_index_sequence_impl<0, Is...> {
		using type = index_sequence<Is...>;
	};

	template<typename, typename>
	struct is_same_impl;

	template<typename T, typename U>
	struct is_same_impl : false_type {};

	template<typename T>
	struct is_same_impl<T, T> : true_type {};
} // namespace EXOTIC::Utility::details

struct true_type {
	static inline constexpr bool value = true;
};

struct false_type { static inline constexpr bool value = false; };

template<class>
struct dependent_false : false_type {};

template<class T>
inline constexpr bool dependent_false_v = dependent_false<T>::value;

template<typename L>
struct size {
	static inline constexpr std::size_t value = details::size_impl<L>::value;
};

template<typename L>
inline constexpr std::size_t size_v = size<L>::value;

template<std::size_t... Is>
struct index_sequence {};

template<std::size_t N>
using make_index_sequence = typename details::make_index_sequence_impl<N>::type;

template<typename T, typename U>
struct is_same {
	using type = details::is_same_impl<T, U>;
};

template<typename T, typename U>
inline constexpr bool is_same_v = is_same<T, U>::type::value;

template<typename T>
struct is_void : is_same<T, void>::type {};

template<typename T>
inline constexpr bool is_void_v = is_void<T>::value;

template<typename T>
struct is_not_void {
	inline static constexpr bool value = !is_void<T>::value;
};

template<typename T>
inline constexpr bool is_not_void_v = is_not_void<T>::value;

} // namespace EXOTIC::Utility


// ============================================================
// EXOTIC::Lyst - typelist and operations
// ============================================================

namespace Lyst {

namespace details {
	template<typename>
	struct clear_impl;

	template<template<typename...> class L, typename... Ts>
	struct clear_impl<L<Ts...>> {
		using type = L<>;
	};

	template<
		std::size_t N, typename List,
		typename Enable = std::enable_if_t<(N < Utility::size_v<List>)>
	>
	struct at_impl;

	template<template<typename...> class L, typename T, typename... Ts>
	struct at_impl<0, L<T, Ts...>> {
		using type = T;
	};

	template<std::size_t N, template<typename...> class L, typename T, typename... Ts>
	struct at_impl<N, L<T, Ts...>> {
		using type = typename at_impl<N - 1, L<Ts...>>::type;
	};

	template<typename>
	struct empty_impl;

	template<template<typename...> class L, typename... Ts>
	struct empty_impl<L<Ts...>> {
		static inline constexpr bool value = sizeof...(Ts) == 0;
	};

	template<typename, typename>
	struct reverse_impl;

	template<template<typename> class L, typename... Ts, std::size_t... Is>
	struct reverse_impl<L<Ts...>, Utility::index_sequence<Is...>> {
		using type = L<at_t<sizeof...(Ts) - 1 - Is, L<Ts...>>...>;
	};

	template<typename, typename, typename>
	struct contains_impl;

	template<typename T, template<typename...> class L, typename... Ts, std::size_t... Is>
	struct contains_impl<T, L<Ts...>, Utility::index_sequence<Is...>> {
		static inline constexpr bool value = (Utility::is_same_v<at_t<Is, L<Ts...>>, T> || ...);
	};

	template<typename, typename, std::size_t>
	struct count_impl;

	template<typename U, template<typename...> class L, std::size_t It>
	struct count_impl<U, L<>, It> {
		inline static constexpr std::size_t value = It;
	};

	template<
		typename U,
		template<typename...> class L, typename T, typename... Ts,
		std::size_t It
	>
	struct count_impl<U, L<T, Ts...>, It> {
		inline static constexpr std::size_t value =
			count_impl<U, L<Ts...>, (Utility::is_same_v<T, U> ? It + 1 : It)>::value;
	};

	template<typename, typename, std::size_t>
	struct index_of_impl;

	template<typename T, template<typename...> class L, typename... Ts>
	struct index_of_impl<T, L<Ts...>, 0> {
		static_assert(Utility::dependent_false_v<T> == false, "Typelist index out of bounds");
		static inline constexpr std::size_t value = ~static_cast<std::size_t>(0);
	};

	template<typename T, template<typename...> class L, typename... Ts, std::size_t Index>
	struct index_of_impl<T, L<Ts...>, Index> {
		static inline constexpr std::size_t value =
			Utility::is_same_v<at_t<Index, L<Ts...>>, T>
				? (Utility::size_v<L<Ts...>> -1) - Index
				: index_of_impl<T, L<Ts...>, (Index - 1)>::value;
	};

	template<typename>
	struct pop_front_impl {};

	template<template<typename...> class L, typename T, typename... Ts>
	struct pop_front_impl<L<T, Ts...>> {
		using type = L<Ts...>;
	};

	template<template<typename...> class L>
	struct pop_front_impl<L<>> {
		using type = L<>;
	};

	template<typename>
	struct front_impl;

	template<template<typename...> class L, typename T, typename... Ts>
	struct front_impl<L<T, Ts...>> {
		using type = T;
	};

	template<typename>
	struct back_impl;

	template<template<typename...> class L, typename... Ts>
	struct back_impl<L<Ts...>> {
		using type = at_t<Utility::size_v<L<Ts...>> -1, L<Ts...>>;
	};

	template<typename, typename>
	struct push_back_impl;

	template<typename T, template<typename...> class L, typename... Ts>
	struct push_back_impl<T, L<Ts...>> {
		using type = L<Ts..., T>;
	};

	template<template<typename> class, typename, typename>
	struct push_back_if_impl;

	template<
		template<typename> class Pred, typename T,
		template<typename...> class L, typename... Ts
	>
	struct push_back_if_impl<Pred, T, L<Ts...>> {
		using type = std::conditional_t<Pred<T>::value, L<Ts..., T>, L<Ts...>>;
	};

	template<typename, typename>
	struct push_front_impl;

	template<typename T, template<typename...> class L, typename... Ts>
	struct push_front_impl<T, L<Ts...>> {
		using type = L<T, Ts...>;
	};

	template<typename, typename>
	struct concat_impl;

	template<template<typename> class L, typename... Ts1, typename... Ts2>
	struct concat_impl<L<Ts1...>, L<Ts2...>> {
		using type = L<Ts1..., Ts2...>;
	};

	template<std::size_t N, typename NewType, typename List, typename Seq,
		typename Enable = std::enable_if_t<(N < Utility::size_v<List>)>
	>
	struct replace_impl;

	template<
		std::size_t N, typename NewType,
		template<typename...> class L, typename... Ts,
		std::size_t... Is
	>
	struct replace_impl<N, NewType, L<Ts...>, Utility::index_sequence<Is...>> {
		using type = L<
			std::conditional_t<Is == N, NewType, at_t<Is, L<Ts...>>>...
		>;
	};

	template<template<typename> class, typename, typename>
	struct filter_impl;

	template<
		template<typename> class Pred, typename Acc,
		template<typename...> class L
	>
	struct filter_impl<Pred, Acc, L<>> {
		using type = Acc;
	};

	template<
		template<typename> class Pred, typename Acc,
		template<typename...> class L, typename T, typename... Ts
	>
	struct filter_impl<Pred, Acc, L<T, Ts...>> {
		using type = typename filter_impl<Pred, push_back_if_t<Pred, T, Acc>, L<Ts...>>::type;
	};

	template<template<typename> class, typename>
	struct transform_impl;

	template<template<typename> class F, template<typename> class L, typename... Ts>
	struct transform_impl<F, L<Ts...>> {
		using type = L<F<Ts>...>;
	};

	template<typename... Ts>
	struct pop_back_impl;

	template<template<typename...> class L, typename... Ts>
	struct pop_back_impl<L<Ts...>> {
		using type = reverse_t<pop_front_t<reverse_t<L<Ts...>>>>;
	};

	template<typename, typename>
	struct remove_first_impl;

	template<
		typename U,
		template<typename...> class L, typename T, typename... Ts
	>
	struct remove_first_impl<U, L<T, Ts...>> {
		using type = filter_t<
			is_not_null_type, replace_t<index_of_v<U, L<T, Ts...>>, null_type, L<T, Ts...>>
		>;
	};

	template<typename, typename>
	struct remove_all_impl;

	template<typename T, template<typename...> class L, typename... Ts>
	struct remove_all_impl<T, L<Ts...>> {
		using type = filter_t<
			is_not_null_type,
			typelist<std::conditional_t<Utility::is_same_v<Ts, T>, null_type, Ts>...>
		>;
	};
} // namespace EXOTIC::Lyst::details

struct null_type {};

template<typename... Ts>
struct typelist {
	using types = typelist<Ts...>;
};

template<typename T>
struct is_null_type : Utility::is_same<T, null_type>::type {};

template<typename T>
inline constexpr bool is_null_type_v = is_null_type<T>::value;

template<typename T>
struct is_not_null_type {
	inline static constexpr bool value = !is_null_type<T>::value;
};

template<typename T>
inline constexpr bool is_not_null_type_v = is_not_null_type<T>::value;

template<typename L>
struct clear {
	using type = typename details::clear_impl<L>::type;
};

template<typename L>
using clear_t = typename clear<L>::type;

template<std::size_t N, typename L>
struct at {
	using type = typename details::at_impl<N, L>::type;
};

template<std::size_t N, typename L>
using at_t = typename at<N, L>::type;

template<typename L>
struct empty {
	static inline constexpr bool value = details::empty_impl<L>::value;
};

template<typename L>
inline constexpr bool empty_v = empty<L>::value;

template<typename L>
struct reverse {
	using type = typename details::reverse_impl<
		L, Utility::make_index_sequence<Utility::size_v<L>>
	>::type;
};

template<typename L>
using reverse_t = typename reverse<L>::type;

template<typename T, typename L>
struct contains {
	static inline constexpr bool value =
		details::contains_impl<T, L, Utility::make_index_sequence<Utility::size_v<L>>>::value;
};

template<typename T, typename L>
inline constexpr bool contains_v = contains<T, L>::value;

template<typename U, typename L>
struct count {
	inline static constexpr std::size_t value = details::count_impl<U, L, 0>::value;
};

template<typename U, typename L>
inline constexpr std::size_t count_v = count<U, L>::value;

template<typename T, typename L>
struct index_of {
	using reversed = typename reverse_t<L>;
	static inline constexpr std::size_t value =
		details::index_of_impl<T, reversed, Utility::size_v<reversed> -1>::value;
};

template<typename T, typename L>
inline constexpr std::size_t index_of_v = index_of<T, L>::value;

template<typename L>
struct pop_front {
	using type = typename pop_front_impl<L>::type;
};

template<typename L>
using pop_front_t = typename pop_front<L>::type;

template<typename L>
struct front {
	using type = typename front_impl<L>::type;
};

template<typename L>
using front_t = typename front<L>::type;

template<typename L>
struct back {
	using type = typename details::back_impl<L>::type;
};

template<typename L>
using back_t = typename back<L>::type;

template<typename L>
struct first {
	using type = typename front<L>::type;
};

template<typename L>
using first_t = typename first<L>::type;

template<typename L>
struct last {
	using type = typename back<L>::type;
};

template<typename L>
using last_t = typename last<L>::type;

template<typename T, typename L>
struct push_back {
	using type = typename details::push_back_impl<T, L>::type;
};

template<typename T, typename L>
using push_back_t = typename push_back<T, L>::type;

template<template<typename> class Pred, typename T, typename L>
struct push_back_if {
	using type = typename details::push_back_if_impl<Pred, T, L>::type;
};

template<template<typename> class Pred, typename T, typename L>
using push_back_if_t = typename push_back_if<Pred, T, L>::type;

template<typename T, typename L>
struct push_front {
	using type = typename details::push_front_impl<T, L>::type;
};

template<typename T, typename L>
using push_front_t = typename push_front<T, L>::type;

template<typename L1, typename L2>
struct concat {
	using type = typename details::concat_impl<L1, L2>::type;
};

template<typename L1, typename L2>
using concat_t = typename concat<L1, L2>::type;

template<std::size_t N, typename NewType, typename L>
struct replace {
	using type = typename details::replace_impl<
		N, NewType, L, Utility::make_index_sequence<Utility::size_v<L>>
	>::type;
};

template<std::size_t N, typename NewType, typename L>
using replace_t = typename replace<N, NewType, L>::type;

template<template<typename> class Pred, typename L, typename Acc = typelist<>>
struct filter {
	using type = typename details::filter_impl<Pred, Acc, L>::type;
};

template<template<typename> class Pred, typename L, typename Acc = typelist<>>
using filter_t = typename filter<Pred, L, Acc>::type;

template<template<typename> class F, typename L>
struct transform {
	using type = typename details::transform_impl<F, L>::type;
};

template<template<typename> class F, typename L>
using transform_t = typename transform<F, L>::type;

template<typename L>
struct pop_back {
	using type = typename details::pop_back_impl<L>::type;
};

template<typename L>
using pop_back_t = typename pop_back<L>::type;

template<typename U, typename L>
struct remove_first {
	using type = typename details::remove_first_impl<U, L>::type;
};

template<typename U, typename L>
using remove_first_t = typename remove_first<U, L>::type;

template<typename T, typename L>
struct remove_all {
	using type = typename details::remove_all_impl<T, L>::type;
};

template<typename T, typename L>
using remove_all_t = typename remove_all<T, L>::type;

} // namespace EXOTIC::Lyst
} // namespace EXOTIC

#endif // EXOTIC_LYST_H
