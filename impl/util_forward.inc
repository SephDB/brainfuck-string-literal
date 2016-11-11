template<int N, char... P>
struct nth_char_impl;

template<int N, template<std::size_t... I>class Meta>
struct apply_index;

template<int N, template<char...> class Meta, char... P>
struct take;

template<int N, bool B, template<char...P> class Meta, char... P>
struct drop;

template<char C, char... P>
struct find;

template<int N, typename T, typename... Ts>
struct get;

template<char C, char... P>
struct num_occurences;

template<char Sep, template<char...> class Row, template<typename...>class Field,char... P>
struct split;

template<int N, char C>
constexpr auto find_nth() {
  return 0;
}

template<int N, char C, char A, char... P>
constexpr auto find_nth() -> typename std::enable_if<(C==A) && (N<=0),int>::type {
  return 0;
}

template<int N, char C, char A, char...P>
constexpr auto find_nth() -> typename std::enable_if<(C==A) && (N>0),int>::type;

template<int N, char C, char A, char... P>
constexpr auto find_nth() -> typename std::enable_if<(C!=A),int>::type
{
  return 1+find_nth<N,C,P...>();
}


template<int N, char C, char A, char...P>
constexpr auto find_nth() -> typename std::enable_if<(C==A) && (N>0),int>::type {
  return 1+find_nth<N-1,C,P...>();
}