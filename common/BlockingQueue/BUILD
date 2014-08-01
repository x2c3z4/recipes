cc_library(
    name = 'blocking_queue',
    srcs = [
    'BlockingQueue.cc',
    ],
    )
cc_test(
    name = 'test_blocking_queue',
    srcs = [
    'test_BlockingQueue.cc',
    ],
    deps = [
    ":blocking_queue",
    "//thirdparty/gtest:gtest_main",
    ],
    )
