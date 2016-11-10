template<int N, char... P>
struct nth_char_impl;

template<int N, template<char...> class Meta, char... P>
struct take;

template<int N, bool B, template<char...P> class Meta, char... P>
struct drop;