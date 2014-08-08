#include <stdio.h>

#include <vector>
#include <memory>

#include "thirdparty/gtest/gtest.h"

TEST(vector,assginAt) {
  std::vector<std::string> v(101);
  printf("size = %zu\n", v.size());
  v[100] = "a";
  printf("size = %zu\n", v.size());
}
