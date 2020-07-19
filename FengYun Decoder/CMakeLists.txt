cmake_minimum_required (VERSION 3.0.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project (FengYun)
file(GLOB_RECURSE FengYun_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(FengYun ${FengYun_CPPS})

target_include_directories(FengYun PUBLIC src)

target_link_libraries(FengYun PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(FengYun PUBLIC Threads::Threads)
