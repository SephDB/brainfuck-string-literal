template<int N>
struct nth_char_impl<N>
{
  //static_assert(N < 0, "N too large");
  static constexpr char value = ' ';
};

template<int N, char H, char... T>
struct nth_char_impl<N,H,T...> {
  static constexpr char value = nth_char_impl<N-1,T...>::value;
};

template<char H, char... T>
struct nth_char_impl<0,H,T...> {
  static constexpr char value = H;
};

template<int N, template<std::size_t... I>class Meta>
struct apply_index {
  template<std::size_t... I>
  static constexpr decltype(auto) inner(std::index_sequence<I...>) {
    return Meta<I...>{};
  }
  
  using type = decltype(inner(std::make_index_sequence<N>()));
};

template<int N, template<char...> class Meta, char... P>
struct take
{
  template<std::size_t... I>
  struct construct {
    using type = Meta<nth_char<I,P...>...>;
  };
  
  using type = typename apply_index_t<N,construct>::type;
};

template<int N, template<char...P> class Meta, char H, char... P>
struct drop<N,true,Meta,H,P...> {using type = typename drop<N-1,(N-1>0), Meta, P...>::type;};

template<int N,template<char...P> class Meta, char... P>
struct drop<N,false,Meta,P...> {using type = Meta<P...>;};

template<char C, char A, typename std::enable_if<(C!=A),char>::type... P>
struct find<C,A,P...> {
  static constexpr int value = 1+find<C,P...>::value;
};

template<char C,char... P>
  struct find<C,C,P...>
{
  static constexpr int value = 0;
};

template<char C>
struct find<C> {static constexpr int value = -1;};

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

template<int N, typename T, typename... Ts>
struct get<N,T,Ts...>
{
  using type = typename get<N-1,Ts...>::type;
};

template<typename T, typename... Ts>
struct get<0,T,Ts...>
{
  using type = T;
};

template<int N>
struct get<N>
{
  using type = void;
};

template<char C>
struct num_occurences<C> {static constexpr int value = 0;};

template<char C, char H, char... T>
  struct num_occurences<C,H,T...> {static constexpr int value = ((C==H)?1:0)+num_occurences<C,T...>::value;};

template<char Sep, template<char...> class Row, template<typename...>class Field,char... P>
struct split {
  static constexpr int num_rows = count<Sep,P...>;
  
  template<std::size_t... I>
  struct splitter
  {
    template<std::size_t Index>
    static constexpr int row_start = find_nth_v<Index,Sep,P...>;
    
    template<std::size_t Index>
    static constexpr int row_end = find_nth_v<Index+1,Sep,P...>;
    
    template<std::size_t Index>
    struct taker {
      template<char...Elems>
      struct impl {
        using type = take_t<row_end<Index>-row_start<Index>-1,Row,Elems...>;
      };
      
      using type = typename drop_t<row_start<Index>+1,impl,P...>::type;
    };
    
    using type = Field<take_t<row_start<0>,Row,P...>,typename taker<I>::type...>;
  };
  
  
  using type = typename apply_index_t<num_rows,splitter>::type;
};