#ifndef __TYPES_H__
#define __TYPES_H__

#include <type_traits>
#include "type_tests.h"

template<typename... T>
struct types;

template<typename... T>
struct types_concat;

template<typename... T>
using types_concat_t = typename types_concat<T...>::type;

template<typename... T1, typename... T2, typename... T3, typename... T4, typename... T5, typename... T6, typename... T7, typename... T8, typename... Tail>
struct types_concat<types<T1...>,types<T2...>,types<T3...>,types<T4...>,types<T5...>,types<T6...>,types<T7...>,types<T8...>,Tail...> {
  using type = types_concat_t<types<T1...,T2...,T3...,T4...,T5...,T6...,T7...,T8...>,Tail...>;
};

template<typename... T, typename... U, typename... Tail>
struct types_concat<types<T...>,types<U...>,Tail...> {
  using type = types_concat_t<types<T...,U...>,Tail...>;
};

template<typename T>
struct types_concat<T> {
  using type = T;
};

template<int N, typename... T>
struct types_split;

template<int N, typename H, typename... T>
struct types_split<N,H,T...> {
  using front = types_concat_t<types<H>,typename types_split<N-1,T...>::front>;
  using back = typename types_split<N-1,T...>::back;
};

template<typename H,typename... T>
struct types_split<0,H,T...> {
  using front = types<>;
  using back = types<H,T...>;
};

template<>
struct types_split<0> {
  using front = types<>;
  using back = types<>;
};

template<int N, typename... T>
using types_take_t = typename types_split<N,T...>::front;

template<int N, typename... T>
using types_drop_t = typename types_split<N,T...>::back;

template<template<typename> class Test, typename... T>
struct types_select {
  template<typename U, bool B>
  struct select_test {
    using type = types<U>;
  };
  
  template<typename U>
  struct select_test<U,false> {
    using type = types<>;
  };
  
  using type = types_concat_t<typename select_test<T,Test<T>::value>::type...>;
};

template<template<typename> class Test, typename... T>
using types_select_t = typename types_select<Test,T...>::type;

template<int Start, int Size, typename...P>
struct types_extract {
  template<typename...Elems>
    struct impl {
      using type = types_take_t<Size,Elems...>;
    };
    
    using type = typename types_drop_t<Start+1,P...>::template apply<impl>::type;
};

template<int Start, int Size, typename...P>
using types_extract_t = typename types_extract<Start,Size,P...>::type;

template<typename T, typename... P>
struct types_find;

template<typename T, typename U, typename... P>
struct types_find<T,U,P...> {
  static constexpr int value = 1+types_find<T,P...>::value;
};

template<typename T,typename... P>
struct types_find<T,T,P...>
{
  static constexpr int value = 0;
};

template<typename T>
struct types_find<T> {static constexpr int value = 0;};

/**
* Finds the position of the first occurence of C in P.
*/
template<typename T, typename... P>
constexpr int types_find_v = types_find<T,P...>::value;

template<template<typename...> class Meta, typename Types>
using types_apply_t = typename Types::template apply<Meta>;

template<typename... T>
struct types {
  template<template<typename...> class Meta>
  using apply = Meta<T...>;
  
  template<int N>
  using drop = types_drop_t<N,T...>;
  
  template<int N>
  using take = types_take_t<N,T...>;
  
  template<int Start, int Size>
  using extract = types_extract_t<Start,Size,T...>;
  
  template<typename H>
  static constexpr bool has_type = any_of<T...>::template apply<H>::value;
};

#endif