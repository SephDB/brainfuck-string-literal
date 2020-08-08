#ifndef __WS_EXEC_H__
#define __WS_EXEC_H__

#include <vector>
#include <unordered_map>
#include <iostream>
#include "types.h"
#include "ws_ops.h"

using Stack = std::vector<int>;
using Heap = std::unordered_map<int,int>;

static auto pop(Stack& s) {
  auto i = s.back();
  s.pop_back();
  return i;
};

static void push(Stack& s, int i) {
  s.push_back(i);
};

static auto& get(Stack& s, int pos) {
  return s.at(s.size()-1-pos);
};

static void drop(Stack& s, int n) {
  s.resize(s.size()-n);
};

static auto& retrieve(Heap& h, int i) {
  return h.at(i);
};

static void store(Heap& h, int loc, int val) {
  h[loc] = val;
};

struct ProgramEnd {};

//Used to find definite end points
struct EndFlow {};
struct IfTest {};

#define RUN(...) template<typename Prog> static void run(Stack& s, Heap& h) { __VA_ARGS__ }

template<int N> struct Push {RUN(push(s,N);)};
struct Dup {RUN(push(s,get(s,0));)};
template<int N> struct Ref {RUN(push(s,get(s,N));)};
template<int N> struct Slide {RUN(auto n = pop(s); drop(s,N); push(s,n);)};
struct Swap {RUN(std::swap(get(s,0),get(s,1));)};
struct Discard {RUN(pop(s);)};

#define OP(Op) RUN(auto a = pop(s); auto b = pop(s); push(s,a Op b);)
struct Plus {OP(+)};
struct Minus {OP(-)};
struct Times {OP(*)};
struct Divide {OP(/)};
struct Modulo {OP(%)};
#undef OP

struct Store {RUN(auto n = pop(s); auto loc = pop(s); store(h,loc,n);)};
struct Retrieve {RUN(auto loc = pop(s); push(s,retrieve(h,loc));)};

struct OutputChar {RUN(std::cout << static_cast<char>(pop(s));)};
struct OutputNum {RUN(std::cout << pop(s);)};
struct ReadChar {RUN(char c; std::cin >> c; store(h,pop(s),c);)};
struct ReadNum {RUN(int i; std::cin >> i; store(h,pop(s),i);)};

template<typename L> struct Label {RUN()}; //deliberately empty
template<typename L> struct Call {RUN(Prog::template run_label<L>(s,h);)};

template<typename L> struct Jump : public EndFlow {RUN(Prog::template run_label<L>(s,h);)};
struct Return : public EndFlow {RUN()}; //deliberately empty
struct End : public EndFlow {RUN(throw ProgramEnd{};)};

template<typename L> struct IfZero : public IfTest {
  static bool test(int i) {return i == 0;};
  using label = L;
};
template<typename L> struct IfNeg : public IfTest {
  static bool test(int i) {return i < 0;};
  using label = L;
};

#undef RUN

template<typename T>
using base_test = types_curry<std::is_base_of,T>;

template<typename T, typename... Types>
constexpr auto find_if_base_v = types_find_if_v<types_curry<std::is_base_of,T>::template apply_one, Types...>;

template<typename Kind, typename... Instr>
struct split_base {
  static constexpr auto loc = find_if_base_v<Kind,Instr...>;
  
  using front = types_take_t<loc,Instr...>;
  using back = types_drop_t<loc,Instr...>;
};

template<typename...>
struct Program;

//Implemented outside of Program bc of use of Program template in apply        V
template<int Pos, typename... Instr>
using label_block_impl = typename types_drop_t<Pos,Instr...>::template apply<Program>::endflow_block;

template<typename... Instr>
struct Program {
  static constexpr auto find_endflow = find_if_base_v<EndFlow,Instr...>;
  
  static_assert(find_endflow < sizeof...(Instr), "No Jump/End instruction at end of input");
  
  using endflow_block = types_take_t<find_endflow+1,Instr...>;
  
  template<typename L>
  static constexpr auto find_label = types_find_v<Label<L>,Instr...>;
  
  template<typename L>
  using label_block = label_block_impl<find_label<L>,Instr...>;
  
  //Splits execution of non-endflow into straight code between branches
  template<typename Dummy, typename... I>
  struct BranchDetector_impl;
  
  template<typename... I>
  using BranchDetector = BranchDetector_impl<void,I...>;
  
  static void run() {
    Stack s;
    Heap h;
    try {
      types_apply_t<BranchDetector,endflow_block>::run(s,h);
      std::cerr << "Improper shutdown of program" << std::endl;
    } catch(ProgramEnd) {
      return;
    }
  };
  
  ///////////////////////////////////////////
  ////////Implementation details/////////////
  ///////////////////////////////////////////
  
  template<typename L>
  static void run_label(Stack& s, Heap& h) {
    types_apply_t<BranchDetector,label_block<L>>::run(s,h);
  };
  
  //Executes a straight bit of instructions
  template<typename... I>
  struct Exec;
  
  //Executes a list of instructions conditionally, based on result of first instruction being a branch
  template<typename Dummy, typename... I>
  struct BranchExec_impl;
  
  template<typename... I>
  using BranchExec = BranchExec_impl<void,I...>;
  
  template<typename Dummy, typename... I>
  struct BranchDetector_impl {
    static constexpr auto if_loc = find_if_base_v<IfTest,I...>;
  
    using front = types_take_t<if_loc,I...>;
    using back = types_drop_t<if_loc,I...>;
    
    static void run(Stack& s, Heap& h) {
      types_apply_t<Exec,front>::run(s,h);
      types_apply_t<BranchExec,back>::run(s,h);
    };
  };
  
  template<typename Dummy>
  struct BranchDetector_impl<Dummy> {
    static void run(Stack&,Heap&) {};
  };
  
  template<typename Dummy, typename If, typename... I>
  struct BranchExec_impl<Dummy,If,I...> {
    static void run(Stack& s,Heap& h) {
      if(If::test(pop(s))) {
        run_label<typename If::label>(s,h);
      } else {
        BranchDetector<I...>::run(s,h);
      }
    };
  };
  
  template<typename Dummy>
  struct BranchExec_impl<Dummy> {
    static void run(Stack&,Heap&) {};
  };
  
  template<typename... I>
  struct Exec {
    static void run(Stack& s, Heap& h) {
      (void)std::initializer_list<int>{ (I::template run<Program>(s,h),0)... };
    };
  };
};

#endif