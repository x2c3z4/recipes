#include <stdio.h>

#include "test.h"

typedef struct{
  int a;
  int b[0];
}buf_t;

int main()
{
  buf_t t;
  int i;
  int b;
  t.b[0]=1;
  t.b[1]=2;
  //printf("sizeof(buf_t)=%d\t%d\t%d\t%d\t\n",sizeof(buf_t),t.b[0],t.b[1],b);
  d_debug("sizeof(buf_t)=%d\t%d\t%d\t%d\t\n",sizeof(buf_t),t.b[0],t.b[1],b);

  return 0;
}
