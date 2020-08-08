#ifndef __WS_TOKENS_H__
#define __WS_TOKENS_H__

#include "types.h"

template<char>
struct char_t {};

using A = char_t<' '>;
using B = char_t<'\t'>;
using C = char_t<'\n'>;

template<char... Chars>
using tokenize = types_select_t<any_of<A,B,C>::template apply, char_t<Chars>...>;

#endif