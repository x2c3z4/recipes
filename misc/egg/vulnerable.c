#include <string.h>
#include <stdio.h>
#include "shellcode.h"

void main(int argc, char *argv[]) {
  char buffer[50];

  printf("EGG addr:0x%lx\n",(unsigned long)getenv("EGG"));
  if (argc > 1)
    strcpy(buffer,argv[1]);
}
