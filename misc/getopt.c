#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> //SIGTERM

typedef struct _state
{
  int parallelism;
  int repetitions;
  int sleepus;
  int min_bytes;
  int max_bytes;
  int step_size;
  pid_t *pid;
}state_t;

state_t state[]={
  { 1,5,0,10,1000,100 },
  { 1,5,0,1024,1048576/* 1M */,1024 },
  { 1,50,0,100,100,0 },
  { 1,50,0,1024,1024,0 },
  /*  */
  { 1,5,20,10,1000,100 },
  { 1,5,20,1024,1048576,1024 },
  { 1,50,20,100,100,0 },
  { 1,50,20,1024,1024,0 },


  { 1,5,200,10,1000,100 },
  { 1,5,200,1024,1048576,1024 },
  { 1,50,200,100,100,0 },
  { 1,50,200,1024,1024,0 },


  { 1,5,2000,10,1000,100 },
  { 1,5,2000,1024,1048576,1024 },
  { 1,50,2000,100,100,0 },
  { 1,50,2000,1024,1024,0 },

};

void bench_child(state_t *state);
void bench_parent(state_t *state);

void bench(state_t *state)
{
  int i;
  state->pid=(pid_t*)malloc(sizeof(pid_t) * state[i].parallelism);
  if(  state->pid == NULL){
    fprintf(stderr, "malloc pid failed\n");
    exit(-1);
  }

  for (i = 0; i < state->parallelism;++i) {
    //fprintf(stderr, "benchmp(%p, %p, %p, %d, %d, %d, %d, %p): creating child %d\n", initialize, benchmark, cleanup, enough, parallel, warmup, repetitions, cookie, i);
    switch(state->pid[i] = fork()) {
      case -1:
        /* could not open enough children! */
        fprintf(stderr, "bench: fork() failed!\n");
        goto error_exit;
      case 0:
        /* If child */
        bench_child(state);
        exit(0);
      default:
        break;
    }
  }
  bench_parent(state);
  goto cleanup_exit;
error_exit:
  while (--i >= 0) {
    kill(state->pid[i], SIGTERM);
    waitpid(state->pid[i], NULL, 0);
  }
cleanup_exit:
  while (--i >= 0) {
    waitpid(state->pid[i], NULL, 0);
  }
  if(state->pid)
    free((void*)(state->pid));
}
void bench_child(state_t *state)
{
  int i,j,flag=0;//flag=0 : one way ;1:two way
  for(j=state->min_bytes;j<=state->max_bytes;j+=state->step_size){
    start_time();
    bench_send();
    for(i=0;i<state->repetitions-1;++i){
      bench_send();
    }
  }

  sleep(10);
  flag=1;
  for(j=state->min_bytes;j<=state->max_bytes;j+=state->step_size){
    start_time();
    for(i=0;i<state->repetitions;++i){
      bench_send(flag);
    }
    stop_time();
    bench_out();
  }
}

int main(int ac,char **av)
{
  int i;
  for(i=0;i<sizeof(state)/sizeof(state[0]);++i){
    bench(&state[i]);
  }
  return 0;
}

void parse_argu(int ac,char *av)
{
  /*
     int c;
     char  *usage = "[-P <parallelism>] [-N <repetitions>] [-S <sleepus>] []<size> open2close|mmap_only <filename>";
     while (( c = getopt(ac, av, "P:W:N:C")) != EOF) {
     switch(c) {
     case 'P':
     parallel = atoi(optarg);
     if (parallel <= 0) lmbench_usage(ac, av, usage);
     break;
     case 'W':
     warmup = atoi(optarg);
     break;
     case 'N':
     repetitions = atoi(optarg);
     break;
     default:
     lmbench_usage(ac, av, usage);
     break;
     }
     }

  // should have three arguments left (bytes type filename) 
  if (optind + 3 != ac) {
  lmbench_usage(ac, av, usage);
  }
  */
}
void lmbench_usage(int argc, char *argv[], char* usage)
{
  fprintf(stderr,"Usage: %s %s", argv[0], usage);
  exit(-1);
}
