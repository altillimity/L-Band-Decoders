cmake_minimum_required (VERSION 3.0.0)

project (MetOp-Decoder)

file(GLOB_RECURSE MetOp-Decoder_CPPS src/main.cpp src/viterbi.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(MetOp-Decoder ${MetOp-Decoder_CPPS})

target_include_directories(MetOp-Decoder PUBLIC src)

target_link_libraries(MetOp-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(MetOp-Decoder PUBLIC Threads::Threads)

install(TARGETS MetOp-Decoder DESTINATION bin)