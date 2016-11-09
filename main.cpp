#include "bf.h"

template<typename CharT, CharT... C> 
constexpr auto operator"" _b()
{
  return bf<C...>{};
}

int main()
{
  "-[++>]++"_b.run();
}