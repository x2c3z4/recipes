#include <vector>
#include <memory>

#include <stdio.h>

#include "thirdparty/gtest/gtest.h"

#include "Looper.h"

void readMessage() {
  printf("read done!\n");
}
TEST(Looper, createFileEvent) {
  base::Looper loop;
  base::Functor func = readMessage;
  loop.createFileEvent(1, AE_READABLE, func, NULL);
  loop.loop();
}
