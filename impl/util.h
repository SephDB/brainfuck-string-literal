#ifndef _UTIL_H__
#define _UTIL_H__

#include "util_forward.inc"

/**
* Returns the nth char in the variadic char list
*/
template<int N, char... P>
constexpr char nth_char = nth_char_impl<N,P...>::value;

/**
* Takes the first N elements from variadic char list and applies them to Meta
*/
template<int N, template<char...> class Meta, char... P>
using take_t = typename take<N,Meta,P...>::type;

/**
* Discards N arguments from variadic char list and applies remainder to Meta
*/
template<int N, template<char...> class Meta, char... P>
using discard_t = typename discard<N,(N>0),Meta,P...>::type;

/**
* Counts the number of consecutive instances of C at the start of P as std::size_t ::size, and applies the remaining elements to Meta in Meta<...> ::remainder
*/
template<char C, template<char...> class Meta, char... P>
struct count_consecutive;

#include "util_impl.inc"

#endif