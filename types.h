template<typename... T>
struct types {
  template<template<typename...> class Meta>
  using apply = Meta<T...>;
};

template<typename... T>
struct types_concat;

template<typename... T>
using types_concat_t = typename types_concat<T...>::type;

template<typename... T, typename... U, typename... Tail>
struct types_concat<types<T...>,types<U...>,Tail...> {
  using type = types_concat_t<types<T...,U...>,Tail...>;
};

template<typename T>
struct types_concat<T> {
  using type = T;
};

template<int N, typename... T>
struct types_take;

template<int N, typename... T>
using types_take_t = typename types_take<N,T...>::type;

template<int N, typename H, typename... T>
struct types_take<N,H,T...> {
  using type = types_concat_t<types<H>,types_take_t<N-1,T...>>;
};

template<typename H, typename... T>
struct types_take<0,H,T...> {
  using type = types<H>;
};

template<int N, typename... T>
struct types_drop;

template<int N, typename... T>
using types_drop_t = typename types_drop<N,T...>::type;

template<int N, typename H, typename... T>
struct types_drop<N,H,T...> {
  using type = types_drop_t<N-1,T...>;
};

template<typename... T>
struct types_drop<0,T...> {
  using type = types<T...>;
};