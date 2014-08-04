cc_library(
    name = 'blocking_queue',
    srcs = [
    'blocking_queue.cc',
    ],
    )
cc_test(
    name = 'test_blocking_queue',
    srcs = [
    'test_blocking_queue.cc',
    ],
    deps = [
    ":blocking_queue",
    "//thirdparty/gtest:gtest_main",
    ],
    )
