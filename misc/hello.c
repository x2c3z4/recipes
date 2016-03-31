#include <stdio.h>
#include <stdint.h> //for intptr_t ,added in C99

#define PRINT_SIZE(type) printf("sizeof(%s)=%d\n",#type,sizeof(type))
int main()
{
  PRINT_SIZE(int);
  PRINT_SIZE(long);
  PRINT_SIZE(intptr_t);
  PRINT_SIZE(void*);
  PRINT_SIZE(long long);
  PRINT_SIZE(size_t);

  return 0;
}
