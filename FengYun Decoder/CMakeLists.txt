cmake_minimum_required (VERSION 3.0.0)

project (FengYun-Decoder)

file(GLOB_RECURSE FengYun-Decoder_CPPS src/diff.cpp src/main.cpp src/viterbi.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(FengYun-Decoder ${FengYun-Decoder_CPPS})

target_include_directories(FengYun-Decoder PUBLIC src)

target_link_libraries(FengYun-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(FengYun-Decoder PUBLIC Threads::Threads)

install(TARGETS FengYun-Decoder DESTINATION bin)