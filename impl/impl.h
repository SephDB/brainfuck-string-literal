#ifndef _BF_IMPL_H__
#define _BF_IMPL_H__

template<char... P>
struct bf_impl;

template<int Depth, int N, char... P>
struct closing_paren_impl;

/**
* The location of the matching closing parenthesis with P starting right after the opening parenthesis
*
* example: "+-[+[+]>]>+", with P being "+[+]>]>+" will return 5
*/
template<char... P>
constexpr int closing_paren = closing_paren_impl<0,0,P...>::value;

template<int Depth, int N, char H, char... T>
struct closing_paren_impl<Depth,N,H,T...>
{
  static constexpr int value = closing_paren_impl<Depth,N+1,T...>::value;
};

template<int N, char... T>
struct closing_paren_impl<0,N,']',T...>
{
  static constexpr int value = N;
};

template<int Depth, int N, char... T>
struct closing_paren_impl<Depth,N,']',T...>
{
  static constexpr int value = closing_paren_impl<Depth-1,N+1,T...>::value;
};

template<int Depth, int N, char... T>
struct closing_paren_impl<Depth,N,'[',T...>
{
  static constexpr int value = closing_paren_impl<Depth+1,N+1,T...>::value;
};
  
#endif