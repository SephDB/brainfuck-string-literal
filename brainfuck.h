#ifndef _BRAINFUCK_H__
#define _BRAINFUCK_H__

#include <cstdio>
#include <array>
#include <utility>

#include "impl/util.h"
#include "impl/impl.h"

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

template<>
struct bf_impl<>
{
  static void run() {};
};

template<char C, char... P>
struct bf_impl<C, P...>
{
  static void run() {
  	bf_impl<P...>::run();
  }
};

template<char... P>
struct bf_impl<'[',P...>
{
  static constexpr int loop_size = closing_paren<P...>;
  
  static void run() {
    while(*ptr) take_t<loop_size,bf_impl,P...>::run();
    drop_t<loop_size+1,bf_impl,P...>::run();
  }
};

template <char... P>
struct bf_impl<'+',P...>
{
  static void run() {
    using counter = count_consecutive<'+',bf_impl,P...>;
    *ptr += counter::size;
    counter::remainder::run();
  }
};

template <char... P>
struct bf_impl<'>',P...>
{
  static void run() {
    using counter = count_consecutive<'>',bf_impl,P...>;
    ptr += counter::size;
    counter::remainder::run();
  }
};

template <char... P>
struct bf_impl<'<',P...>
{
  static void run() {
    using counter = count_consecutive<'<',bf_impl,P...>;
    ptr -= counter::size;
    counter::remainder::run();
  }
};

template <char... P>
struct bf_impl<'-',P...>
{
  static void run() {
    using counter = count_consecutive<'-',bf_impl,P...>;
    *ptr -= counter::size;
    counter::remainder::run();
  }
};

template <char... P>
struct bf_impl<'.',P...>
{
  static void run() {
    std::putchar(*ptr);
    bf_impl<P...>::run();
  }
};

template <char... P>
struct bf_impl<',',P...>
{
  static void run() {
    *ptr = std::getchar();
    bf_impl<P...>::run();
  }
};

#endif