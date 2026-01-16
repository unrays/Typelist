

# EXOTIC.lyst

[![Build](https://img.shields.io/badge/build-passing-brightgreen)](#)
[![License](https://img.shields.io/badge/license-BSL%201.0-blue)](#)
[![C++ Version](https://img.shields.io/badge/C%2B%2B-17-blue)](#)
[![Release](https://img.shields.io/badge/release-1.0.1-blueviolet)](#)

**EXOTIC.lyst** is a high-performance, header-only C++ library for managing and manipulating typelists.
Part of the **EXOTIC** collection of modern C++ libraries, it provides a full suite of compile-time type operations optimized for template metaprogramming (TMP).

---

## Table of Contents

* [Features](#features)
* [Installation](#installation)
* [Usage](#usage)

  * [Typelist Basics](#typelist-basics)
  * [Operations](#operations)
* [Type Utilities](#type-utilities)
* [License](#license)
* [Author](#author)

---

## Features

* Fully header-only C++ library, no dependencies outside the standard library.
* Compile-time typelist operations:

  * Access elements: `at`, `front`, `back`
  * Modification: `push_front`, `push_back`, `replace`, `pop_front`, `pop_back`
  * Filtering and transformation of types
  * Concatenation of typelists
  * Counting and searching types: `count`, `index_of`, `contains`
* Advanced TMP helpers and type traits in `EXOTIC::Utility`
* Zero runtime overhead: all operations are resolved at compile time
* Compatible with modern C++ standards (C++17 and later recommended)

---

## Installation

Simply include the header in your project:

```cpp
#include "EXOTIC.lyst.h"
```

No compilation or linking steps are required, as this is a header-only library.

---

## Usage

### Typelist Basics

A typelist is a compile-time list of types:

```cpp
using MyTypes = EXOTIC::Lyst::typelist<int, float, double>;
```

Check if a typelist is empty:

```cpp
static_assert(!EXOTIC::Lyst::empty_v<MyTypes>, "Typelist should not be empty");
```

Access elements:

```cpp
using FirstType  = EXOTIC::Lyst::front_t<MyTypes>;  // int
using LastType   = EXOTIC::Lyst::back_t<MyTypes>;   // double
using SecondType = EXOTIC::Lyst::at_t<1, MyTypes>;  // float
```

---

### Operations

Modify typelists at compile time:

```cpp
// Push and pop types
using Extended = EXOTIC::Lyst::push_back_t<char, MyTypes>; // int, float, double, char
using Shorter  = EXOTIC::Lyst::pop_front_t<Extended>;      // float, double, char

// Replace a type
using Replaced = EXOTIC::Lyst::replace_t<1, long, MyTypes>; // int, long, double

// Filter types based on a predicate
template<typename T>
struct is_integral_type : EXOTIC::Utility::is_same<T, int>::type {};

using Filtered = EXOTIC::Lyst::filter_t<is_integral_type, MyTypes>; // int

// Transform types
template<typename T>
struct add_pointer { using type = T*; };

using PointerTypes = EXOTIC::Lyst::transform_t<add_pointer, MyTypes>; // int*, float*, double*
```

Other available operations:

* `concat_t<L1, L2>` – concatenate two typelists
* `remove_first_t<T, L>` – remove the first occurrence of a type
* `remove_all_t<T, L>` – remove all occurrences of a type
* `reverse_t<L>` – reverse the order of a typelist
* `count_v<T, L>` – count how many times a type appears
* `contains_v<T, L>` – check if a type exists in a typelist
* `index_of_v<T, L>` – get the compile-time index of a type

---

## Type Utilities

`EXOTIC::Utility` provides TMP and type trait helpers:

* `size_v<L>` – get typelist size
* `is_same_v<T, U>` – check if two types are the same
* `is_void_v<T>` / `is_not_void_v<T>` – check for void types
* `dependent_false_v<T>` – helper for `static_assert` in templates

These utilities enable safer, more readable TMP code.

---

## License

This library is licensed under the **Boost Software License, Version 1.0**.
See [LICENSE](https://www.boost.org/LICENSE_1_0.txt) for details.

---

## Author

**Félix-Olivier Dumas**
GitHub: [unrays/Lyst](https://github.com/unrays/Lyst)
Version: 1.0.0
Last Updated: January 15, 2026

I had zero experience with typelists before this. I built **Lyst** in 5 intense days and am proud of the work.



