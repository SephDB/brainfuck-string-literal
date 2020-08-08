#ifndef __WS_PARSE_H__
#define __WS_PARSE_H__

#include "ws_tokens.h"
#include "ws_ops.h"

template<typename...>
struct parse;

template<typename... T>
using parse_t = types_apply_t<Program, typename parse<T...>::program>;

template<typename... T> struct parseNum;
template<typename... T> struct parseLabel;

template<typename Instr, typename... T> struct parseSimpleInstr {
  using instr = Instr;
  using program = types_concat_t<types<instr>,typename parse<T...>::program>;
};

template<typename Instr, typename T> struct parseComplexInstr;
template<typename Instr, typename... T> struct parseComplexInstr<Instr,types<T...>> : public parseSimpleInstr<Instr,T...> {};

template<template<int> typename Instr, typename... T> struct parseIntInstr : public parseComplexInstr<Instr<parseNum<T...>::value>,typename parseNum<T...>::remainder> {};
template<template<typename> typename Instr, typename... T> struct parseLabelInstr : public parseComplexInstr<Instr<typename parseLabel<T...>::label>,typename parseLabel<T...>::remainder> {};

template<> struct parse<> {
  using program = types<>;
};

template<typename T, typename... Ts> struct parse<T,Ts...> { static_assert(!std::is_same<T,T>::value,"Mismatched input"); };

#define PARSE(KIND,INSTR,...) template<typename... T> struct parse<__VA_ARGS__,T...> : public parse##KIND##Instr<INSTR,T...> {}
#define SIM(INSTR,...) PARSE(Simple,INSTR,__VA_ARGS__)
#define INT(INSTR,...) PARSE(Int,INSTR,__VA_ARGS__)
#define LAB(INSTR,...) PARSE(Label,INSTR,__VA_ARGS__)

INT(Push,   A,A);
SIM(Dup,    A,C,A);
INT(Ref,    A,B,A);
INT(Slide,  A,B,C);
SIM(Swap,   A,C,B);
SIM(Discard,A,C,C);

SIM(Plus,   B,A,A,A);
SIM(Minus,  B,A,A,B);
SIM(Times,  B,A,A,C);
SIM(Divide, B,A,B,A);
SIM(Modulo, B,A,B,B);

SIM(Store,   B,B,A);
SIM(Retrieve,B,B,B);

LAB(Label,  C,A,A);
LAB(Call,   C,A,B);
LAB(Jump,   C,A,C);
LAB(IfZero, C,B,A);
LAB(IfNeg,  C,B,B);

SIM(Return, C,B,C);
SIM(End,    C,C,C);

SIM(OutputChar, B,C,A,A);
SIM(OutputNum,  B,C,A,B);
SIM(ReadChar,   B,C,B,A);
SIM(ReadNum,    B,C,B,B);

#undef PARSE
#undef SIM
#undef INT
#undef LAB

template<typename... T> struct computeNum;
template<typename Current, typename... T>
struct computeNum<Current,T...> {
  static constexpr int value = std::is_same<Current,B>::value*(1<<sizeof...(T)) + computeNum<T...>::value;
};
template<>
struct computeNum<> {
  static constexpr int value = 0;
};

template<typename Sign, typename... T>
struct parseNum<Sign,T...> {
  static constexpr int sign = std::is_same<Sign,B>::value ? -1 : 1;
  static constexpr int len = types_find_v<C,T...>;
  using num = types_take_t<len,T...>;
  
  
  using remainder = types_drop_t<len+1,T...>;
  static constexpr int value = sign*(types_apply_t<computeNum,num>::value);
};

template<typename... T> struct parseLabel {
  static constexpr int len = types_find_v<C,T...>;
  using label = types_take_t<len,T...>;
  using remainder = types_drop_t<len+1,T...>;
};

#endif