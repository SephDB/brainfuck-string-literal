#ifndef _BF_IMPL_H__
#define _BF_IMPL_H__

#include <type_traits>

//Global pointer set at the start of each execution, not thread-safe
char* ptr;

template<char C>
struct bf_char;

template<int Depth, int N, typename... P>
struct closing_paren_impl;

/**
* The location of the matching closing parenthesis with P starting right after the opening parenthesis
*
* example: "+-[+[+]>]>+", with P being "+[+]>]>+" will return 5
*/
template<typename... P>
using closing_paren_t = closing_paren_impl<0,0,P...>;

template<int Depth, int N, typename H, typename... T>
struct closing_paren_impl<Depth,N,H,T...>
{
  static constexpr int value = closing_paren_impl<Depth,N+1,T...>::value;
};

template<int N, typename... T>
struct closing_paren_impl<0,N,bf_char<']'>,T...>
{
  static constexpr int value = N;
};

template<int Depth, int N, typename... T>
struct closing_paren_impl<Depth,N,bf_char<']'>,T...>
{
  static constexpr int value = closing_paren_impl<Depth-1,N+1,T...>::value;
};

template<int Depth, int N, typename... T>
struct closing_paren_impl<Depth,N,bf_char<'['>,T...>
{
  static constexpr int value = closing_paren_impl<Depth+1,N+1,T...>::value;
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
struct bf_char<','>
{
  static void run() {
    *ptr = std::getchar();
  }
};

#endif