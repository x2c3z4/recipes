#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


typedef unsigned int   u32int;
typedef          int   s32int;
typedef unsigned short u16int;
typedef          short s16int;
typedef unsigned char  u8int;
typedef          char  s8int;

#define LONG_SIZE 4
#define PRIME_INC 514229
//#define PRIME_INC 4096

#define PAGE_SIZE  (4*1024)
#define ONE_GIG  (1024*1024*1024)
#define TWO_GIG  ((u32int)2 * ONE_GIG)
#define ARRAY_SIZE  (ONE_GIG/LONG_SIZE)

#define WORDS_PER_PAGE  (PAGE_SIZE / LONG_SIZE)
#define ARRAY_MASK  (ARRAY_SIZE - 1)
#define PAGE_MASK  (WORDS_PER_PAGE - 1)

#define N 5
typedef u32int (*next_func_t)(u32int pageOffset,u32int wordOffset,u32int pos);

u32int linear_walk(u32int pageOffset,u32int wordOffset,u32int pos)
{
  return (pos+1) & ARRAY_MASK;
}

u32int random_page_walk(u32int pageOffset,u32int wordOffset,u32int pos)
{
  return (pageOffset + ((pos + PRIME_INC) & PAGE_MASK)) & ARRAY_MASK;
}

u32int random_heap_walk(u32int pageOffset,u32int wordOffset,u32int pos)
{
  return (pos + PRIME_INC) & ARRAY_MASK;
}

next_func_t nexts[3] = {linear_walk,random_page_walk,random_heap_walk};
next_func_t next;

int main()
{
  u32int *memory = malloc(sizeof(u32int) * (u32int)ARRAY_SIZE);
  u32int i,walk_type=0,iter,pageOffset,wordOffset,limit,pos;
  long long result;
  clock_t start,end;
	
  for(i=0;i<ARRAY_SIZE;i++){
    memory[i] = 777;
  }


  for(walk_type = 0;walk_type <3;walk_type++){
    next = nexts[walk_type];
    for(iter = 0;iter < N;iter++){
      start = clock();
      for(pageOffset = 0; pageOffset <ARRAY_SIZE; pageOffset += WORDS_PER_PAGE){
        for(wordOffset = pageOffset,limit = pageOffset + WORDS_PER_PAGE;wordOffset < limit;wordOffset++){
          pos = next(pageOffset,wordOffset,pos);
          result += memory[pos];
        }
      }
      end = clock();
      printf("%d - %.2fs walk_type:%d\n",iter,1.0*(end-start)/CLOCKS_PER_SEC,walk_type);
    }
  }

  if(memory)
    free(memory);
}
