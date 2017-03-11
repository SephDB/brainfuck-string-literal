#ifndef __WS_OPS_H__
#define __WS_OPS_H__

template<int N> struct Push;
struct Dup;
template<int N> struct Ref;
template<int N> struct Slide;
struct Swap;
struct Discard;
template<typename T> struct Infix;
struct Plus;
struct Minus;
struct Times;
struct Divide;
struct Modulo;
struct Store;
struct Retrieve;
template<typename L> struct Label;
template<typename L> struct Call;
template<typename L> struct Jump;
template<typename L> struct IfZero;
template<typename L> struct IfNeg;
struct Return;
struct OutputChar;
struct OutputNum;
struct ReadChar;
struct ReadNum;
struct End;

template<typename... Instr> struct Program;

#endif