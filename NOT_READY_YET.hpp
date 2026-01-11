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
 * For more information, visit: {URL or GitHub Link}
 *
 * Author: Félix-Olivier Dumas
 * Version: 0.4.0
 * Last Updated: January 11, 2026
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

/**************************************/
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

template<typename>
struct pop_front {};

template<typename Head, typename... Tail>
struct pop_front<typelist<Head, Tail...>> {
    using type = typelist<Tail...>;
};

template<>
struct pop_front<typelist<>> {
    using type = typelist<>;
};

template<typename List>
using pop_front_t = typename pop_front<List>::type;

/**************************************/

template<typename... Ts>
struct pop_back; //reverse + pop_front + reverse

template<typename T>
struct pop_back<typelist<T>> {
    using type = typelist<>;
};

template<typename First, typename... Rest>
struct pop_back<typelist<First, Rest...>> {
private:
    template<typename... Ts>
    using TailPop = pop_back<typelist<Ts...>>::type;

private:
    template<typename, typename>
    struct Prepend;

    template<typename T1, typename... Ts>
    struct Prepend<T1, typelist<Ts...>> {
        using type = typelist<T1, Ts...>;
    };

public:
    using type = Prepend<First, TailPop<Rest...>>::type;
};

template<typename List>
using pop_back_t = typename pop_back<List>::type;

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
struct back<typelist<First, Rest...>> {
    using type = at<
        typelist_size_v<typelist<First, Rest...>> - 1,
        typelist<First, Rest...>
    >;
};

template<typename List>
using back_t = typename back<List>::type;

/**************************************/

template<typename, typename>
struct push_back;

template<typename T>
struct push_back<T, typelist<>> {
    using type = typelist<T>;
};

template<typename T, typename First, typename... Rest>
struct push_back<T, typelist<First, Rest...>> {
    using type = typelist<First, Rest..., T>;
};

template<typename T, typename List>
using push_back_t = typename push_back<T, List>::type;

/**************************************/

template<typename, typename>
struct push_front;

template<typename T>
struct push_front<T, typelist<>> {
    using type = typelist<T>;
};

template<typename T, typename First, typename... Rest> 
struct push_front<T, typelist<First, Rest...>> {
    using type = typelist<T, First, Rest...>;
};

template<typename T, typename List>
using push_front_t = typename push_front<T, List>::type;

/**************************************/

template<typename, typename>
struct concat;

template<>
struct concat<typelist<>, typelist<>> {
    using type = typelist<>;
};

template<typename T1, typename... Ts1>
struct concat<typelist<T1, Ts1...>, typelist<>> {
    using type = typelist<T1, Ts1...>;
};

template<typename T2, typename... Ts2>
struct concat<typelist<>, typelist<T2, Ts2...>> {
    using type = typelist<T2, Ts2...>;
};

template<typename T1, typename... Ts1, typename T2, typename... Ts2>
struct concat<typelist<T1, Ts1...>, typelist<T2, Ts2...>> {
    using type = typelist<T1, Ts1..., T2, Ts2...>;
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

template<template<typename> class F, typename List>
struct transform;

// À TERMINER...

/**************************************/

template<typename>
struct reverse;

template<typename T1, typename... Ts> 
struct reverse<typelist<T1, Ts...>> {


};

// À TERMINER...

/**************************************/


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

    std::cout << typeid(new_list_4).name() << "\n";
}

#endif // EXOTIC_TYPELIST_H
