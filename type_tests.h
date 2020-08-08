#ifndef __TYPE_TESTS_H__
#define __TYPE_TESTS_H__

#include <type_traits>

template<typename... T>
struct any_of;

template<typename H, typename... T>
struct any_of<H,T...> {
  template<typename U>
  struct apply {
    static constexpr bool value = std::is_same<H,U>::value || any_of<T...>::template apply<U>::value;
  };
};

template<>
struct any_of<> {
  template<typename U>
  struct apply : std::false_type {};
};

template<template<typename> class... Tests>
struct any_of_tests;

template<template<typename> class H, template<typename> class... T>
struct any_of_tests<H,T...> {
  template<typename U>
  struct apply {
    static constexpr bool value = H<U>::value || any_of_tests<T...>::template apply<U>::value;
  };
};

template<>
struct any_of_tests<> {
  template<typename U>
  struct apply : std::false_type {};
};

#endif