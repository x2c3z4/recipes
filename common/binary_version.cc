/*
 * Copyright (c) 2014
 * Author: Feng,Li (lifeng1519@gmail.com)
 */

// Don't include header here, otherwise gcc 4.6.2+ will report:
// warning: 'weakref' attribute ignored because variable is initialized
// #include "common/binary_version.h"

#include "binary_version.h"

extern "C" {
namespace binary_version {
__attribute__((weak)) extern const int kSvnInfoCount = 0;
__attribute__((weak)) extern const char * const kSvnInfo[] = {0};
__attribute__((weak)) extern const char kBuildType[] = "Unknown";
__attribute__((weak)) extern const char kBuildTime[] = "Unknown";
__attribute__((weak)) extern const char kBuilderName[] = "Unknown";
__attribute__((weak)) extern const char kHostName[] = "Unknown";
__attribute__((weak)) extern const char kCompiler[] = "Unknown";
} // namespace binary_version
} // extern "C"
