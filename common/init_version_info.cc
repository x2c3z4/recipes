/*
 * Copyright (c) 2014
 * Author: Feng,Li (lifeng1519@gmail.com)
 */

#include <sstream>
#include "binary_version.h"
#include "thirdparty/gflags/gflags.h"

static std::string MakeVersionInfo()
{
    using namespace binary_version;

    std::ostringstream oss;
    oss << "\n"; // Open a new line in gflags --version output.

    if (kSvnInfoCount > 0)
    {
        oss << "----------------------------------------------------------\n";
        for (int i = 0; i < kSvnInfoCount; ++i)
            oss << kSvnInfo[i];
        oss << "----------------------------------------------------------\n";
    }

    oss << "BuildTime: " << kBuildTime << "\n"
        << "BuildType: " << kBuildType << "\n"
        << "BuilderName: " << kBuilderName << "\n"
        << "HostName: " << kHostName << "\n"
        << "Compiler: " << kCompiler << "\n";

    return oss.str();
}

void InitVersionInfo()
{
    google::SetVersionString(MakeVersionInfo());
}
