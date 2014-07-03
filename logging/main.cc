#include <iostream>
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

using namespace std;

DEFINE_int32(qps, 10, "test qps");

int main(int argc, char * argv[])
{
        google::InitGoogleLogging(argv[0]);
        google::ParseCommandLineFlags(&argc, &argv, false);
        cout << "test qps:" << FLAGS_qps << endl;
        LOG(INFO) << "INFO" << endl;
        LOG(ERROR) << "ERROR" << endl;
        LOG(WARNING) << "WARNING" << endl;
        return 0;
}
