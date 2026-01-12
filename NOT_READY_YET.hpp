/*********************************************************************
 * EXOTIC.typelist - {Short Description of the Library}
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
 * NOTE: The name of this library is provisional and subject to change.
 *       It may not reflect the final name of the library.
 * 
 * For more information, visit: https://github.com/unrays/Typelist
 *
 * Author: Félix-Olivier Dumas
 * Version: 0.6.0
 * Last Updated: January 12, 2026
 *********************************************************************/

#ifndef EXOTIC_TYPELIST_H
#define EXOTIC_TYPELIST_H

#include <iostream>
#include <cstddef>

/**************************************/

template<typename... Ts>
struct typelist {
    using types = typelist<Ts...>;
};

/**************************************/

template<typename>
struct typelist_size;

template<typename... Ts>
struct typelist_size<typelist<Ts...>> {
    static inline constexpr std::size_t value = sizeof...(Ts);
};

template<typename List>
inline constexpr std::size_t typelist_size_v = typelist_size<List>::value;

/**************************************/

//public
template<std::size_t... Is>
struct index_sequence {};

/**************************************/

//details
template<std::size_t...>
struct make_index_sequence_impl;

template<std::size_t N, std::size_t... Is>
struct make_index_sequence_impl<N, Is...>
    : make_index_sequence_impl<N - 1, N - 1, Is...> { };

template<std::size_t... Is>
struct make_index_sequence_impl<0, Is...> {
    using type = index_sequence<Is...>;
};

//public
template<std::size_t N>
using make_index_sequence = typename make_index_sequence_impl<N>::type;

/**************************************/

template<typename> //un peu bad, typelist_size_v le fait deja
struct make_index_sequence_from_typelist;

template<typename... Ts>
struct make_index_sequence_from_typelist<typelist<Ts...>> {
    using type = make_index_sequence<sizeof...(Ts)>;
};

template<typename List>
using make_index_sequence_from_typelist_t =
    typename make_index_sequence_from_typelist<List>::type;

/**************************************/

template<typename>
struct empty;

template<typename... Ts>
struct empty<typelist<Ts...>> {
    static inline constexpr bool value = false;
};

template<>
struct empty<typelist<>> {
    static inline constexpr bool value = true;
};

template<typename List>
inline constexpr bool empty_v = empty<List>::value;

/**************************************/

template<std::size_t N, typename List,
    typename Enable = std::enable_if_t<(N < typelist_size_v<List>)>
>
struct at;

template<typename First, typename... Rest>
struct at<0, typelist<First, Rest...>> {
    using type = First;
};

template<std::size_t N, typename First, typename... Rest>
struct at<N, typelist<First, Rest...>> {
    using type = typename at<N - 1, typelist<Rest...>>::type;
};

template<std::size_t N, typename List>
using at_t = typename at<N, List>::type;

/**************************************/

//details
template<typename>
struct pop_front_impl {};

template<template<typename...> class L, typename Head, typename... Tail>
struct pop_front_impl<L<Head, Tail...>> {
    using type = L<Tail...>;
};

template<template<typename...> class L>
struct pop_front_impl<L<>> {
    using type = L<>;
};

//public
template<typename L>
struct pop_front {
    using type = typename pop_front_impl<L>::type;
};

template<typename L>
using pop_front_t = typename pop_front<L>::type;

/**************************************/

template<typename>
struct front;

template<typename First, typename... Rest>
struct front<typelist<First, Rest...>> {
    using type = First;
};

template<typename List>
using front_t = typename front<List>::type;

/**************************************/

template<typename>
struct back;

template<typename First, typename... Rest>
struct back<typelist<First, Rest...>> { //reverse + first
    using type = at<
        typelist_size_v<typelist<First, Rest...>> - 1,
        typelist<First, Rest...>
    >;
};

template<typename List>
using back_t = typename back<List>::type;

/**************************************/

//details
template<typename, typename>
struct push_back_impl;

template<typename T, template<typename...> class L, typename... Ts>
struct push_back_impl<T, L<Ts...>> {
    using type = typelist<Ts..., T>;
};

//public
template<typename T, typename L>
struct push_back {
    using type = typename push_back_impl<T, L>::type;
};

template<typename T, typename L>
using push_back_t = typename push_back<T, L>::type;

/**************************************/

//details
template<typename, typename>
struct push_front_impl;

template<typename T, template<typename...> class L, typename... Ts>
struct push_front_impl<T, L<Ts...>> {
    using type = L<T, Ts...>;
};

//public
template<typename T, typename L>
struct push_front {
    using type = typename push_front_impl<T, L>::type;
};

template<typename T, typename L>
using push_front_t = typename push_front<T, L>::type;

/**************************************/

//details
template<typename, typename>
struct concat_impl;

template<template<typename> class L, typename... Ts1, typename... Ts2>
struct concat_impl<L<Ts1...>, L<Ts2...>> {
    using type = L<Ts1..., Ts2...>;
};

//public
template<typename L1, typename L2>
struct concat {
    using type = typename concat_impl<L1, L2>::type;
};

template<typename L1, typename L2>
using concat_t = typename concat<L1, L2>::type;

/**************************************/

template<std::size_t, typename, typename>
struct replace;

template<std::size_t N, typename NewType, typename First, typename... Rest>
struct replace<N, NewType, typelist<First, Rest...>> { //faire avec : herite de using type
    using type = typename replace<N - 1, NewType, typelist<Rest...>>::type;

        //on itere jusqua ce que le First != newtype, si == new type, on return early
};

template<typename NewType, typename First, typename... Rest>
struct replace<0, NewType, typelist<First, Rest...>> {
    replace() {
        std::cout << "t\n";
    }
    using type = typelist<NewType, Rest...>;
};

// À TERMINER...s

/**************************************/

//details
template<template<typename> class, typename>
struct transform_impl;

template<template<typename> class F, template<typename> class L, typename... Ts>
struct transform_impl<F, L<Ts...>> {
    using type = typelist<F<Ts>...>;
};

//public
template<template<typename> class F, typename L>
struct transform {
    using type = typename transform_impl<F, L>::type;
};

template<template<typename> class F, typename L>
using transform_t = typename transform<F, L>::type;

/**************************************/

//details
template<typename, typename>
struct reverse_impl;

template<template<typename> class L, typename... Ts, std::size_t... Is>
struct reverse_impl<L<Ts...>, index_sequence<Is...>> {
    using type = typelist<at_t<sizeof...(Ts) - 1 - Is, typelist<Ts...>>...>;
};

//public
template<typename List>
struct reverse {
    using type = typename reverse_impl<
        List,
        make_index_sequence<typelist_size_v<List>>
    >::type;
};

template<typename List>
using reverse_t = typename reverse<List>::type;

/**************************************/

//details
template<typename... Ts>
struct pop_back_impl;

template<template<typename...> class L, typename... Ts>
struct pop_back_impl<L<Ts...>> {
    using type = reverse_t<pop_front_t<reverse_t<L<Ts...>>>>;
};

//public
template<typename L> 
struct pop_back {
    using type = typename pop_back_impl<L>::type;
};

template<typename L>
using pop_back_t = typename pop_back<L>::type;

/**************************************/

// REMPLACER TYPELIST PAR L

int main() {
    using list = typelist<int, float, double, bool>;

    using new_list = pop_front_t<list>;
    using new_list_2 = pop_back_t<new_list>;

    using new_list_3 = push_front_t<bool, new_list_2>;
    using new_list_4 = push_back_t<int, new_list_3>;

    using test = at_t<2, list>;

    using test2 = replace<1, unsigned int, new_list_4>;

    std::cout << typeid(test2::type).name() << "\n";

    std::cout << std::boolalpha << empty_v<list> << "\n";

    std::cout << typeid(make_index_sequence<0>).name() << "\n";

    using new_list_5 = reverse_t<new_list_4>;

    using new_list_6 = push_front_t<long, new_list_5>;

    using new_list_7 = pop_back_impl<new_list_6>::type;

    std::cout << typeid(new_list_4).name() << "\n";
    std::cout << typeid(new_list_7).name() << "\n";
}

#endif // EXOTIC_TYPELIST_H
