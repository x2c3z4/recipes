/*
 * Copyright (c) 2014
 * Author: Feng,Li (lifeng1519@gmail.com)
 */

#ifndef COMMON_BINARY_VERSION_H
#define COMMON_BINARY_VERSION_H
#pragma once

extern "C" {
namespace binary_version {
extern const int kSvnInfoCount;
extern const char * const kSvnInfo[];
extern const char kBuildType[];
extern const char kBuildTime[];
extern const char kBuilderName[];
extern const char kHostName[];
extern const char kCompiler[];
}
}

// Set version information to gflags, which make --version print usable
// text.
void InitVersionInfo();

#endif // COMMON_BINARY_VERSION_H
