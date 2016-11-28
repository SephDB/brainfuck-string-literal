#ifndef _UTIL_H__
#define _UTIL_H__

#include "util_forward.inc"

/**
* Returns the nth char in the variadic char list
*/
template<int N, char... P>
constexpr char nth_char = nth_char_impl<N,P...>::value;

/**
* returns Meta<0,1,2,3,...,N-1>
*/
template<int N, template<std::size_t... I>class Meta>
using apply_index_t = typename apply_index<N,Meta>::type;

/**
* Takes the first N elements from variadic char list and applies them to Meta
*/
template<int N, template<char...> class Meta, char... P>
using take_t = typename take<N,Meta,P...>::type;

/**
* Discards N arguments from variadic char list and applies remainder to Meta
*/
template<int N, template<char...> class Meta, char... P>
using drop_t = typename drop<N,(N>0),Meta,P...>::type;

/**
* Finds the position of the first occurence of C in P.
*/
template<char C, char... P>
constexpr int find_v = find<C,P...>::value;

/**
* Finds the position of the nth occurence of C in P(0-indexed)
*/
template<int N, char C, char... P>
constexpr int find_nth_v = find_nth<N,C,P...>();

/**
* Alias for the nth type in a list of types
*/
template<int N, typename... Ts>
using get_t = typename get<N,Ts...>::type;

template<char C, char... P>
constexpr int count = num_occurences<C,P...>::value;

template<int Start, int Size, template<char...> class Meta, char...P>
using extract_t = typename extract<Start,Size,Meta,P...>::type;

/**
* Splits a variadic list into a tuple of Meta<Split...>...
*/
template<char Sep, template<char...> class Meta, template<typename...>class Apply, char... P>
using split_t = typename split<Sep,Meta,Apply,P...>::type;

/**
* Counts the number of consecutive instances of C at the start of P as std::size_t ::size, and applies the remaining elements to Meta in Meta<...> ::remainder
*/
template<char C, template<char...> class Meta, char... P>
struct count_consecutive;

#include "util_impl.inc"

#endif