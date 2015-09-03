#include <stdint.h>
#include <stdio.h>

#define min(x, y) ({        \
    typeof(x) _min1 = (x);      \
    typeof(y) _min2 = (y);      \
    (void) (&_min1 == &_min2);    \
    _min1 < _min2 ? _min1 : _min2; })

#define min2(x, y) ({        \
    x < y ? x : y;})
int main()
{
  int a = 5;
  int64_t b =6;
  int i;
// printf("min()=%d\n", min(a, b));
  printf("min()=%d\n", min2(a, b));

  int c[] = {[0 ... 1] = 1, [2 ... 3] = 2, [4 ... 5] = 3};
  for (i = 0; i < sizeof(c)/sizeof(c[0]); ++i) {
    printf("%d\n", c[i]);
  }

  return 0;
}
