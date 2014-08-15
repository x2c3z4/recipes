cc_library(
    name = "net",
    srcs = [
    "Looper.cc",
    "TcpServer.cc",
    "Socket.cc",
    ],
    deps = [
    "//base:base",
    "//thirdparty/glog:glog",
    ])

cc_test(
        name="test_looper",
        srcs="test_looper.cc",
        deps = [
        "//base:base",
        "//net:net",
        ]
        )

cc_test(
    name="test_tcpserver",
    srcs="test_tcpserver.cc",
    deps = [
    "//base:base",
    ":net",
    ]
    )
