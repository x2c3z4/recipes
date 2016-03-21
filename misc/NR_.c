#include <stdio.h>
#include <sys/timerfd.h>
// check default include path: gcc -xc -E -v -
//
int main() {
#if defined(__NR_timerfd_create) && defined(USE_TIMERFD)
printf(" defined one of two \n");
#endif

#if defined(__NR_timerfd_create)
printf(" defined __NR_timerfd_create\n");
#endif

#if defined(USE_TIMERFD)
printf("defined USE_TIMERFD\n");
#endif
#if defined(__NR_signalfd) && defined(USE_SIGNALFD)
printf("defined __NR_signalfd or USE_SIGNALFD\n");
#endif
return 0;
}
