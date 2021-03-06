#include <cstdio>
#include <random>
#include <ctime>
#include <vector>
#include <utility>
#include <algorithm>
#include <tuple>

#include "impl/util.h"

template<char... P>
struct row
{
  template<int N>
  static constexpr char get() {return nth_char<N,P...>;};
  
  static constexpr std::size_t size = sizeof...(P);
};



template<typename... Rows>
struct field
{
  template<int N>
  using get = get_t<N,Rows...>;
  
  static constexpr std::size_t size = sizeof...(Rows);
  
  static constexpr std::size_t max_row_size = std::max({Rows::size...});
};

template<char... P>
struct program
{
  using instr = split_t<'\n',row,field,P...>;
};

template<typename Pos, class Enable = void>
struct check_bounds {
  using pos = Pos;
};

namespace move {
  struct right {
    template<typename Pos>
	using next_pos = typename check_bounds<typename Pos::template new_pos<Pos::x+1,Pos::y>>::pos;
  };
  struct left {
    template<typename Pos>
	using next_pos = typename check_bounds<typename Pos::template new_pos<Pos::x-1,Pos::y>>::pos;
  };
  struct up {
    template<typename Pos>
	using next_pos = typename check_bounds<typename Pos::template new_pos<Pos::x,Pos::y-1>>::pos;
  };
  struct down {
    template<typename Pos>
	using next_pos = typename check_bounds<typename Pos::template new_pos<Pos::x,Pos::y+1>>::pos;
  };
}

template<int X, int Y, char... P>
struct position
{
  /**
  * Exposes x and y as static members
  */
  static constexpr int x = X;
  static constexpr int y = Y;
  
  /*
  * Data associated with this position
  */
  using p = program<P...>; ///< Program associated w this position
  using f = typename p::instr; ///< Field of instructions
  using r = typename f::template get<Y>; ///< Row this position is in
  static constexpr char value = r::template get<X>(); ///< Value located at this position
  
  template<template<char...> class Meta>
  using apply_program = Meta<P...>;
  
  /**
  * Alias for another position in the same program
  */
  template<int Xp, int Yp>
  using new_pos = position<Xp,Yp,P...>;
};

static_assert(position<1,1,'<','\n','>','a'>::value == 'a',"Position not working");

static_assert(std::is_same<move::right::next_pos<position<0,0,'<','>'>>,position<1,0,'<','>'>>::value,"move right failed");

template<typename Pos>
using getfield = typename Pos::template apply_program<program>::instr;

/**
* Underflow, wrap to max
*/

/// x underflow, to final element of this row
template<typename Pos>
struct check_bounds<Pos,typename std::enable_if<Pos::x == -1>::type>
{
  using pos = typename check_bounds<typename Pos::template new_pos<Pos::f::template get<Pos::y>::size-1,Pos::y>>::pos;
};

/// y underflow, to last row with enough elements
template<typename Pos>
struct check_bounds<Pos,typename std::enable_if<Pos::y == -1>::type>
{
  template<int N, typename Enable = void>
  struct find_sufficient {static constexpr int value = find_sufficient<N-1>::value;};
  
  template<int N>
  struct find_sufficient<N, typename std::enable_if<Pos::f::template get<N>::size >= Pos::x>::type> {static constexpr int value = N;};

  static constexpr int sufficient = find_sufficient<Pos::f::size-1>::value;
  
  using pos = typename check_bounds<typename Pos::template new_pos<Pos::x,sufficient>>::pos;
};

static_assert(check_bounds<position<-1,0,'1','2','\n','3','4','5','\n','6','7'>>::pos::value == '2', "");
static_assert(check_bounds<position<0,-1,'1','2','\n','3','4','5','\n','6','7'>>::pos::value == '6', "");
static_assert(check_bounds<position<1,-1,'1','2','\n','3','4','5','\n','6','7'>>::pos::value == '7', "");
static_assert(check_bounds<position<-1,1,'1','2','\n','3','4','5','\n','6','7'>>::pos::value == '5', "");

/**
* Overflow scenarios, compare x to max row width instead of current row width
*/
///x overflow, back to start of row
template<typename Pos>
struct check_bounds<Pos,typename std::enable_if<(Pos::x == Pos::f::max_row_size)>::type>
{
  using pos = typename check_bounds<typename Pos::template new_pos<0,Pos::y>>::pos;
};

///y overflow, go to start of column
template<typename Pos>
struct check_bounds<Pos,typename std::enable_if<(Pos::y == Pos::f::size)>::type>
{
  template<int N, typename Enable = void>
  struct find_sufficient {static constexpr int value = find_sufficient<N+1>::value;};
  
  template<int N>
  struct find_sufficient<N, typename std::enable_if<(Pos::f::template get<N>::size > Pos::x)>::type> {static constexpr int value = N;};

  static constexpr int sufficient = find_sufficient<0>::value;
  
  using pos = typename check_bounds<typename Pos::template new_pos<Pos::x,sufficient>>::pos;
};

static_assert(check_bounds<position<3,1,'1','2','\n','3','4','5','\n','6','7'>>::pos::value == '3', "");
static_assert(check_bounds<position<0,3,'1','2','\n','3','4','5','\n','6','7'>>::pos::value == '1', "");
static_assert(check_bounds<position<3,2,'1','2','\n','3','4','5','\n','6','7'>>::pos::value == '6', "");
static_assert(check_bounds<position<2,3,'1','2','\n','3','4','5','\n','6','7','8'>>::pos::value == '5', "");

namespace instr_types {
  struct op {};
  struct branch {};
  struct set_movement {};
  struct internal_move {};
}

template<char C,typename Position, typename Movement, typename Enable = void>
struct impl;

namespace {
  std::vector<char> s;
}

template<typename Position, typename Movement>
struct befunge_impl;

template<typename Position, typename Movement>
using set_direction = befunge_impl<typename Movement::template next_pos<Position>,Movement>;

template<typename Position, typename Movement>
struct befunge_impl {
  using internal = impl<Position::value,Position,Movement>;
  
  static void run() {
    run_impl(internal{});
  };
  
  static void run_impl(instr_types::op) {
    internal::op(s);
	set_direction<Position,Movement>::run();
  };
  
  static void run_impl(instr_types::branch) {
    bool b = s.back() != 0;
	s.pop_back();
	if(b) {
	  set_direction<Position,typename internal::true_branch>::run();
	} else {
	  set_direction<Position,typename internal::false_branch>::run();
	}
  };
  
  static void run_impl(instr_types::set_movement) {
    using new_movement = typename internal::movement;
	set_direction<Position,new_movement>::run();
  };
  
  static void run_impl(instr_types::internal_move) {
    internal::run();
  };
  
};

template<typename Pos, typename Mov>
struct impl<'"',Pos,Mov> : instr_types::internal_move {
  template<char C, typename P, typename M>
  struct string_internal {
    static void run() {
	  s.push_back(C);
	  using next_pos = typename M::template next_pos<P>;
	  string_internal<next_pos::value,next_pos,M>::run();
	};
  };
  
  template<typename P, typename M>
  struct string_internal<'"',P,M> {
    static void run() {
	  set_direction<P,M>::run();
    } 
  };
  
  static void run() {
    using next_pos = typename Mov::template next_pos<Pos>;
    string_internal<next_pos::value,next_pos,Mov>::run();
  };
};

template<typename Pos, typename Mov>
struct impl<'#',Pos,Mov> : instr_types::internal_move {
  using next_pos = typename Mov::template next_pos<Pos>;
  static void run() {
    set_direction<next_pos,Mov>::run(); //skip next_pos
  };
};

template<typename Pos, typename Mov>
struct impl<'?',Pos,Mov> : instr_types::internal_move {
  static void run() {
    switch(std::rand()%4) {
	  case 0:
	  set_direction<Pos,move::up>::run();
	  break;
	  case 1:
	  set_direction<Pos,move::down>::run();
	  break;
	  case 2:
	  set_direction<Pos,move::left>::run();
	  break;
	  case 3:
	  set_direction<Pos,move::right>::run();
	}
  };
};

template<typename Pos, typename Mov>
struct impl<'@',Pos,Mov> : instr_types::internal_move {
  static void run() {}; //End program
};

template<typename Pos, typename Mov>
struct impl<'|',Pos,Mov> : instr_types::branch {
  using true_branch = move::up;
  using false_branch = move::down;
};

template<typename Pos, typename Mov>
struct impl<'_',Pos,Mov> : instr_types::branch {
  using true_branch = move::left;
  using false_branch = move::right;
};

template<typename Pos, typename Mov>
struct impl<'>',Pos,Mov> : instr_types::set_movement {
  using movement = move::right;
};

template<typename Pos, typename Mov>
struct impl<'<',Pos,Mov> : instr_types::set_movement {
  using movement = move::left;
};

template<typename Pos, typename Mov>
struct impl<'^',Pos,Mov> : instr_types::set_movement {
  using movement = move::up;
};

template<typename Pos, typename Mov>
struct impl<'v',Pos,Mov> : instr_types::set_movement {
  using movement = move::down;
};

template<typename Pos, typename Mov>
struct impl<' ',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>&) {};
};

template<char C, typename Pos, typename Mov>
struct impl<C,Pos,Mov,typename std::enable_if<(C >= '0' && C <= '9')>::type> : instr_types::op {
  static void op(std::vector<char>& stack) {
    stack.push_back(C-'0');
  };
};

template<typename Pos, typename Mov>
struct impl<'+',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
    auto v2 = stack.back();
	stack.pop_back();
	stack.back() += v2;
  };
};

template<typename Pos, typename Mov>
struct impl<'-',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
    auto v2 = stack.back();
	stack.pop_back();
	stack.back() -= v2;
  };
};

template<typename Pos, typename Mov>
struct impl<'*',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
    auto v2 = stack.back();
	stack.pop_back();
	stack.back() *= v2;
  };
};

template<typename Pos, typename Mov>
struct impl<'/',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
    auto v2 = stack.back();
	stack.pop_back();
	stack.back() /= v2;
  };
};

template<typename Pos, typename Mov>
struct impl<'%',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
    auto v2 = stack.back();
	stack.pop_back();
	stack.back() %= v2;
  };
};

template<typename Pos, typename Mov>
struct impl<'`',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
    auto v2 = stack.back();
	stack.pop_back();
	stack.back() = (stack.back() > v2);
  };
};

template<typename Pos, typename Mov>
struct impl<'!',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
    stack.back() = !stack.back();
  };
};

template<typename Pos, typename Mov>
struct impl<':',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
	stack.push_back(stack.back());
  };
};

template<typename Pos, typename Mov>
struct impl<'\\',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
    auto size = stack.size();
	std::swap(stack[size-1],stack[size-2]);
  };
};

template<typename Pos, typename Mov>
struct impl<'$',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
	stack.pop_back();
  };
};

template<typename Pos, typename Mov>
struct impl<'.',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
	std::printf("%d",(int)stack.back());
	stack.pop_back();
  };
};

template<typename Pos, typename Mov>
struct impl<',',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
	std::putchar(stack.back());
	stack.pop_back();
  };
};

template<typename Pos, typename Mov>
struct impl<'&',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
    int result;
	std::scanf("%d",&result);
	stack.push_back(result);
  };
};

template<typename Pos, typename Mov>
struct impl<'~',Pos,Mov> : instr_types::op {
  static void op(std::vector<char>& stack) {
	stack.push_back(std::getchar());
  };
};

template<char... P>
struct befunge
{
  using instructions = typename program<P...>::instr;
  void run() {s.clear();befunge_impl<position<0,0,P...>,move::right>::run();};
};

template<typename CharT, CharT... C> 
constexpr auto operator"" _befunge()
{
  return befunge<C...>{};
}

int main() {
std::srand(std::time(0));
R"foo( 025*"!dlrow ,olleH":v
                  v:,_@
                  >  ^)foo"_befunge.run();
R"foo(v>>>>>v
 12345
 ^?^
> ? ?^
 v?v
 6789
 >>>> v
^    .<)foo"_befunge.run();
}
