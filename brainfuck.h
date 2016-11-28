#ifndef _BRAINFUCK_H__
#define _BRAINFUCK_H__

#include <cstdio>
#include <array>
#include <utility>

#include "impl/util.h"
#include "impl/impl.h"
#include "types.h"

//Global pointer set at the start of each execution, not thread-safe
char* ptr;

template<char... P>
struct bf_impl;

template<char... Prog>
struct bf
{
  
  void run()
  {
    std::array<char,30000> d;
	d.fill(0);
	ptr = d.data();
    bf_impl<Prog...>::run();
  }
};

template<char... T>
struct loop
{
  static void run() {
    while(*ptr) bf_impl<T...>::run();
  };
};

template<char C>
struct bf_char;

template<char... P>
struct bf_impl {
  
  template<int LoopStart, bool B>
  struct loop_extractor {
    static constexpr int loop_size = drop_t<LoopStart+1,closing_paren_t,P...>::value;
    
    using front_types = types_take_t<LoopStart,bf_char<P>...>;
	using loop_type = extract_t<LoopStart,loop_size,loop,P...>;
	using back_types = typename drop_t<LoopStart+loop_size+2,bf_impl,P...>::type;
    using type = types_concat_t<front_types,types<loop_type>,back_types>;
  };
  
  ///Case where there is no loop in the program
  template<int N>
  struct loop_extractor<N,true> {
    using type = types<bf_char<P>...>;
  };
  
  static constexpr int loop_start = find_v<'[',P...>;
  using type = typename loop_extractor<loop_start,loop_start==sizeof...(P)>::type;
  
  template<typename... T>
  static void run_helper(types<T...>) {
    (void)std::initializer_list<int>{ (T::run(),0)... };
  };
  
  static void run() {
    run_helper(type{});
  };
};

template<>
struct bf_impl<>
{
  using type = types<>;
  static void run() {};
};

template<char C>
struct bf_char
{
  static void run() {}
};

template <>
struct bf_char<'+'>
{
  static void run() {
    ++*ptr;
  }
};

template <>
struct bf_char<'>'>
{
  static void run() {
    ++ptr;
  }
};

template <>
struct bf_char<'-'>
{
  static void run() {
    --*ptr;
  }
};

template <>
struct bf_char<'<'>
{
  static void run() {
    --ptr;
  }
};

template <>
struct bf_char<'.'>
{
  static void run() {
    std::putchar(*ptr);
  }
};

template <>
struct bf_impl<','>
{
  static void run() {
    *ptr = std::getchar();
  }
};

#endif