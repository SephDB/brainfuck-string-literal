template<int N>
struct nth_char_impl<N>
{
  static_assert(N < 0, "N too large");
  static constexpr char value = '0';
};

template<int N, char H, char... T>
struct nth_char_impl<N,H,T...> {
  static constexpr char value = nth_char_impl<N-1,T...>::value;
};

template<char H, char... T>
struct nth_char_impl<0,H,T...> {
  static constexpr char value = H;
};

template<int N, template<char...> class Meta, char... P>
struct take
{
  template<std::size_t... I>
  static constexpr decltype(auto) inner(std::index_sequence<I...>) {
    return Meta<nth_char<I,P...>...>{};
  }
  
  using type = decltype(inner(std::make_index_sequence<N>()));
};

template<int N, template<char...P> class Meta, char H, char... P>
struct drop<N,true,Meta,H,P...> {using type = typename drop<N-1,(N-1>0), Meta, P...>::type;};

template<template<char...P> class Meta, char... P>
struct drop<0,false,Meta,P...> {using type = Meta<P...>;};

template<char C, template<char...> class Meta, char... P>
struct count_consecutive
{
  template<char... Prog>
  struct count
  {
    static constexpr std::size_t size = 0;
    using remainder = Meta<Prog...>;
  };
  
  template<char... Prog>
  struct count<C,Prog...>
  {
    typedef count<Prog...> subcount;
    static constexpr std::size_t size = 1 + subcount::size;
    using remainder = typename subcount::remainder;
  };
  
  static constexpr std::size_t size = 1 + count<P...>::size;
  using remainder = typename count<P...>::remainder;
  
};
