add_executable(SuperString.test main.cc)

target_link_libraries(SuperString.test SuperString)

add_executable(SuperString.test.compare bench_compare.cc)
target_link_libraries(SuperString.test.compare SuperString benchmark)

add_executable(SuperString.withSS withSS.cc)
target_link_libraries(SuperString.withSS SuperString)

add_executable(SuperString.withStd withStd.cc)
target_link_libraries(SuperString.withStd)
