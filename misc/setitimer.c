#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

void print_elapsed_time(void);
void timer_handler(int signum)
{
  fflush(NULL);
  print_elapsed_time();
}
void print_elapsed_time(void)
{
  static struct timespec start;
  struct timespec curr;
  static int first_call = 1;
  int secs, nsecs;

  if (first_call) {
    first_call = 0;
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
      perror("clock_gettime");
  }

  if (clock_gettime(CLOCK_MONOTONIC, &curr) == -1)
    perror("clock_gettime");

  secs = curr.tv_sec - start.tv_sec;
  nsecs = curr.tv_nsec - start.tv_nsec;
  if (nsecs < 0) {
    secs--;
    nsecs += 1000000000;
  }
  printf("%d.%03d: expire\n", secs, (nsecs + 500000) / 1000000);
  fflush(NULL);
}
int main()
{
  struct sigaction s;
  int err;
  struct itimerval worker_timer = {
    {1,0},
    {5,0}
  };
  sigemptyset(&s.sa_mask);
  sigaddset(&s.sa_mask, SIGALRM);
  s.sa_flags  = 0;
  s.sa_handler = timer_handler;

  err =sigaction(SIGALRM, &s, NULL);
  if (err) {printf("error\n");exit(1);}
  err = setitimer(ITIMER_REAL, &worker_timer, 0);
  if (err) {printf("error\n");exit(1);}
  for(;;) {

  }
  return 0;
}
