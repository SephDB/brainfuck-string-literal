#ifndef _BRAINFUCK_H__
#define _BRAINFUCK_H__

#include <cstdio>
#include <array>
#include <utility>

#include "impl/util.h"
#include "impl/impl.h"
#include "type_tests.h"
#include "types.h"

template<typename... P>
struct bf_impl;

template<char... Prog>
struct bf
{
  using type = types<bf_char<Prog>...>;
  void run()
  {
    std::array<char,30000> d;
	d.fill(0);
	ptr = d.data();
	type::template apply<bf_impl>::run();
  }
};

template<typename... P>
struct loop
{
  static void run() {
    while(*ptr) bf_impl<P...>::run();
  };
};

template<typename... P>
struct bf_impl {
  
  template<int LoopStart, bool B>
  struct loop_extractor {
    static constexpr int loop_size = types_drop_t<LoopStart+1,P...>::template apply<closing_paren_t>::value;
    
    using front_types = types_take_t<LoopStart,P...>;
	using loop_type = typename types_extract_t<LoopStart,loop_size,P...>::template apply<loop>;
	using back_types = typename types_drop_t<LoopStart+loop_size+2,P...>::template apply<::bf_impl>::type;
    using type = types_concat_t<front_types,types<loop_type>,back_types>;
  };
  
  ///Case where there is no loop in the program
  template<int N>
  struct loop_extractor<N,true> {
    using type = types<P...>;
  };
  
  static constexpr int loop_start = types_find_v<bf_char<'['>,P...>;
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

#endif