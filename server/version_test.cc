/*
 * Copyright (c) 2014
 * Author: Feng,Li (lifeng1519@gmail.com)
 */

#include "common/binary_version.h"

#include "thirdparty/gflags/gflags.h"

int main(int argc, char *argv[]) {
  InitVersionInfo();
  google::ParseCommandLineFlags(&argc, &argv, true);
  return 0;
}
