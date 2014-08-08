cc_library(
    name="base",
    srcs=[
    "Timers.cpp",
    "Looper.cc",
    "Thread.cc",
    ])

cc_test(
        name="test_blockingqueue",
        srcs="test_blockingqueue.cc"
        )
cc_test(
        name="test_vector",
        srcs="test_vector.cc",
)
cc_test(
        name="test_looper",
        srcs="test_looper.cc",
        deps = [
        ":base",
        ]
        )
cc_test(
        name="test_thread",
        srcs="test_thread.cc",
        deps = [
        ":base",
        ]
        )

