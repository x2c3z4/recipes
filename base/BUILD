cc_library(
    name="base",
    srcs=[
    "Timers.cpp",
    "Looper.cc",
    ])

cc_test(
        name="test_BlockingQueue",
        srcs="test_BlockingQueue.cc"
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

