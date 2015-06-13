#include <stdio.h>

#define roundup_pow_of_two(n)			\
(						\
	__builtin_constant_p(n) ? (		\
		(n == 1) ? 1 :			\
		(1UL << (ilog2((n) - 1) + 1))	\
				   ) :		\
	__roundup_pow_of_two(n)			\
 )

int main()
{
  int n = 16;
  int rst = roundup_pow_of_two(n);
  printf("rst = %d\n", rst);
  return 0;
}
