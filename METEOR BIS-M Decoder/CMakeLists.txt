cmake_minimum_required (VERSION 3.0.0)

project (METEOR-BIS-M-Decoder)
file(GLOB_RECURSE METEOR-BIS-M-Decoder_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(METEOR-BIS-M-Decoder ${METEOR-BIS-M-Decoder_CPPS})

target_include_directories(METEOR-BIS-M-Decoder PUBLIC src)

target_link_libraries(METEOR-BIS-M-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(METEOR-BIS-M-Decoder PUBLIC Threads::Threads)
