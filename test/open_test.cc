#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h> //getchar
#include <unistd.h>
#include <assert.h>

#include "bench_time.h"

int main() {
  const long count = 100;
  double start;
  char files[count][25];
  int fds[count];

  for(int i = 0; i < count; ++i) {
    snprintf(files[i], 25, "/tmp/1/test.%d",i);
    // printf("%s\n", files[i]);
  }

  start = bench_begin();
  for (int i = 0; i < count; ++i) {
    fds[i] = open(files[i], O_CREAT);
    assert(fds[i] > 0);
  }
  bench_end(start, "create", count);
  getchar();


  start = bench_begin();
  for (int i = 0; i < count; ++i) {
    fds[i] = open(files[i], O_RDWR);
    assert(fds[i] > 0);
  }
  bench_end(start, "open", count);
  getchar();


  start = bench_begin();
  for (int i = 0; i < count; ++i) {
    write(fds[i], "hello world", 12);
  }
  bench_end(start, "write", count);
  getchar();

  char buf[15];
  start = bench_begin();
  for (int i = 0; i < count; ++i) {
    read(fds[i], buf, 15);
  }
  bench_end(start, "read", count);
  getchar();


  start = bench_begin();
  for (int i = 0; i < count; ++i) {
    close(fds[i]);
  }
  bench_end(start, "close", count);
  getchar();

  start = bench_begin();
  for (int i = 0; i < count; ++i) {
    unlink(files[i]);
  }
  bench_end(start, "delete", count);
  getchar();
  return 0;
}
