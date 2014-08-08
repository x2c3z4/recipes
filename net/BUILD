cc_library(
    name = "net",
    srcs = [
    "TcpServer.cc",
    "Socket.cc",
    ],
    deps = [
    "//base:base",
    "//thirdparty/glog:glog",
    ])

cc_test(
    name="test_tcpserver",
    srcs="test_tcpserver.cc",
    deps = [
    "//base:base",
    ":net",
    ]
    )
