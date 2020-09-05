cmake_minimum_required (VERSION 3.0.0)

project (FengYun)

file(GLOB_RECURSE FengYun_CPPS src/diff.cpp src/main.cpp src/viterbi.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(FengYun ${FengYun_CPPS})

target_include_directories(FengYun PUBLIC src)

target_link_libraries(FengYun PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(FengYun PUBLIC Threads::Threads)
