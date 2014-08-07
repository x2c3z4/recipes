#include <vector>
#include <memory>

#include <stdio.h>

#include "thirdparty/gtest/gtest.h"

TEST(vector,assginAt) {
  std::vector<std::string> v(101);
  printf("size = %d\n", v.size());
  v[100] = "a";
  printf("size = %d\n", v.size());
}
